/*
GoatKit - a themable/animated widget toolkit for games
Copyright (C) 2014-2015 John Tsiombikas <nuclear@member.fsf.org>

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
#include "config.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <string.h>
#include "theme.h"
#include "widget.h"

#ifdef WIN32
#include <windows.h>

#define RTLD_DEFAULT	((void*)0)

static void *dlopen(const char *name, int flags);
static void dlclose(void *so);
static void *dlsym(void *so, const char *symbol);
#else
#include <unistd.h>
#include <dlfcn.h>
#endif

#ifdef HAVE_OPENGL_H
#include "opengl.h"

#else

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#endif	/* HAVE_OPENGL_H_ */

#ifndef PREFIX
#define PREFIX	"/usr/local"
#endif

namespace goatkit {

struct ThemeImpl {
	void *so;
	WidgetDrawFunc (*lookup_theme_draw_func)(const char*);
	mutable std::map<std::string, WidgetDrawFunc> func_cache;
};

Theme *theme;
static std::vector<std::string> search_paths;
static const char *fallback_paths[] = {
	PREFIX "/share/goatkit",
	0
};

void add_theme_path(const char *path)
{
	if(!path || !*path) return;

	std::string s = path;
	int last = s.length() - 1;
	if(s[last] == '/' || s[last] == '\\') {
		s.erase(last);
	}

	if(std::find(search_paths.begin(), search_paths.end(), s) != search_paths.end()) {
		return;
	}

	search_paths.push_back(s);
}

Theme::Theme()
{
	impl = new ThemeImpl;
	impl->so = 0;
	impl->lookup_theme_draw_func = 0;
}

Theme::~Theme()
{
	unload();
	delete impl;
}

typedef WidgetDrawFunc (*LookupFunc)(const char*);

bool Theme::load(const char *name)
{
	unload();

	if(strcmp(name, "GOATKIT_THEME_BUILTIN") == 0) {
		impl->so = RTLD_DEFAULT;
	} else {
		std::string fname = std::string(name) + ".gtheme";
		if(!(impl->so = dlopen(fname.c_str(), RTLD_LAZY))) {
			for(size_t i=0; i<search_paths.size(); i++) {
				std::string path = search_paths[i] + "/" + fname;

				if((impl->so = dlopen(path.c_str(), RTLD_LAZY))) {
					break;
				}
			}

			// try the fallback paths
			if(!impl->so) {
				for(int i=0; fallback_paths[i]; i++) {
					std::string path = std::string(fallback_paths[i]) + "/" + fname;

					if((impl->so = dlopen(path.c_str(), RTLD_LAZY))) {
						break;
					}
				}
			}

			if(!impl->so) {
				fprintf(stderr, "%s: failed to load theme plugin: %s\n", __func__, name);
				return false;
			}
		}
	}

	// loaded the shared object, now get the lookup function
	impl->lookup_theme_draw_func = (LookupFunc)dlsym(impl->so, "get_widget_func");
	if(!impl->lookup_theme_draw_func) {
		fprintf(stderr, "%s: invalid theme plugin %s\n", __func__, name);
		unload();
		return false;
	}

	return true;
}

void Theme::unload()
{
	if(impl->so) {
		if(impl->so != RTLD_DEFAULT) {
			dlclose(impl->so);
		}
		impl->so = 0;
	}
	impl->func_cache.clear();
}

WidgetDrawFunc Theme::get_draw_func(const char *type) const
{
	std::map<std::string, WidgetDrawFunc>::const_iterator it = impl->func_cache.find(type);
	if(it == impl->func_cache.end()) {
		// don't have it cached, try to look it up
		WidgetDrawFunc func;
		if(impl->lookup_theme_draw_func && (func = impl->lookup_theme_draw_func(type))) {
			impl->func_cache[type] = func;
			return func;
		}

		// can't look it up, return the default
		return default_draw_func;
	}
	return it->second;
}

#define LERP(a, b, t)	((a) + ((b) - (a)) * t)
#define DEF_TEX_SZ	32
void default_draw_func(const Widget *w)
{
	static unsigned int tex;

	if(!tex) {
		unsigned char *pixels = new unsigned char[DEF_TEX_SZ * DEF_TEX_SZ * 3];
		unsigned char *ptr = pixels;
		for(int i=0; i<DEF_TEX_SZ; i++) {
			for(int j=0; j<DEF_TEX_SZ; j++) {
				bool stripe = (((j + i) / 8) & 1) == 1;
				ptr[0] = ptr[1] = ptr[2] = stripe ? 255 : 0;
				ptr += 3;
			}
		}

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DEF_TEX_SZ, DEF_TEX_SZ, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		delete [] pixels;
	}

	Vec2 pos = w->get_position();
	Vec2 sz = w->get_size();
	float aspect = sz.x / sz.y;

#if !defined(GL_ES_VERSION_2_0)
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_TEXTURE_2D);
#endif
	glBindTexture(GL_TEXTURE_2D, tex);

	float offs = w->get_pressed() * 0.1 * sz.y;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(offs, -offs, 0);

	float active = w->get_active();
	float hover = w->get_under_mouse();

	float rg = LERP(0.4, 1.0, hover);
	float b = LERP(rg, 0, active);
	glColor3f(rg, rg, b);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex2f(pos.x, pos.y);
	glTexCoord2f(aspect, 1);
	glVertex2f(pos.x + sz.x, pos.y);
	glTexCoord2f(aspect, 0);
	glVertex2f(pos.x + sz.x, pos.y + sz.y);
	glTexCoord2f(0, 0);
	glVertex2f(pos.x, pos.y + sz.y);
	glEnd();

	glPopMatrix();

#ifndef GL_ES_VERSION_2_0
	glPopAttrib();
#endif
}

}	// namespace goatkit

#ifdef WIN32
// XXX untested
static void *dlopen(const char *name, int flags)
{
	return LoadLibrary(name);
}

static void dlclose(void *so)
{
	FreeLibrary(so);
}

static void *dlsym(void *so, const char *symbol)
{
	if(so == RTLD_DEFAULT) {
		so = GetModuleHandle(0);
	}
	return (void*)GetProcAddress(so, symbol);
}
#endif
