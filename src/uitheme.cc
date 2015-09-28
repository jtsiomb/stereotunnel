#include <map>
#include <string>
#include "goatkit/goatkit.h"
#include "opengl.h"
#include "sanegl.h"
#include "drawtext.h"
#include "sdr.h"

#define VIS_THRES	0.005

using namespace goatkit;

extern int view_xsz, view_ysz;
extern float view_aspect;
extern unsigned int prog_ui, prog_font, prog_color;
extern struct dtx_font *font;

static void draw_label(const Widget *w);
static void draw_button(const Widget *w);
static void draw_rect(const Vec2 &pos, const Vec2 &sz, float r, float g, float b, float a = 1.0f);
static void draw_text(float x, float y, const char *text);

static struct {
	const char *name;
	WidgetDrawFunc func;
} widget_funcs[] = {
	{ "label", draw_label },
	{ "button", draw_button },
	{0, 0}
};

static std::map<std::string, WidgetDrawFunc> funcmap;


extern "C" __attribute__ ((used))
WidgetDrawFunc get_widget_func(const char *name)
{
	static bool initialized;

	if(!initialized) {
		for(int i=0; widget_funcs[i].func; i++) {
			funcmap[widget_funcs[i].name] = widget_funcs[i].func;
		}
		initialized = true;
	}
	return funcmap[name];
}

// register ourselves as a built-in theme
GOATKIT_BUILTIN_THEME("istereo", get_widget_func);

static void begin_drawing(const Widget *w)
{
	Vec2 pos = w->get_position();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gl_matrix_mode(GL_MODELVIEW);
	gl_push_matrix();
	gl_load_identity();
	gl_translatef(pos.x, pos.y, 0);
}

static void end_drawing(const Widget *w)
{
	gl_matrix_mode(GL_MODELVIEW);
	gl_pop_matrix();
}

static void draw_label(const Widget *w)
{
	Vec2 pos = w->get_position();
	float vis = w->get_visibility();
	if(vis < VIS_THRES) return;

	begin_drawing(w);
	draw_text(pos.x, pos.y, w->get_text());
	end_drawing(w);
}

static void draw_button(const Widget *w)
{
	Vec2 pos = w->get_position();
	Vec2 sz = w->get_size();
	float vis = w->get_visibility();
	if(vis < VIS_THRES) return;

	begin_drawing(w);

	draw_rect(pos, sz, 1.0, 0.3, 0.2);
	draw_text(pos.x, pos.y, w->get_text());

	end_drawing(w);
}

static void draw_rect(const Vec2 &pos, const Vec2 &sz, float r, float g, float b, float a)
{
	float aspect = sz.x / sz.y;

	bind_program(prog_color);
	gl_apply_xform(prog_color);

	gl_begin(GL_QUADS);
	gl_color4f(r, g, b, a);
	gl_texcoord2f(0, 1);
	gl_vertex2f(pos.x, pos.y);
	gl_texcoord2f(aspect, 1);
	gl_vertex2f(pos.x + sz.x, pos.y);
	gl_texcoord2f(aspect, 0);
	gl_vertex2f(pos.x + sz.x, pos.y + sz.y);
	gl_texcoord2f(0, 0);
	gl_vertex2f(pos.x, pos.y + sz.y);
	gl_end();
}

static void draw_text(float x, float y, const char *text)
{
	struct dtx_glyphmap *gmap = dtx_get_font_glyphmap_idx(font, 0);
	dtx_use_font(font, dtx_get_glyphmap_ptsize(gmap));

	float virt_xsz = 420.0 * view_aspect;
	float virt_ysz = 420.0;

	gl_matrix_mode(GL_PROJECTION);
	gl_push_matrix();
	gl_load_identity();
	gl_ortho(0, virt_xsz, 0, virt_ysz, -1, 1);

	gl_matrix_mode(GL_MODELVIEW);
	gl_push_matrix();
	gl_load_identity();
	gl_translatef(x * virt_xsz, y * virt_ysz, 0);

	bind_program(prog_font);
	set_uniform_float4(prog_font, "ucolor", 1.0, 1.0, 1.0, 1.0);
	gl_apply_xform(prog_ui);

	dtx_string(text);

	gl_matrix_mode(GL_PROJECTION);
	gl_pop_matrix();
	gl_matrix_mode(GL_MODELVIEW);
	gl_pop_matrix();
}
