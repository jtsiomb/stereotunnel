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
#ifndef THEME_H_
#define THEME_H_

#define GOATKIT_THEME_BUILTIN	"GOATKIT_THEME_BUILTIN"

namespace goatkit {

class Widget;

typedef void (*WidgetDrawFunc)(const Widget*);

void add_theme_path(const char *path);
void default_draw_func(const Widget *w);

struct ThemeImpl;

class Theme {
private:
	ThemeImpl *impl;

public:
	Theme();
	~Theme();

	bool load(const char *name);
	void unload();

	WidgetDrawFunc get_draw_func(const char *type) const;
};

extern Theme *theme;	// the current theme

}	// namespace goatkit

#endif	// THEME_H_
