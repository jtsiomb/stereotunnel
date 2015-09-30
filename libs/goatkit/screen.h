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
#ifndef SCREEN_H_
#define SCREEN_H_

#include "vec.h"
#include "widget.h"

namespace goatkit {

class ScreenImpl;

class Screen {
private:
	ScreenImpl *scr;

public:
	Screen();
	~Screen();

	void set_position(float x, float y);
	void set_position(const Vec2 &pos);
	const Vec2 &get_position() const;
	void set_size(float x, float y);
	void set_size(const Vec2 &sz);
	const Vec2 get_size() const;
	const BBox &get_box() const;

	void add_widget(Widget *w);
	int get_widget_count() const;
	Widget *get_widget(int idx) const;
	Widget *get_widget(const char *name) const;

	void show();
	void hide();
	bool is_visible() const;
	float get_visibility() const;
	void set_visibility_transition(long msec);
	long get_visibility_transition() const;

	bool grab_mouse(Widget *w);

	void draw() const;

	// window system events used to generate widget events (see event.h)
	void sysev_keyboard(int key, bool press);
	// mouse position as reported by the window system. might fall outside
	void sysev_mouse_button(int bn, bool press, float x, float y);
	void sysev_mouse_motion(float x, float y);
};

}	// namespace goatkit

#endif /* SCREEN_H_ */
