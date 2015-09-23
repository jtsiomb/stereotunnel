/*
GoatKit - a themable/animated widget toolkit for games
Copyright (C) 2014  John Tsiombikas <nuclear@member.fsf.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <math.h>
#include "slider.h"
#include "screen.h"

namespace goatkit {

struct SliderImpl {
	float value, prev_value;
	float range_min, range_max;
	float padding;
	float step;
	bool dragging;
	bool cont_change;
};

static float remap(float val, float inlow, float inhigh, float outlow, float outhigh);

Slider::Slider()
{
	slider = new SliderImpl;
	slider->value = slider->prev_value = 0.0f;
	slider->dragging = false;
	slider->cont_change = true;

	slider->range_min = 0.0;
	slider->range_max = 1.0;
	slider->step = 0.0;

	slider->padding = -1.0;
}

Slider::~Slider()
{
	delete slider;
}

const char *Slider::get_type_name() const
{
	return "slider";
}

void Slider::set_value(float val)
{
	slider->value = remap(val, slider->range_min, slider->range_max, 0, 1);
}

float Slider::get_value() const
{
	return remap(slider->value, 0, 1, slider->range_min, slider->range_max);
}

void Slider::set_value_norm(float val)
{
	slider->value = val < 0.0 ? 0.0 : (val > 1.0 ? 1.0 : val);
}

float Slider::get_value_norm() const
{
	return slider->value;
}

void Slider::set_padding(float pad)
{
	slider->padding = pad;
}

float Slider::get_padding() const
{
	if(slider->padding < 0.0) {
		BBox box = get_box();
		return (box.bmax.y - box.bmin.y) * 0.25;
	}
	return slider->padding;
}

void Slider::set_continuous_change(bool cont)
{
	slider->cont_change = cont;
}

bool Slider::get_continuous_change() const
{
	return slider->cont_change;
}

void Slider::set_range(float min, float max)
{
	slider->range_min = min;
	slider->range_max = max;
}

float Slider::get_range_min() const
{
	return slider->range_min;
}

float Slider::get_range_max() const
{
	return slider->range_max;
}

void Slider::set_step(float step)
{
	slider->step = step;
}

float Slider::get_step() const
{
	return slider->step;
}

void Slider::on_mouse_button(const ButtonEvent &ev)
{
	if(ev.button == 0) {
		Screen *scr = get_screen();

		slider->dragging = ev.press;
		if(ev.press) {
			if(scr) scr->grab_mouse(this);
		} else {
			if(scr) scr->grab_mouse(0);

			// on release, if the value has changed send the appropriate event
			if(slider->prev_value != slider->value) {
				Event ev;
				ev.type = EV_CHANGE;
				handle_event(ev);
				slider->prev_value = slider->value;
			}
		}
	}
}

#define ROUND(x)	floor((x) + 0.5)

void Slider::on_mouse_motion(const MotionEvent &ev)
{
	if(!slider->dragging) {
		return;
	}

	BBox box = get_box();

	float padding = get_padding();
	float start = box.bmin.x + padding;
	float end = box.bmax.x - padding;
	float new_val = (ev.pos.x - start) / (end - start);

	// if we have a non-zero step, snap to the nearest value
	if(slider->step > 0.0) {
		float range = slider->range_max - slider->range_min;
		float st = slider->step / range;

		new_val = ROUND(new_val / st) * st;
	}

	if(new_val < 0.0) new_val = 0.0;
	if(new_val > 1.0) new_val = 1.0;

	if(new_val != slider->value) {
		slider->value = new_val;
		if(slider->cont_change) {
			Event cev;
			cev.type = EV_CHANGE;
			handle_event(cev);
		}
	}
}

static float remap(float val, float inlow, float inhigh, float outlow, float outhigh)
{
	float t = (val - inlow) / (inhigh - inlow);
	if(t < 0.0) t = 0.0;
	if(t > 1.0) t = 1.0;
	return t * (outhigh - outlow) + outlow;
}

}	// namespace goatkit
