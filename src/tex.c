/*
Stereoscopic tunnel for iOS.
Copyright (C) 2011  John Tsiombikas <nuclear@member.fsf.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "opengl.h"
#include "tex.h"
#include "config.h"
#include "imago2.h"

unsigned int load_texture(const char *fname)
{
	int xsz, ysz;
	unsigned int tex;
	void *pixels;

	if(!fname) {
		return 0;
	}
	if(!(pixels = img_load_pixels(fname, &xsz, &ysz, IMG_FMT_RGBA32))) {
		fprintf(stderr, "failed to load image: %s\n", fname);
		return 0;
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xsz, ysz, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	img_free_pixels(pixels);

	return tex;
}

void bind_texture(unsigned int tex, int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);

#ifndef IPHONE
	if(tex) {
		glEnable(GL_TEXTURE_2D);
	} else {
		glDisable(GL_TEXTURE_2D);
	}
#endif

	glBindTexture(GL_TEXTURE_2D, tex);
}
