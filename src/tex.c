/*
Stereoscopic tunnel for iOS.
Copyright (C) 2011-2015 John Tsiombikas <nuclear@member.fsf.org>

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
#include "assman.h"

static size_t ioread(void *buf, size_t bytes, void *uptr);
static long ioseek(long offs, int whence, void *uptr);

unsigned int load_texture(const char *fname)
{
	unsigned int tex;
	ass_file *fp;
	struct img_io io;
	struct img_pixmap img;

	if(!fname) {
		return 0;
	}
	if(!(fp = ass_fopen(fname, "rb"))) {
		fprintf(stderr, "failed to open texture file: %s: %s\n", fname, strerror(errno));
		return 0;
	}
	io.uptr = fp;
	io.read = ioread;
	io.write = 0;
	io.seek = ioseek;

	img_init(&img);
	if(img_read(&img, &io) == -1) {
		fprintf(stderr, "failed to load image: %s\n", fname);
		ass_fclose(fp);
		return 0;
	}
	ass_fclose(fp);
	img_convert(&img, IMG_FMT_RGBA32);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#ifdef __GLEW_H__
	if(GLEW_SGIS_generate_mipmap) {
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
#endif
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, img.pixels);
#ifdef __GLEW_H__
	} else {
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.width, img.height,
				GL_RGBA, GL_UNSIGNED_BYTE, img.pixels);
	}
#endif

#ifdef GL_ES_VERSION_2_0
	glGenerateMipmap(GL_TEXTURE_2D);
#endif
	img_destroy(&img);

	return tex;
}

void bind_texture(unsigned int tex, int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);

#ifndef GL_ES_VERSION_2_0
	if(tex) {
		glEnable(GL_TEXTURE_2D);
	} else {
		glDisable(GL_TEXTURE_2D);
	}
#endif

	glBindTexture(GL_TEXTURE_2D, tex);
}

static size_t ioread(void *buf, size_t bytes, void *uptr)
{
	return ass_fread(buf, 1, bytes, uptr);
}

static long ioseek(long offs, int whence, void *uptr)
{
	return ass_fseek(uptr, offs, whence);
}
