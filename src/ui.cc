#include <stdio.h>
#include "ui.h"
#include "goatkit/goatkit.h"
#include "opengl.h"
#include "sanegl.h"
#include "sdr.h"

using namespace goatkit;

static void done_bn_handler(Widget *w, const Event &ev, void *cls)
{
	printf("done\n");
}

static goatkit::Screen scr;
static float aspect;
static int width, height;
static int virt_width, virt_height;

extern unsigned int prog_color, prog_ui;

int ui_init(void)
{
	float xpos = 100;
	float ypos = 50;
	float vsep = 50;

	/*goatkit::Label *label = new goatkit::Label;
	label->set_position(xpos, ypos);
	label->set_size(20, 20);
	label->set_text("Stereoscopic rendering");
	scr.add_widget(label);*/

	goatkit::Button *button = new goatkit::Button;
	button->set_position(xpos, ypos);
	button->set_size(80, 30);
	button->set_text("Done");
	button->set_callback(goatkit::EV_CLICK, done_bn_handler);
	scr.add_widget(button);

	/*
	goatkit::TextBox *text = new goatkit::TextBox;
	text->set_position(300, ypos += vsep);
	text->set_size(200, 30);
	text->set_text("foo");
	text->set_callback(goatkit::EV_CHANGE, callback);
	scr.add_widget(text);

	goatkit::CheckBox *cbox = new goatkit::CheckBox;
	cbox->set_position(300, ypos += vsep);
	cbox->set_size(200, 20);
	cbox->set_text("a checkbox!");
	cbox->set_callback(goatkit::EV_CHANGE, callback);
	scr.add_widget(cbox);

	goatkit::Slider *slider = new goatkit::Slider;
	slider->set_position(300, ypos += vsep);
	slider->set_size(200, 40);
	slider->set_callback(goatkit::EV_CHANGE, callback);
	slider->set_continuous_change(false);
	slider->set_range(0, 100.0);
	scr.add_widget(slider);

	goatkit::Slider *intslider = new goatkit::Slider;
	intslider->set_position(300, ypos += vsep);
	intslider->set_size(200, 40);
	intslider->set_callback(goatkit::EV_CHANGE, callback);
	intslider->set_continuous_change(false);
	intslider->set_range(0, 100.0);
	intslider->set_step(10);
	scr.add_widget(intslider);
	 */

	scr.show();

	// load the theme
	//goatkit::add_theme_path("themes/simple");

	if(!(goatkit::theme = goatkit::get_theme("istereo"))) {
		return -1;
	}

	return 0;
}

void ui_shutdown(void)
{
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

	set_uniform_float4(prog_ui, "ucolor", 0, 0, 0, 0.5);

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
