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
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "screen.h"
#include "widget.h"
#include "boolanm.h"

#define MAX_BUTTONS		16

namespace goatkit {

struct ScreenImpl {
	BoolAnim visible;
	std::vector<Widget*> widgets;
	BBox box;

	Widget *inp_focused, *over, *pressed[MAX_BUTTONS];
	Widget *mgrab;
};

static Vec2 world_to_scr(const ScreenImpl *scr, const Vec2 &v);
//static Vec2 scr_to_world(const ScreenImpl *scr, const Vec2 &v)

Screen::Screen()
{
	scr = new ScreenImpl;

	scr->box.bmin = Vec2(0, 0);
	scr->box.bmax = Vec2(1, 1);

	scr->inp_focused = scr->over = 0;
	for(int i=0; i<MAX_BUTTONS; i++) {
		scr->pressed[i] = 0;
	}

	scr->visible = true;
	scr->mgrab = 0;
}

Screen::~Screen()
{
	delete scr;
}

void Screen::set_position(float x, float y)
{
	set_position(Vec2(x, y));
}

void Screen::set_position(const Vec2 &pos)
{
	Vec2 sz = get_size();

	scr->box.bmin = pos;
	scr->box.bmax.x = pos.x + sz.x;
	scr->box.bmax.y = pos.y + sz.y;
}

const Vec2 &Screen::get_position() const
{
	return scr->box.bmin;
}

void Screen::set_size(float x, float y)
{
	set_size(Vec2(x, y));
}

void Screen::set_size(const Vec2 &sz)
{
	scr->box.bmax.x = scr->box.bmin.x + sz.x;
	scr->box.bmax.y = scr->box.bmin.y + sz.y;
}

const Vec2 Screen::get_size() const
{
	return Vec2(scr->box.bmax.x - scr->box.bmin.x,
			scr->box.bmax.y - scr->box.bmin.y);
}

const BBox &Screen::get_box() const
{
	return scr->box;
}

void Screen::add_widget(Widget *w)
{
	scr->widgets.push_back(w);
	if(scr->visible) {
		w->show();
	} else {
		w->hide();
	}

	w->set_screen(this);
}

int Screen::get_widget_count() const
{
	return (int)scr->widgets.size();
}

Widget *Screen::get_widget(int idx) const
{
	if(idx < 0 || idx >= (int)scr->widgets.size()) {
		return 0;
	}
	return scr->widgets[idx];
}

Widget *Screen::get_widget(const char *name) const
{
	for(size_t i=0; i<scr->widgets.size(); i++) {
		if(strcmp(scr->widgets[i]->get_name(), name) == 0) {
			return scr->widgets[i];
		}
	}
	return 0;
}

void Screen::show()
{
	scr->visible.change(true);

	for(size_t i=0; i<scr->widgets.size(); i++) {
		scr->widgets[i]->show();
	}
}

void Screen::hide()
{
	scr->visible.change(false);

	for(size_t i=0; i<scr->widgets.size(); i++) {
		scr->widgets[i]->hide();
	}
}

bool Screen::is_visible() const
{
	return scr->visible;
}

float Screen::get_visibility() const
{
	return scr->visible.get_value();
}

void Screen::set_visibility_transition(long msec)
{
	scr->visible.set_transition_duration(msec);
	for(size_t i=0; i<scr->widgets.size(); i++) {
		scr->widgets[i]->set_visibility_transition(msec);
	}
}

long Screen::get_visibility_transition() const
{
	return scr->visible.get_transition_duration();
}

bool Screen::grab_mouse(Widget *w)
{
	if(!scr->mgrab || !w) {
		scr->mgrab = w;
		return true;
	}
	return false;
}

void Screen::draw() const
{
	for(size_t i=0; i<scr->widgets.size(); i++) {
		scr->widgets[i]->draw();
	}
}

static Widget *find_widget_at(const ScreenImpl *scr, const Vec2 &pt)
{
	for(size_t i=0; i<scr->widgets.size(); i++) {
		Widget *w = scr->widgets[i];

		if(w->hit_test(pt)) {
			return w;
		}
	}
	return 0;
}

void Screen::sysev_keyboard(int key, bool press)
{
	Event ev;

	if(scr->inp_focused) {
		ev.type = EV_KEY;
		ev.key.key = key;
		ev.key.press = press;
		scr->inp_focused->handle_event(ev);
	}
}

void Screen::sysev_mouse_button(int bn, bool press, float x, float y)
{
	Event ev;
	Vec2 pt = world_to_scr(scr, Vec2(x, y));
	Widget *new_over = scr->mgrab ? scr->mgrab : find_widget_at(scr, pt);

	ev.type = EV_MOUSE_BUTTON;
	ev.button.button = bn;
	ev.button.pos = pt;
	ev.button.press = press;

	if(press) {
		if(bn == 0) {
			// left click gives input focus
			// TODO: add input focus event in widget
			if(new_over && new_over != scr->inp_focused && new_over->can_focus()) {
				printf("input focus %p -> %p\n", (void*)scr->inp_focused, (void*)new_over);
				new_over->focusin();

				if(scr->inp_focused) {
					scr->inp_focused->focusout();
				}
				scr->inp_focused = new_over;
			}
		}

		scr->pressed[bn] = new_over;
		scr->over = new_over;

		if(new_over) {
			new_over->handle_event(ev);
		}

	} else {
		// send the mouse release event to the widget that got the matching press
		if(scr->pressed[bn]) {
			scr->pressed[bn]->handle_event(ev);
			scr->pressed[bn] = 0;
		}
	}

	// if we're not over the same widget any more send the leave/enter events
	// TODO also add drag/drop events
	if(scr->over != new_over) {
		ev.type = EV_MOUSE_FOCUS;
		if(scr->over) {
			ev.focus.enter = false;
			scr->over->handle_event(ev);
		}
		if(new_over) {
			ev.focus.enter = true;
			new_over->handle_event(ev);
		}
		scr->over = new_over;
	}
}

void Screen::sysev_mouse_motion(float x, float y)
{
	Event ev;
	Vec2 pt = world_to_scr(scr, Vec2(x, y));
	Widget *new_over = scr->mgrab ? scr->mgrab : find_widget_at(scr, pt);

	// if we're not over the same widget any more send the leave/enter events
	if(scr->over != new_over) {
		ev.type = EV_MOUSE_FOCUS;
		if(scr->over) {
			ev.focus.enter = false;
			scr->over->handle_event(ev);
		}
		if(new_over) {
			ev.focus.enter = true;
			new_over->handle_event(ev);
		}
		scr->over = new_over;
	}

	if(new_over) {
		// send motion event
		ev.type = EV_MOUSE_MOTION;
		ev.motion.pos = pt;
		new_over->handle_event(ev);
	}
}

static Vec2 world_to_scr(const ScreenImpl *scr, const Vec2 &v)
{
	return Vec2(v.x - scr->box.bmin.x, v.y - scr->box.bmin.y);
}

/*
static Vec2 scr_to_world(const ScreenImpl *scr, const Vec2 &v)
{
	return Vec2(v.x + scr->box.bmin.x, v.y + scr->box.bmin.y);
}
*/


} // namespace goatkit
