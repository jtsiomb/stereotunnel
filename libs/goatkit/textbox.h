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
#ifndef TEXTBOX_H_
#define TEXTBOX_H_

#include "widget.h"

namespace goatkit {

struct TextBoxImpl;

class TextBox : public Widget {
private:
	TextBoxImpl *tbox;

public:
	TextBox();
	virtual ~TextBox();

	virtual const char *get_type_name() const;
	virtual bool can_focus() const;

	virtual void clear();

	virtual void set_text(const char *t);
	virtual const char *get_text() const;

	virtual int set_cursor(int idx);
	virtual int get_cursor() const;

	virtual int cursor_begin();
	virtual int cursor_end();
	virtual int cursor_prev();
	virtual int cursor_next();

	virtual void insert(char c);

	virtual void on_key(const KeyEvent &ev);
	virtual void on_click();
};

}	// namespace goatkit

#endif	// TEXTBOX_H_
