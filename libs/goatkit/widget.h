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
#ifndef GOATKIT_WIDGET_H_
#define GOATKIT_WIDGET_H_

#include "vec.h"
#include "event.h"

namespace goatkit {

struct BBox {
	Vec2 bmin, bmax;
};

class Screen;
class Widget;
struct WidgetImpl;

typedef void (*EventCallback)(Widget*, const Event &ev, void *cls);

class Widget {
protected:
	WidgetImpl *widget;

	virtual void set_screen(Screen *scr);
	Screen *get_screen() const;

public:
	Widget();
	virtual ~Widget();

	virtual const char *get_type_name() const;

	virtual void set_name(const char *name);
	virtual const char *get_name() const;

	virtual void set_text(const char *text);
	virtual const char *get_text() const;

	virtual void show();
	virtual void hide();
	virtual float get_visibility() const;
	virtual bool is_visible() const;

	virtual void activate();
	virtual void deactivate();
	virtual float get_active() const;
	virtual bool is_active() const;

	virtual void press();
	virtual void release();
	virtual float get_pressed() const;
	virtual bool is_pressed() const;

	virtual void mousein();
	virtual void mouseout();
	virtual float get_under_mouse() const;
	virtual bool is_under_mouse() const;

	// input focus, managed by the screen
	virtual bool can_focus() const;
	virtual void focusin();
	virtual void focusout();
	virtual float get_focus() const;
	virtual bool is_focused() const;

	virtual void set_position(float x, float y);
	virtual void set_position(const Vec2 &pos);
	virtual const Vec2 &get_position() const;

	virtual void set_size(float x, float y);
	virtual void set_size(const Vec2 &size);
	virtual const Vec2 get_size() const;

	virtual const BBox &get_box() const;

	virtual bool hit_test(const Vec2 &pt) const;

	virtual void draw() const;

	// low level events
	virtual void on_mouse_button(const ButtonEvent &ev);
	virtual void on_mouse_motion(const MotionEvent &ev);
	virtual void on_mouse_focus(const FocusEvent &ev);
	virtual void on_key(const KeyEvent &ev);

	// high level events
	virtual void on_click();
	virtual void on_double_click();
	virtual void on_change();
	//virtual void on_drag_move(int bn, const Vec2 &pt);
	//virtual void on_drag_release(int bn, const Vec2 &pt);

	// event dispatcher
	virtual void handle_event(const Event &ev);

	// external callback setting
	virtual void set_callback(EventType evtype, EventCallback func, void *cls = 0);

	friend class Screen;
};

}

#endif	// GOATKIT_WIDGET_H_
