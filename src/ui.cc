#include <stdio.h>
#include "ui.h"
#include "goatkit/goatkit.h"
#include "opengl.h"
#include "sanegl.h"
#include "sdr.h"

using namespace goatkit;

extern "C" void ad_banner_show(void);
extern "C" void ad_banner_hide(void);
extern "C" void use_retina_res(int enable);
extern "C" int using_retina_res(void);
extern "C" int have_retina(void);

static void done_bn_handler(Widget *w, const Event &ev, void *cls);
static void stereo_cbox_handler(Widget *w, const Event &ev, void *cls);
static void bump_cbox_handler(Widget *w, const Event &ev, void *cls);
static void split_slider_handler(Widget *w, const Event &ev, void *cls);
#ifdef IPHONE
static void retina_cbox_handler(Widget *w, const Event &ev, void *cls);
#else
static void qual_slider_handler(Widget *w, const Event &ev, void *cls);
#endif

extern int stereo;
extern int show_opt;
extern int use_bump;
extern float split;
extern float draw_quality;

static Screen scr;
static float aspect;
static int width, height;
static int virt_width, virt_height;
static Slider *slider_split;
static Label *label_split;

extern unsigned int prog_color, prog_ui;

int ui_init(void)
{
	float ypos = 280;
	float vsep = 75;
	float vsz = 36;

	CheckBox *cbox;

#ifdef IPHONE
	if(have_retina()) {
		cbox = new CheckBox;
		cbox->set_position(300, ypos + vsep);
		cbox->set_size(300, vsz);
		cbox->set_text("Retina quality rendering");
		if(using_retina_res()) {
			cbox->check();
		} else {
			cbox->uncheck();
		}
		cbox->set_callback(EV_CHANGE, retina_cbox_handler);
		scr.add_widget(cbox);
	}
#else
	/*
	label_split = new Label;
	label_split->set_position(100, ypos + vsep);
	label_split->set_size(20, vsz);
	label_split->set_text("Rendering quality");
	scr.add_widget(label_split);

	Slider *slider = new Slider;
	slider->set_position(300, ypos + vsep);
	slider->set_size(300, vsz);
	slider->set_range(0.5, 1);
	slider->set_step(0.25);
	slider->set_value(1.0);
	slider->set_continuous_change(true);
	slider->set_callback(EV_CHANGE, qual_slider_handler);
	scr.add_widget(slider);
	*/
#endif

	cbox = new CheckBox;
	cbox->set_position(300, ypos);
	cbox->set_size(300, vsz);
	cbox->set_text("Stereoscopic rendering");
	cbox->set_callback(EV_CHANGE, stereo_cbox_handler);
	scr.add_widget(cbox);
	ypos -= vsep;

	label_split = new Label;
	label_split->set_position(70, ypos);
	label_split->set_size(20, vsz);
	label_split->set_text("Stereo split");
	if(!stereo) label_split->deactivate();
	scr.add_widget(label_split);

	slider_split = new Slider;
	slider_split->set_position(200, ypos);
	slider_split->set_size(400, vsz);
	slider_split->set_continuous_change(true);
	slider_split->set_range(0, 1.0);
	slider_split->set_value(split);
	slider_split->set_callback(EV_CHANGE, split_slider_handler);
	if(!stereo) slider_split->deactivate();
	scr.add_widget(slider_split);
	ypos -= vsep;

	cbox = new CheckBox;
	cbox->set_position(300, ypos);
	cbox->set_size(300, vsz);
	cbox->set_text("Bump mapping");
	cbox->set_callback(EV_CHANGE, bump_cbox_handler);
	scr.add_widget(cbox);
	ypos -= vsep;

	ypos -= vsep * 0.25;
	Button *button = new Button;
	button->set_position(450, ypos);
	button->set_size(150, vsz);
	button->set_text("Ok");
	button->set_callback(EV_CLICK, done_bn_handler);
	scr.add_widget(button);
	ypos -= vsep;

	scr.set_visibility_transition(400);

	if(!(theme = get_theme("istereo"))) {
		return -1;
	}

	return 0;
}

void ui_shutdown(void)
{
}

void ui_show()
{
	scr.show();
	ad_banner_show();
}

void ui_hide()
{
	scr.hide();
	ad_banner_hide();
}

int ui_visible()
{
	return scr.get_visibility() > 0.01;
}

void ui_reshape(int x, int y)
{
	width = x;
	height = y;
	aspect = (float)width / (float)height;

	virt_width = 500.0 * aspect;
	virt_height = 500.0;
}

void ui_draw(void)
{
	bind_program(prog_ui);

	gl_matrix_mode(GL_PROJECTION);
	gl_push_matrix();
	gl_load_identity();
	gl_ortho(0, virt_width, 0, virt_height, -1, 1);
	gl_matrix_mode(GL_MODELVIEW);
	gl_push_matrix();
	gl_load_identity();

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	set_uniform_float4(prog_ui, "ucolor", 0, 0, 0, 0.5 * scr.get_visibility());

	gl_begin(GL_QUADS);
	//gl_color4f(0, 0, 0, 0.5);
	gl_vertex3f(0, 0, 0);
	gl_vertex3f(0, virt_height, 0);
	gl_vertex3f(virt_width, virt_height, 0);
	gl_vertex3f(virt_width, 0, 0);
	gl_end();

	scr.draw();

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);


	gl_matrix_mode(GL_PROJECTION);
	gl_pop_matrix();
	gl_matrix_mode(GL_MODELVIEW);
	gl_pop_matrix();
}

void ui_button(int bn, int press, int x, int y)
{
	float normx = virt_width * (float)x / (float)width;
	float normy = virt_height - virt_height * (float)y / (float)height;

	scr.sysev_mouse_button(bn, press != 0, normx, normy);
}

void ui_motion(int x, int y)
{
	float normx = virt_width * (float)x / (float)width;
	float normy = virt_height - virt_height * (float)y / (float)height;

	scr.sysev_mouse_motion(normx, normy);
}


static void done_bn_handler(Widget *w, const Event &ev, void *cls)
{
	show_opt = 0;
	ui_hide();
}

static void stereo_cbox_handler(Widget *w, const Event &ev, void *cls)
{
	stereo = ((CheckBox*)w)->is_checked() ? 1 : 0;

	if(stereo) {
		slider_split->activate();
		label_split->activate();
	} else {
		slider_split->deactivate();
		label_split->deactivate();
	}
}

static void bump_cbox_handler(Widget *w, const Event &ev, void *cls)
{
	use_bump = ((CheckBox*)w)->is_checked() ? 1 : 0;
}

static void split_slider_handler(Widget *w, const Event &ev, void *cls)
{
	split = ((Slider*)w)->get_value();
}

#ifdef IPHONE
static void retina_cbox_handler(Widget *w, const Event &ev, void *cls)
{
	use_retina_res(((CheckBox*)w)->is_checked() ? 1 : 0);
}
#else
static void qual_slider_handler(Widget *w, const Event &ev, void *cls)
{
	draw_quality = ((Slider*)w)->get_value();
}
#endif

