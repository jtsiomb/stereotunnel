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
#include <ctype.h>
#include <limits.h>
#include <string>
#include "textbox.h"

namespace goatkit {

struct TextBoxImpl {
	std::string text;
	int cursor;
};

TextBox::TextBox()
{
	tbox = new TextBoxImpl;
	tbox->cursor = 0;
}

TextBox::~TextBox()
{
	delete tbox;
}

const char *TextBox::get_type_name() const
{
	return "textbox";
}

bool TextBox::can_focus() const
{
	return true;
}

void TextBox::clear()
{
	tbox->text.clear();
}

void TextBox::set_text(const char *t)
{
	tbox->text = std::string(t);
}

const char *TextBox::get_text() const
{
	return tbox->text.c_str();
}

int TextBox::set_cursor(int idx)
{
	int len = tbox->text.size();

	if(idx < 0) {
		tbox->cursor = 0;
	} else if(idx > len) {
		tbox->cursor = len;
	} else {
		tbox->cursor = idx;
	}
	return tbox->cursor;
}

int TextBox::get_cursor() const
{
	return tbox->cursor;
}

int TextBox::cursor_begin()
{
	return tbox->cursor = 0;
}

int TextBox::cursor_end()
{
	return set_cursor(INT_MAX);
}

int TextBox::cursor_prev()
{
	return set_cursor(tbox->cursor - 1);
}

int TextBox::cursor_next()
{
	return set_cursor(tbox->cursor + 1);
}

void TextBox::insert(char c)
{
	int len = tbox->text.size();
	if(tbox->cursor >= len) {
		tbox->text.push_back(c);
		tbox->cursor++;
	} else {
		tbox->text.insert(tbox->cursor++, 1, c);
	}
}

void TextBox::on_key(const KeyEvent &ev)
{
	if(!ev.press) return;	// ignore key release events

	switch(ev.key) {
	case KEY_LEFT:
		cursor_prev();
		break;

	case KEY_RIGHT:
		cursor_next();
		break;

	case KEY_HOME:
		cursor_begin();
		break;

	case KEY_END:
		cursor_end();
		break;

	case KEY_DELETE:
		tbox->text.erase(tbox->cursor, 1);
		break;

	case '\b':
		if(tbox->cursor > 0) {
			tbox->text.erase(--tbox->cursor, 1);
		}
		break;

	case '\n':
	case '\t':
		{
			Event ev;
			ev.type = EV_CHANGE;
			handle_event(ev);
		}
		break;

	default:
		if(isprint(ev.key)) {
			insert(ev.key);
		}
	}
}

void TextBox::on_click()
{
	// TODO place cursor
}

}	// namespace goatkit
