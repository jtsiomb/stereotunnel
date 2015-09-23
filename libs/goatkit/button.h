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
#ifndef GOATKIT_BUTTON_H_
#define GOATKIT_BUTTON_H_

#include "widget.h"

namespace goatkit {

struct ButtonImpl;

class Button : public Widget {
private:
	ButtonImpl *button;

public:
	Button();
	virtual ~Button();

	virtual const char *get_type_name() const;
};

}	// namespace goatkit

#endif	// GOATKIT_BUTTON_H_
