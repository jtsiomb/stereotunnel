#include <map>
#include <string>
#include "goatkit/goatkit.h"
#include "opengl.h"
#include "sanegl.h"
#include "drawtext.h"
#include "sdr.h"

#define BEVEL		1.0
#define VIS_THRES	0.005

using namespace goatkit;

struct Color {
	float r, g, b, a;

	Color() : r(1), g(1), b(1), a(1) {}
	Color(float r, float g, float b, float a = 1.0f)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
};

enum {
	TEXT_COLOR,
	TOP_COLOR,
	BOTTOM_COLOR,
	BEVEL_LIT_COLOR,
	BEVEL_SHAD_COLOR,
	CURSOR_COLOR,
	SELECTION_COLOR,
	CHECK_COLOR
};

static Color colors[] = {
	Color(0.0, 0.0, 0.0, 1.0),		/* text color */
	Color(0.75, 0.75, 0.75, 1.0),	/* top color */
	Color(0.56, 0.56, 0.56, 1.0),	/* bot color */
	Color(0.8, 0.8, 0.8, 1.0),		/* lit bevel */
	Color(0.35, 0.35, 0.35, 1.0),		/* shadowed bevel */
	Color(0.8, 0.25, 0.18, 1.0),	/* cursor color */
	Color(0.68, 0.85, 1.3, 1.0),	/* selection color */
	Color(0.75, 0.1, 0.095, 1.0)	/* check color */
};


#define JLEFT	-1.0f
#define JCENTER	0.0f
#define JRIGHT	1.0f

enum { FRAME_INSET, FRAME_OUTSET };

extern int view_xsz, view_ysz;
extern float view_aspect;
extern unsigned int prog_ui, prog_font, prog_color;
extern struct dtx_font *font;

static void draw_label(const Widget *w);
static void draw_button(const Widget *w);
static void draw_rect(const Vec2 &pos, const Vec2 &sz, const Color &color);
static void draw_rect(const Vec2 &pos, const Vec2 &sz, const Color &ctop, const Color &cbot);
static void draw_text(float justify, float x, float y, const char *text);
static void draw_frame(const Vec2 &pos, const Vec2 &sz, float inset);
static Color lerp(const Color &a, const Color &b, float t);

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
	//Vec2 pos = w->get_position();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gl_matrix_mode(GL_MODELVIEW);
	gl_push_matrix();
	gl_load_identity();
	//gl_translatef(pos.x, pos.y, 0);
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
	draw_text(JLEFT, pos.x, pos.y, w->get_text());
	end_drawing(w);
}

static void draw_button(const Widget *w)
{
	Vec2 pos = w->get_position();
	Vec2 sz = w->get_size();
	float vis = w->get_visibility();
	float press = w->get_pressed();
	if(vis < VIS_THRES) return;

	Color tcol = lerp(colors[TOP_COLOR], colors[BOTTOM_COLOR], press);
	Color bcol = lerp(colors[BOTTOM_COLOR], colors[TOP_COLOR], press);

	begin_drawing(w);

	draw_frame(pos, sz, press);
	draw_rect(pos, sz, tcol, bcol);
	//draw_rect(Vec2(pos.x + sz.x / 2.0 - 2.0, pos.y), Vec2(4.0, sz.y),
	//		Color(0.4, 0.5, 1.0));
	draw_text(JCENTER, pos.x + sz.x / 2.0, pos.y, w->get_text());

	end_drawing(w);
}

static void draw_rect(const Vec2 &pos, const Vec2 &sz, const Color &color)
{
	draw_rect(pos, sz, color, color);
}

static void draw_rect(const Vec2 &pos, const Vec2 &sz, const Color &ctop, const Color &cbot)
{
	float aspect = sz.x / sz.y;

	bind_program(prog_color);

	gl_begin(GL_QUADS);
	gl_color4f(cbot.r, cbot.g, cbot.b, cbot.a);
	gl_texcoord2f(0, 1);
	gl_vertex3f(pos.x, pos.y, 0);
	gl_texcoord2f(aspect, 1);
	gl_vertex3f(pos.x + sz.x, pos.y, 0);
	gl_color4f(ctop.r, ctop.g, ctop.b, ctop.a);
	gl_texcoord2f(aspect, 0);
	gl_vertex3f(pos.x + sz.x, pos.y + sz.y, 0);
	gl_texcoord2f(0, 0);
	gl_vertex3f(pos.x, pos.y + sz.y, 0);
	gl_end();
}

static void draw_text(float justify, float x, float y, const char *text)
{
	struct dtx_glyphmap *gmap = dtx_get_font_glyphmap_idx(font, 0);
	dtx_use_font(font, dtx_get_glyphmap_ptsize(gmap));

	float twidth = dtx_string_width(text);
	float thalf = twidth / 2.0;

	gl_matrix_mode(GL_MODELVIEW);
	gl_push_matrix();
	gl_load_identity();
	gl_translatef(x - thalf - justify * thalf, y + 8, 0);

	bind_program(prog_font);
	set_uniform_float4(prog_font, "ucolor", 1.0, 1.0, 1.0, 1.0);
	gl_apply_xform(prog_font);

	dtx_string(text);

	gl_matrix_mode(GL_MODELVIEW);
	gl_pop_matrix();
}

static void draw_frame(const Vec2 &pos, const Vec2 &sz, float inset)
{
	float x = pos.x - BEVEL;
	float y = pos.y - BEVEL;
	float w = sz.x + BEVEL * 2.0;
	float h = sz.y + BEVEL * 2.0;
	float b = BEVEL;

	Color tcol = lerp(colors[BEVEL_LIT_COLOR], colors[BEVEL_SHAD_COLOR], inset);
	Color bcol = lerp(colors[BEVEL_SHAD_COLOR], colors[BEVEL_LIT_COLOR], inset);

	bind_program(prog_color);

	gl_begin(GL_QUADS);
	gl_color4f(tcol.r, tcol.g, tcol.b, tcol.a);
	gl_vertex2f(x + b, y + h - b);
	gl_vertex2f(x + w - b, y + h - b);
	gl_vertex2f(x + w, y + h);
	gl_vertex2f(x, y + h);

	gl_vertex2f(x + b, y + b);
	gl_vertex2f(x, y);
	gl_vertex2f(x, y + h);
	gl_vertex2f(x + b, y + h - b);

	gl_color4f(bcol.r, bcol.g, bcol.b, bcol.a);
	gl_vertex2f(x, y);
	gl_vertex2f(x + b, y + b);
	gl_vertex2f(x + w - b, y + b);
	gl_vertex2f(x + w, y);

	gl_vertex2f(x + w - b, y + b);
	gl_vertex2f(x + w, y);
	gl_vertex2f(x + w, y + h);
	gl_vertex2f(x + w - b, y + h - b);
	gl_end();
}

static Color lerp(const Color &a, const Color &b, float t)
{
	Color res;
	res.r = a.r + (b.r - a.r) * t;
	res.g = a.g + (b.g - a.g) * t;
	res.b = a.b + (b.b - a.b) * t;
	return res;
}
