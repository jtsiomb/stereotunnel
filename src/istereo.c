/*
Stereoscopic tunnel for iOS.
Copyright (C) 2011-2015  John Tsiombikas <nuclear@member.fsf.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include "opengl.h"
#include "istereo.h"
#include "sanegl.h"
#include "sdr.h"
#include "respath.h"
#include "tex.h"
#include "cam.h"
#include "vmath.h"
#include "config.h"
#include "ui.h"
#include "drawtext.h"
#include "timer.h"

static void render(float t);
static void draw_tunnel(float t);
static void tunnel_vertex(float u, float v, float du, float dv, int tang_loc, float t);
static vec3_t calc_text_pos(float sec);
static void draw_text(float idx, vec3_t tpos, float alpha);
static void worm(float t, float z, float *tx, float *ty);
static unsigned int get_shader_program(const char *vfile, const char *pfile);

unsigned int prog, prog_simple, prog_tunnel, prog_text, prog_color, prog_ui, prog_font;
unsigned int tex, tex_stones, tex_normal, tex_text;
struct dtx_font *font;

int view_xsz, view_ysz;
float view_aspect;

int stereo = 0;
int use_bump = 0;
int show_opt = 1;

/* construction parameters */
int sides = 24;
int segm = 20;
float tunnel_speed = 0.75;
float ring_height = 0.5;
float text_period = 13.0;
float text_speed = 2.2;

float split = 0.5275;

int init(void)
{
	add_resource_path("sdr");
	add_resource_path("data");

	if(!(prog_simple = get_shader_program("test.v.glsl", "test.p.glsl"))) {
		return -1;
	}
	if(!(prog_tunnel = get_shader_program("tunnel.v.glsl", "tunnel.p.glsl"))) {
		return -1;
	}
	if(!(prog_text = get_shader_program("text.v.glsl", "text.p.glsl"))) {
		return -1;
	}
	if(!(prog_color = get_shader_program("color.v.glsl", "color.p.glsl"))) {
		return -1;
	}
	if(!(prog_ui = get_shader_program("ui.v.glsl", "ui.p.glsl"))) {
		return -1;
	}
	if(!(prog_font = get_shader_program("ui.v.glsl", "font.p.glsl"))) {
		return -1;
	}

	if(!(tex = load_texture(find_resource("tiles.jpg", 0, 0)))) {
		return -1;
	}
	if(!(tex_stones = load_texture(find_resource("stonewall.jpg", 0, 0)))) {
		return -1;
	}
	if(!(tex_normal = load_texture(find_resource("stonewall_normal.jpg", 0, 0)))) {
		return -1;
	}
	if(!(tex_text = load_texture(find_resource("text.png", 0, 0)))) {
		return -1;
	}

	if(!(font = dtx_open_font_glyphmap(find_resource("linux-libertine_s24.glyphmap", 0, 0)))) {
		fprintf(stderr, "failed to load font\n");
		return -1;
	}
	dtx_vertex_attribs(get_attrib_loc(prog_ui, "attr_vertex"), get_attrib_loc(prog_ui, "attr_texcoord"));
	dtx_use_font(font, 24);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	if(ui_init() == -1) {
		return -1;
	}

	cam_fov(42.5);
	cam_clip(0.5, 250.0);

	return 0;
}

void cleanup(void)
{
	ui_shutdown();
	free_program(prog_simple);
	free_program(prog_tunnel);
	free_program(prog_color);
	free_program(prog_ui);
	free_program(prog_font);
	dtx_close_font(font);
}

void redraw(void)
{
	float pan_x, pan_y, z;
	float tsec = get_time_sec();

	z = ring_height * segm;
	worm(tsec, z, &pan_x, &pan_y);

	if(use_bump) {
		glClearColor(0.01, 0.01, 0.01, 1.0);
		tunnel_speed = 0.5;
	} else {
		glClearColor(0.6, 0.6, 0.6, 1.0);
		tunnel_speed = 0.75;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(stereo) {
		int split_pt = (int)((float)view_xsz * split);

		/* right eye */
		glViewport(0, 0, split_pt, view_ysz);
		cam_aspect((float)split_pt / (float)view_ysz);

		gl_matrix_mode(GL_PROJECTION);
		gl_load_identity();
		cam_stereo_proj_matrix(CAM_RIGHT);
		//gl_rotatef(-90, 0, 0, 1);

		gl_matrix_mode(GL_MODELVIEW);
		gl_load_identity();
		cam_stereo_view_matrix(CAM_RIGHT);
		gl_translatef(-pan_x, -pan_y, -1.1 * ring_height * segm);

		render(tsec);

		/* left eye */
		glViewport(split_pt, 0, view_xsz - split_pt, view_ysz);
		cam_aspect((float)(view_xsz - split_pt) / (float)view_ysz);

		gl_matrix_mode(GL_PROJECTION);
		gl_load_identity();
		cam_stereo_proj_matrix(CAM_LEFT);
		//gl_rotatef(-90, 0, 0, 1);

		gl_matrix_mode(GL_MODELVIEW);
		gl_load_identity();
		cam_stereo_view_matrix(CAM_LEFT);
		gl_translatef(-pan_x, -pan_y, -1.1 * ring_height * segm);

		render(tsec);
	} else {
		glViewport(0, 0, view_xsz, view_ysz);
		cam_aspect((float)view_xsz / (float)view_ysz);

		gl_matrix_mode(GL_PROJECTION);
		gl_load_identity();
		//gl_rotatef(-90, 0, 0, 1);
		cam_proj_matrix();

		gl_matrix_mode(GL_MODELVIEW);
		gl_load_identity();
		cam_view_matrix();
		gl_translatef(-pan_x, -pan_y, -1.1 * ring_height * segm);

		render(tsec);
	}

	/* TEST */
	/*bind_program(prog_ui);

	gl_matrix_mode(GL_PROJECTION);
	gl_load_identity();
	gl_ortho(0, view_xsz, 0, view_ysz, -1, 1);
	gl_matrix_mode(GL_MODELVIEW);
	gl_load_identity();
	gl_apply_xform(prog_ui);

	glDisable(GL_DEPTH_TEST);
	dtx_printf("hello world\n");
	glEnable(GL_DEPTH_TEST);*/

	assert(glGetError() == GL_NO_ERROR);
}

static void render(float t)
{
	int i;
	float text_line;

	draw_tunnel(t);

	if(use_bump) {
		glDepthMask(0);
		text_line = floor((text_speed * t) / text_period);
		for(i=0; i<8; i++) {
			vec3_t tpos = calc_text_pos(t - (float)i * 0.011);
			draw_text(text_line, tpos, 1.5 / (float)i);
		}
		glDepthMask(1);
	}

	if(show_opt) {
		ui_draw();
	}
}

static void draw_tunnel(float t)
{
	static const float uoffs[] = {0.0, 0.0, 1.0, 1.0};
	static const float voffs[] = {0.0, 1.0, 1.0, 0.0};
	int i, j, k, tang_loc = -1;
	float du, dv;

	prog = use_bump ? prog_tunnel : prog_simple;

	bind_program(prog);
	set_uniform_float(prog, "t", t);

	if(use_bump) {
		vec3_t ltpos = calc_text_pos(t);

		bind_texture(tex_normal, 1);
		set_uniform_int(prog, "tex_norm", 1);
		bind_texture(tex_stones, 0);
		set_uniform_int(prog, "tex", 0);

		set_uniform_float4(prog, "light_pos", ltpos.x, ltpos.y, ltpos.z, 1.0);
		tang_loc = get_attrib_loc(prog, "attr_tangent");
	} else {
		bind_texture(tex, 0);
		set_uniform_int(prog, "tex", 0);
	}

	gl_matrix_mode(GL_TEXTURE);
	gl_load_identity();
	gl_translatef(0, -fmod(t * tunnel_speed, 1.0), 0);

	gl_begin(GL_QUADS);
	gl_color3f(1.0, 1.0, 1.0);

	du = 1.0 / sides;
	dv = 1.0 / segm;

	for(i=0; i<segm; i++) {
		float trans_zp[2], trans_z0[2], trans_z1[2];

		float zp = ring_height * (i - 1);
		float z0 = ring_height * i;
		float z1 = ring_height * (i + 1);

		worm(t, zp, trans_zp, trans_zp + 1);
		worm(t, z0, trans_z0, trans_z0 + 1);
		worm(t, z1, trans_z1, trans_z1 + 1);

		for(j=0; j<sides; j++) {
			for(k=0; k<4; k++) {
				float u = (j + uoffs[k]) * du;
				float v = (i + voffs[k]) * dv;

				tunnel_vertex(u, v, du, dv, tang_loc, t);
			}
		}
	}
	gl_end();

	bind_texture(0, 1);
	bind_texture(0, 0);
}

static void tunnel_vertex(float u, float v, float du, float dv, int tang_loc, float t)
{
	vec3_t pos, norm;
	vec3_t dfdu, dfdv, pos_du, pos_dv;

	float theta = 2.0 * M_PI * u;
	float theta1 = 2.0 * M_PI * (u + du);

	float x = cos(theta);
	float y = sin(theta);
	float x1 = cos(theta1);
	float y1 = sin(theta1);
	float z = v / dv * ring_height;
	float z1 = (v + dv) / dv * ring_height;

	float trans_z[2], trans_z1[2];

	worm(t, z, trans_z, trans_z + 1);
	worm(t, z1, trans_z1, trans_z1 + 1);

	pos = v3_cons(x + trans_z[0], y + trans_z[1], z);
	pos_du = v3_cons(x1 + trans_z[0], y1 + trans_z[1], z);
	pos_dv = v3_cons(x + trans_z1[0], y + trans_z1[1], z1);

	dfdu = v3_sub(pos_du, pos);
	dfdv = v3_sub(pos_dv, pos);
	norm = v3_cross(dfdv, dfdu);

	gl_vertex_attrib3f(tang_loc, dfdu.x, -dfdu.y, dfdu.z);
	gl_normal3f(norm.x, norm.y, norm.z);
	gl_texcoord2f(u * 2.0, v * 4.0);
	gl_vertex3f(pos.x, pos.y, pos.z);
}

static vec3_t calc_text_pos(float sec)
{
	float t = text_speed * sec;
	float z = fmod(t, text_period);
	float pan[2];

	worm(sec, z, pan, pan + 1);
	return v3_cons(pan[0], pan[1], z + ring_height);
}

static void draw_text(float idx, vec3_t tpos, float alpha)
{
	gl_matrix_mode(GL_MODELVIEW);
	gl_push_matrix();
	gl_translatef(tpos.x, tpos.y, tpos.z);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	bind_program(prog_text);
	set_uniform_float(prog_text, "idx", idx);

	bind_texture(tex_text, 0);

	gl_begin(GL_QUADS);
	gl_color4f(1.0, 1.0, 1.0, alpha > 1.0 ? 1.0 : alpha);

	gl_texcoord2f(0, 1);
	gl_vertex3f(-1, -0.2, 0);

	gl_texcoord2f(1, 1);
	gl_vertex3f(1, -0.2, 0);

	gl_texcoord2f(1, 0);
	gl_vertex3f(1, 0.2, 0);

	gl_texcoord2f(0, 0);
	gl_vertex3f(-1, 0.2, 0);
	gl_end();

	bind_texture(0, 0);
	glDisable(GL_BLEND);

	gl_pop_matrix();
}


static void worm(float t, float z, float *tx, float *ty)
{
	float x, y;
	x = sin(t) + cos(t + z) + sin(t * 2.0 + z) / 2.0;
	y = cos(t) + sin(t + z) + cos(t * 2.0 + z) / 2.0;

	*tx = x * 0.5;
	*ty = y * 0.5;
}


void reshape(int x, int y)
{
	glViewport(0, 0, x, y);

	float aspect = (float)x / (float)y;
	float maxfov = 40.0;
	float vfov = aspect > 1.0 ? maxfov / aspect : maxfov;

	cam_fov(vfov);

	gl_matrix_mode(GL_PROJECTION);
	gl_load_identity();
	glu_perspective(vfov, aspect, 0.5, 500.0);

	view_xsz = x;
	view_ysz = y;
	view_aspect = aspect;

	ui_reshape(x, y);
}

void mouse_button(int bn, int press, int x, int y)
{
	if(show_opt) {
		ui_button(bn, press, x, y);
	}
}

void mouse_motion(int x, int y)
{
	if(show_opt) {
		ui_motion(x, y);
	}
}

static unsigned int get_shader_program(const char *vfile, const char *pfile)
{
	unsigned int prog, vs, ps;

	if(!(vs = get_vertex_shader(find_resource(vfile, 0, 0)))) {
		return 0;
	}
	if(!(ps = get_pixel_shader(find_resource(pfile, 0, 0)))) {
		return 0;
	}

	if(!(prog = create_program_link(vs, ps))) {
		return 0;
	}
	return prog;
}
