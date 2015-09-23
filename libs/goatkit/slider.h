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
#ifndef SLIDER_H_
#define SLIDER_H_

#include "widget.h"

namespace goatkit {

struct SliderImpl;

class Slider : public Widget {
private:
	SliderImpl *slider;

public:
	Slider();
	virtual ~Slider();

	virtual const char *get_type_name() const;

	virtual void set_value(float val);
	virtual float get_value() const;

	virtual void set_value_norm(float val);
	virtual float get_value_norm() const;

	virtual void set_padding(float pad);
	virtual float get_padding() const;

	virtual void set_continuous_change(bool cont);
	virtual bool get_continuous_change() const;

	virtual void set_range(float min, float max);
	virtual float get_range_min() const;
	virtual float get_range_max() const;

	virtual void set_step(float step);
	virtual float get_step() const;

	virtual void on_mouse_button(const ButtonEvent &ev);
	virtual void on_mouse_motion(const MotionEvent &ev);
};

}	// namespace goatkit

#endif	// SLIDER_H_
