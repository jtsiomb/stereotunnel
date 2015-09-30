#include <map>
#include <string>
#include "goatkit/goatkit.h"
#include "opengl.h"
#include "sanegl.h"
#include "drawtext.h"
#include "sdr.h"

#define BEVEL		1.5
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
	TOP_ON_COLOR,
	BOTTOM_ON_COLOR,
	TOP_OFF_COLOR,
	BOTTOM_OFF_COLOR,
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
	Color(0.4, 0.5, 0.8, 1.0),		/* top on color */
	Color(0.15, 0.2, 0.4, 1.0),		/* bottom on color */
	Color(0.8, 0.5, 0.4, 1.0),		/* top off color */
	Color(0.4, 0.2, 0.15, 1.0),		/* bottom off color */
	Color(0.8, 0.8, 0.8, 1.0),		/* lit bevel */
	Color(0.35, 0.35, 0.35, 1.0),	/* shadowed bevel */
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
static void draw_checkbox(const Widget *w);
static void draw_slider(const Widget *w);
static void draw_rect(const Widget *w, const Vec2 &pos, const Vec2 &sz, const Color &color);
static void draw_rect(const Widget *w, const Vec2 &pos, const Vec2 &sz, const Color &ctop, const Color &cbot);
static void draw_shadow_text(const Widget *w, float justify, float x, float y, const Color &col, const char *text, float press = 0.0);
static void draw_shadow_text(const Widget *w, float justify, float x, float y, const char *text, float press = 0.0);
static void draw_text(const Widget *w, float justify, float x, float y, const char *text);
static void draw_frame(const Widget *w, const Vec2 &pos, const Vec2 &sz, float inset);
static float lerp(float a, float b, float t);
static Color lerp(const Color &a, const Color &b, float t);

static struct {
	const char *name;
	WidgetDrawFunc func;
} widget_funcs[] = {
	{ "label", draw_label },
	{ "button", draw_button },
	{ "checkbox", draw_checkbox },
	{ "slider", draw_slider },
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
	Vec2 sz = w->get_size();
	float vis = w->get_visibility();
	if(vis < VIS_THRES) return;

	begin_drawing(w);
	draw_shadow_text(w, JLEFT, pos.x, pos.y + sz.y / 2.0, w->get_text());
	end_drawing(w);
}

static void draw_button(const Widget *w)
{
	float vis = w->get_visibility();
	if(vis < VIS_THRES) return;

	Vec2 pos = w->get_position();
	Vec2 sz = w->get_size();
	float press = w->get_pressed();

	Color tcol = lerp(colors[TOP_COLOR], colors[BOTTOM_COLOR], press);
	Color bcol = lerp(colors[BOTTOM_COLOR], colors[TOP_COLOR], press);

	begin_drawing(w);

	draw_frame(w, pos, sz, press);
	draw_rect(w, pos, sz, tcol, bcol);
	//draw_rect(w, Vec2(pos.x + sz.x / 2.0 - 2.0, pos.y), Vec2(4.0, sz.y),
	//		Color(0.4, 0.5, 1.0));
	draw_shadow_text(w, JCENTER, pos.x + sz.x / 2.0, pos.y + sz.y / 2.0, w->get_text(), press);

	end_drawing(w);
}

static void draw_checkbox(const Widget *w)
{
	float vis = w->get_visibility();
	if(vis < VIS_THRES) return;

	Vec2 pos = w->get_position();
	Vec2 sz = w->get_size();
	Vec2 boxsz = Vec2(sz.y * 3.0, sz.y);
	Vec2 boxpos = Vec2(pos.x + sz.x - boxsz.x, pos.y);

	float checked = ((CheckBox*)w)->get_checked();

	float vcenter = boxpos.y + boxsz.y / 2.0;

	begin_drawing(w);

	draw_frame(w, boxpos, boxsz, 1.0);
	Color tcol = lerp(colors[TOP_OFF_COLOR], colors[TOP_ON_COLOR], checked);
	Color bcol = lerp(colors[BOTTOM_OFF_COLOR], colors[BOTTOM_ON_COLOR], checked);
	draw_rect(w, boxpos, boxsz, bcol, tcol);

	draw_shadow_text(w, JLEFT, boxpos.x + 2, vcenter, "ON", 0.0);
	draw_shadow_text(w, JRIGHT, boxpos.x + boxsz.x - 3, vcenter, "OFF", 0.0);

	Vec2 knobpos = Vec2(0, boxpos.y + 1.0);
	Vec2 knobsz = Vec2(boxsz.x / 2.0, boxsz.y - 2.0);

	knobpos.x = lerp(boxpos.x + 1.0, boxpos.x + boxsz.x / 2.0, checked);

	draw_frame(w, knobpos, knobsz, 0.0);
	draw_rect(w, knobpos, knobsz, colors[TOP_COLOR], colors[BOTTOM_COLOR]);

	draw_shadow_text(w, JRIGHT, boxpos.x - 2.0, vcenter, w->get_text(), 0.0);

	end_drawing(w);
}

static void draw_slider(const Widget *w)
{
	float vis = w->get_visibility();
	if(vis < VIS_THRES) return;

	float nval = ((Slider*)w)->get_value_norm();

	Vec2 pos = w->get_position();
	Vec2 sz = w->get_size();
	Vec2 tsz = Vec2(sz.y * 0.5, sz.y);
	Vec2 tpos = Vec2(pos.x + sz.x * nval - tsz.x / 2.0, pos.y);

	// modify original pos/size to correspond to the trough geometry
	sz.y /= 5.0;
	pos.y += sz.y * 2.0;

	begin_drawing(w);

	// draw trough
	draw_frame(w, pos, sz, 1.0);
	draw_rect(w, pos, sz, colors[BOTTOM_COLOR], colors[TOP_COLOR]);

	// draw thumb
	draw_frame(w, tpos, tsz, 0.0);
	draw_rect(w, tpos, tsz, colors[TOP_COLOR], colors[BOTTOM_COLOR]);

	end_drawing(w);
}

static void draw_rect(const Widget *w, const Vec2 &pos, const Vec2 &sz, const Color &color)
{
	draw_rect(w, pos, sz, color, color);
}

static void draw_rect(const Widget *w, const Vec2 &pos, const Vec2 &sz, const Color &ctop, const Color &cbot)
{
	float vis = w ? w->get_visibility() : 1.0;
	float act = w ? w->get_active() : 1.0;
	float aspect = sz.x / sz.y;

	float alpha = vis * (act * 0.5 + 0.5);

	bind_program(prog_color);

	gl_begin(GL_QUADS);
	gl_color4f(cbot.r, cbot.g, cbot.b, cbot.a * alpha);
	gl_texcoord2f(0, 1);
	gl_vertex3f(pos.x, pos.y, 0);
	gl_texcoord2f(aspect, 1);
	gl_vertex3f(pos.x + sz.x, pos.y, 0);
	gl_color4f(ctop.r, ctop.g, ctop.b, ctop.a * alpha);
	gl_texcoord2f(aspect, 0);
	gl_vertex3f(pos.x + sz.x, pos.y + sz.y, 0);
	gl_texcoord2f(0, 0);
	gl_vertex3f(pos.x, pos.y + sz.y, 0);
	gl_end();
}

static void draw_shadow_text(const Widget *w, float justify, float x, float y, const Color &col, const char *text, float press)
{
	static const Color cshad = Color(0.1, 0.1, 0.1, 1.0);

	float xoffs = 1.0 - press;
	float yoffs = 1.0 - press;

	Color orig = colors[TEXT_COLOR];
	colors[TEXT_COLOR] = cshad;
	colors[TEXT_COLOR].a = (1.0 - press);
	draw_text(w, justify, x + xoffs, y - yoffs, text);
	colors[TEXT_COLOR] = col;
	draw_text(w, justify, x, y, text);
	colors[TEXT_COLOR] = orig;
}

static void draw_shadow_text(const Widget *w, float justify, float x, float y, const char *text, float press)
{
	draw_shadow_text(w, justify, x, y, Color(1.0, 1.0, 1.0, 1.0), text, press);
}

static void draw_text(const Widget *w, float justify, float x, float y, const char *text)
{
	struct dtx_glyphmap *gmap = dtx_get_font_glyphmap_idx(font, 0);
	dtx_use_font(font, dtx_get_glyphmap_ptsize(gmap));

	float vis = w ? w->get_visibility() : 1.0;
	float act = w ? w->get_active() : 1.0;

	float twidth = dtx_string_width(text);
	float thalf = twidth / 2.0;
	float theight = dtx_line_height();

	gl_matrix_mode(GL_MODELVIEW);
	gl_push_matrix();
	gl_load_identity();
	gl_translatef(x - thalf - justify * thalf, y - theight * 0.25, 0);

	bind_program(prog_font);
	const Color &tcol = colors[TEXT_COLOR];
	set_uniform_float4(prog_font, "ucolor", tcol.r, tcol.g, tcol.b, tcol.a * vis * (act * 0.5 + 0.5));
	gl_apply_xform(prog_font);

	dtx_string(text);

	gl_matrix_mode(GL_MODELVIEW);
	gl_pop_matrix();
}

static void draw_frame(const Widget *widget, const Vec2 &pos, const Vec2 &sz, float inset)
{
	float vis = widget ? widget->get_visibility() : 1.0;
	float act = widget ? widget->get_active() : 1.0;
	float offs = 300.0 * (1.0 - vis);

	float alpha = vis * (act * 0.5 + 0.5);

	float x = pos.x - BEVEL;
	float y = pos.y - BEVEL;
	float w = sz.x + BEVEL * 2.0;
	float h = sz.y + BEVEL * 2.0;
	float b = BEVEL;

	Color tcol = lerp(colors[BEVEL_LIT_COLOR], colors[BEVEL_SHAD_COLOR], inset);
	Color bcol = lerp(colors[BEVEL_SHAD_COLOR], colors[BEVEL_LIT_COLOR], inset);

	bind_program(prog_color);

	gl_begin(GL_QUADS);
	gl_color4f(tcol.r, tcol.g, tcol.b, tcol.a * alpha);
	gl_vertex2f(x + b + offs, y + h - b);
	gl_vertex2f(x + w - b + offs, y + h - b);
	gl_vertex2f(x + w + offs, y + h);
	gl_vertex2f(x + offs, y + h);

	gl_vertex2f(x + b, y + b + offs);
	gl_vertex2f(x, y + offs);
	gl_vertex2f(x, y + h + offs);
	gl_vertex2f(x + b, y + h - b + offs);

	gl_color4f(bcol.r, bcol.g, bcol.b, bcol.a * alpha);
	gl_vertex2f(x - offs, y);
	gl_vertex2f(x + b - offs, y + b);
	gl_vertex2f(x + w - b - offs, y + b);
	gl_vertex2f(x + w - offs, y);

	gl_vertex2f(x + w - b, y + b - offs);
	gl_vertex2f(x + w, y - offs);
	gl_vertex2f(x + w, y + h - offs);
	gl_vertex2f(x + w - b, y + h - b - offs);
	gl_end();
}

static float lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

static Color lerp(const Color &a, const Color &b, float t)
{
	Color res;
	res.r = a.r + (b.r - a.r) * t;
	res.g = a.g + (b.g - a.g) * t;
	res.b = a.b + (b.b - a.b) * t;
	return res;
}
