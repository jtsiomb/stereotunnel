#include <stdio.h>
#include "ui.h"
#include "goatkit/goatkit.h"
#include "opengl.h"
#include "sanegl.h"
#include "sdr.h"

static goatkit::Screen scr;
static int width, height;

extern unsigned int prog_color, prog_ui;

int ui_init(void)
{
	float ypos = 0;
	float vsep = 0.1;

	goatkit::Label *label = new goatkit::Label;
	label->set_position(0.5, ypos += vsep);
	label->set_size(0.1, 0.1);
	label->set_text("Stereoscopic rendering");
	scr.add_widget(label);

	/*
	goatkit::Button *button = new goatkit::Button;
	button->set_position(300, ypos += vsep);
	button->set_size(200, 40);
	button->set_text("a button!");
	button->set_callback(goatkit::EV_CLICK, callback);
	scr.add_widget(button);

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
}

void ui_draw(void)
{
	float aspect = (float)width / (float)height;

	bind_program(prog_ui);

	gl_matrix_mode(GL_PROJECTION);
	gl_push_matrix();
	gl_load_identity();
	gl_scalef(2.0 / aspect, 2.0, 1);
	gl_translatef(-1, -1, 0);
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
	gl_vertex3f(0, 1.0, 0);
	gl_vertex3f(1.0 * aspect, 1.0, 0);
	gl_vertex3f(1.0 * aspect, 0, 0);
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
	scr.sysev_mouse_button(bn, press != 0, (float)x / (float)width, (float)y / (float)height);
}

void ui_motion(int x, int y)
{
	scr.sysev_mouse_motion((float)x / (float)width, (float)y / (float)height);
}
