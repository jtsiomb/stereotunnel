/*
Stereoscopic tunnel for iOS.
Copyright (C) 2011  John Tsiombikas <nuclear@member.fsf.org>

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

#include <math.h>
#include "opengl.h"
#include "cam.h"
#include "sanegl.h"

#define DEG_TO_RAD(x)	(M_PI * (x) / 180.0)

typedef struct vec3 {
	float x, y, z;
} vec3_t;

/* viewing parameters */
#define DEF_THETA	0
#define DEF_PHI		0
#define DEF_DIST	0
#define DEF_X		0
#define DEF_Y		0
#define DEF_Z		0

static float cam_theta = DEF_THETA, cam_phi = DEF_PHI;
static float cam_dist = DEF_DIST;
static vec3_t cam_pos = {DEF_X, DEF_Y, DEF_Z};
static float cam_speed = 1.0;

/* projection parameters */
#define DEF_VFOV	45.0
#define DEF_ASPECT	1.3333333
#define DEF_NEAR	1.0
#define DEF_FAR		1000.0

static float vfov = DEF_VFOV;
static float aspect = DEF_ASPECT;
static float nearclip = DEF_NEAR, farclip = DEF_FAR;

/* stereo parameters */
#define DEF_EYE_SEP		0.1
#define DEF_FOCUS_DIST	1.0

static float eye_sep = DEF_EYE_SEP;
static float focus_dist = DEF_FOCUS_DIST;


static float rot_speed = 0.5;
static float zoom_speed = 0.1;

void cam_reset(void)
{
	cam_reset_view();
	cam_reset_proj();
	cam_reset_stereo();
}

void cam_reset_view(void)
{
	cam_theta = DEF_THETA;
	cam_phi = DEF_PHI;
	cam_dist = DEF_DIST;
	cam_pos.x = DEF_X;
	cam_pos.y = DEF_Y;
	cam_pos.z = DEF_Z;
}

void cam_reset_proj(void)
{
	vfov = DEF_VFOV;
	aspect = DEF_ASPECT;
	nearclip = DEF_NEAR;
	farclip = DEF_FAR;
}

void cam_reset_stereo(void)
{
	eye_sep = DEF_EYE_SEP;
	focus_dist = DEF_FOCUS_DIST;
}

void cam_pan(int dx, int dy)
{
	float dxf = dx * cam_speed;
	float dyf = dy * cam_speed;
	float angle = -DEG_TO_RAD(cam_theta);

	cam_pos.x += cos(angle) * dxf + sin(angle) * dyf;
	cam_pos.z += -sin(angle) * dxf + cos(angle) * dyf;
}

void cam_height(int dh)
{
	cam_pos.y += dh * cam_speed;
}

void cam_rotate(int dx, int dy)
{
	cam_theta += dx * rot_speed;
	cam_phi += dy * rot_speed;

	if(cam_phi < -90) cam_phi = -90;
	if(cam_phi > 90) cam_phi = 90;
}

void cam_zoom(int dz)
{
	cam_dist += dz * zoom_speed;
	if(cam_dist < 0.001) {
		cam_dist = 0.001;
	}
}

void cam_clip(float n, float f)
{
	nearclip = n;
	farclip = f;
}

void cam_fov(float f)
{
	vfov = f;
}

void cam_aspect(float a)
{
	aspect = a;
}

void cam_separation(float s)
{
	eye_sep = s;
}

void cam_focus_dist(float d)
{
	focus_dist = d;

	cam_separation(d / 30.0);
}

void cam_view_matrix(void)
{
	cam_stereo_view_matrix(CAM_CENTER);
}

void cam_stereo_view_matrix(int eye)
{
	static const float offs_sign[] = {0.0f, 0.5f, -0.5f};	/* center, left, right */
	float offs = eye_sep * offs_sign[eye];

	gl_translatef(offs, 0, 0);

	gl_translatef(0, 0, -cam_dist);
	gl_rotatef(cam_phi, 1, 0, 0);
	gl_rotatef(cam_theta, 0, 1, 0);
	gl_translatef(-cam_pos.x, -cam_pos.y, -cam_pos.z);
}

void cam_proj_matrix(void)
{
	cam_stereo_proj_matrix(CAM_CENTER);
}

void cam_stereo_proj_matrix(int eye)
{
	float vfov_rad = M_PI * vfov / 180.0;
	float top = nearclip * tan(vfov_rad * 0.5);
	float right = top * aspect;

	static const float offs_sign[] = {0.0f, 1.0, -1.0};	/* center, left, right */
	float frust_shift = offs_sign[eye] * (eye_sep * 0.5 * nearclip / focus_dist);

	gl_frustum(-right + frust_shift, right + frust_shift, -top, top, nearclip, farclip);
}
