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
#include <string.h>
#include <math.h>
#include <string>
#include <sstream>
#include "widget.h"
#include "boolanm.h"
#include "theme.h"
#include "screen.h"

namespace goatkit {

struct WidgetImpl {
	Screen *scr;
	std::string name, text;
	BBox box;

	BoolAnim visible, active, press, hover, focus;

	struct {
		EventCallback func;
		void *cls;
	} cb[NUM_EVENTS];
};


Widget::Widget()
{
	static int widget_count;

	widget = new WidgetImpl;
	widget->scr = 0;

	std::stringstream sstr;
	sstr << get_type_name() << widget_count++;
	widget->name = sstr.str();

	widget->box.bmin = Vec2(0, 0);
	widget->box.bmax = Vec2(1, 1);

	widget->visible.set(true);
	widget->active.set(true);

	widget->hover.set_transition_duration(250);
	widget->press.set_transition_duration(50);

	memset(widget->cb, 0, sizeof widget->cb);
}

Widget::~Widget()
{
	delete widget;
}

void Widget::set_screen(Screen *scr)
{
	widget->scr = scr;
}

Screen *Widget::get_screen() const
{
	return widget->scr;
}

const char *Widget::get_type_name() const
{
	return "widget";
}

void Widget::set_name(const char *name)
{
	widget->name = std::string(name);
}

const char *Widget::get_name() const
{
	return widget->name.c_str();
}

void Widget::set_text(const char *text)
{
	widget->text = std::string(text);
}

const char *Widget::get_text() const
{
	return widget->text.c_str();
}

void Widget::show()
{
	widget->visible.change(true);
}

void Widget::hide()
{
	widget->visible.change(false);
}

float Widget::get_visibility() const
{
	return widget->visible.get_value();
}

bool Widget::is_visible() const
{
	return widget->visible.get_state();
}

void Widget::activate()
{
	widget->active.change(true);
}

void Widget::deactivate()
{
	widget->active.change(false);
}

float Widget::get_active() const
{
	return widget->active.get_value();
}

bool Widget::is_active() const
{
	return widget->active.get_state();
}

void Widget::press()
{
	widget->press.change(true);
}

void Widget::release()
{
	widget->press.change(false);
}

float Widget::get_pressed() const
{
	return widget->press.get_value();
}

bool Widget::is_pressed() const
{
	return widget->press.get_state();
}

void Widget::mousein()
{
	widget->hover.change(true);
}

void Widget::mouseout()
{
	widget->hover.change(false);
	if(widget->press) {
		widget->press.change(false);
	}
}

float Widget::get_under_mouse() const
{
	return widget->hover.get_value();
}

bool Widget::is_under_mouse() const
{
	return widget->hover.get_state();
}

bool Widget::can_focus() const
{
	return false;
}

void Widget::focusin()
{
	widget->focus.change(true);
}

void Widget::focusout()
{
	widget->focus.change(false);
}

float Widget::get_focus() const
{
	return widget->focus.get_value();
}

bool Widget::is_focused() const
{
	return widget->focus.get_state();
}

void Widget::set_position(float x, float y)
{
	set_position(Vec2(x, y));
}

void Widget::set_position(const Vec2 &pos)
{
	Vec2 sz = get_size();

	widget->box.bmin = pos;
	widget->box.bmax.x = pos.x + sz.x;
	widget->box.bmax.y = pos.y + sz.y;
}

const Vec2 &Widget::get_position() const
{
	return widget->box.bmin;
}

void Widget::set_size(float x, float y)
{
	set_size(Vec2(x, y));
}

void Widget::set_size(const Vec2 &sz)
{
	widget->box.bmax.x = widget->box.bmin.x + sz.x;
	widget->box.bmax.y = widget->box.bmin.y + sz.y;
}

const Vec2 Widget::get_size() const
{
	return Vec2(widget->box.bmax.x - widget->box.bmin.x,
			widget->box.bmax.y - widget->box.bmin.y);
}


const BBox &Widget::get_box() const
{
	return widget->box;
}

bool Widget::hit_test(const Vec2 &pt) const
{
	return pt.x >= widget->box.bmin.x && pt.x < widget->box.bmax.x &&
		pt.y >= widget->box.bmin.y && pt.y < widget->box.bmax.y;
}

void Widget::draw() const
{
	WidgetDrawFunc draw_func = default_draw_func;

	if(theme) {
		draw_func = theme->get_draw_func(get_type_name());
	}

	draw_func(this);
}

// dummy event handlers
void Widget::on_mouse_button(const ButtonEvent &ev)
{
}

void Widget::on_mouse_motion(const MotionEvent &ev)
{
}

void Widget::on_mouse_focus(const FocusEvent &ev)
{
}

void Widget::on_key(const KeyEvent &ev)
{
}

void Widget::on_click()
{
}

void Widget::on_double_click()
{
}

void Widget::on_change()
{
}


#define CALL_CB(w, ev) \
	do { \
		if((w)->widget->cb[ev.type].func) { \
			(w)->widget->cb[ev.type].func((w), ev, (w)->widget->cb[ev.type].cls); \
		} \
	} while(0)

#define CALL_CB_TYPE(w, t) \
	do { \
		Event ev; \
		ev.type = (t); \
		CALL_CB(w, ev); \
	} while(0)

/* the event dispatcher generates high-level events (click, etc)
 * and calls the on_whatever() functions for both low and high-level
 * events.
 * The on_whatever functions are called *after* any other actions performed
 * here, to give subclasses the opportunity to override them easily, by
 * overriding the on_ functions, without having to override handle_event itself
 */
// TODO also call callbacks here I guess...
void Widget::handle_event(const Event &ev)
{
	switch(ev.type) {
	case EV_MOUSE_BUTTON:
		if(ev.button.press) {
			press();
		} else {
			if(is_pressed()) {
				CALL_CB_TYPE(this, EV_CLICK);
				on_click();
			}
			release();
		}

		on_mouse_button(ev.button);
		break;

	case EV_MOUSE_MOTION:
		on_mouse_motion(ev.motion);
		break;

	case EV_MOUSE_FOCUS:
		if(ev.focus.enter) {
			mousein();
		} else {
			mouseout();
		}
		on_mouse_focus(ev.focus);
		break;

	case EV_KEY:
		on_key(ev.key);
		break;

	case EV_CHANGE:
		on_change();
		break;

	default:
		break;
	}

	CALL_CB(this, ev);
}


void Widget::set_callback(EventType evtype, EventCallback func, void *cls)
{
	widget->cb[evtype].func = func;
	widget->cb[evtype].cls = cls;
}


}	// namespace goatkit
