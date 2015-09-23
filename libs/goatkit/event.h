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
#ifndef EVENT_H_
#define EVENT_H_

#include "vec.h"

namespace goatkit {

enum EventType {
	// primary events
	EV_MOUSE_BUTTON,
	EV_MOUSE_MOTION,
	EV_MOUSE_FOCUS,
	EV_KEY,

	// derived events
	EV_CLICK,
	EV_DOUBLE_CLICK,
	EV_CHANGE,

	NUM_EVENTS
};

enum SpecialKeys {
	KEY_ESCAPE = 27,
	KEY_DELETE = 127,

	KEY_F1 = 256, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
	KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
	KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN,
	KEY_HOME, KEY_END, KEY_PGUP, KEY_PGDOWN,
	KEY_INSERT
};

struct ButtonEvent {
	Vec2 pos;
	int button;
	bool press;
};

struct MotionEvent {
	Vec2 pos;
};

struct FocusEvent {
	bool enter;
};

struct KeyEvent {
	int key;
	bool press;
};

struct Event {
	EventType type;

	ButtonEvent button;
	MotionEvent motion;
	FocusEvent focus;
	KeyEvent key;
};

const char *event_type_name(EventType type);

}	// namespace goatkit

#endif	// EVENT_H_
