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
#include <unistd.h>
#include "respath.h"
#include "config.h"
#include "assman.h"

#ifdef IPHONE
#include <CoreFoundation/CoreFoundation.h>
#endif


#ifndef IPHONE
struct path_node {
	char *path;
	struct path_node *next;
};

static struct path_node *pathlist;

void add_resource_path(const char *path)
{
	struct path_node *node = 0;

	if(!(node = malloc(sizeof *node)) || !(node->path = malloc(strlen(path) + 1))) {
		free(node);
		fprintf(stderr, "failed to add path: %s: %s\n", path, strerror(errno));
		return;
	}
	strcpy(node->path, path);
	node->next = pathlist;
	pathlist = node;
}

char *find_resource(const char *fname, char *path, size_t sz)
{
	static char buffer[1024];
	struct path_node *node;

	if(!path) {
		path = buffer;
		sz = sizeof buffer;
	}

	node = pathlist;
	while(node) {
		ass_file *fp;

		snprintf(path, sz, "%s/%s", node->path, fname);
		if((fp = ass_fopen(path, "r"))) {
			ass_fclose(fp);
			return path;
		}
		node = node->next;
	}

	fprintf(stderr, "can't find resource: %s\n", fname);
	return 0;
}

#else	/* IPHONE */

void add_resource_path(const char *path)
{
}


char *find_resource(const char *fname, char *path, size_t sz)
{
	static char buffer[1024];
	CFBundleRef bundle;
	CFURLRef url;
	CFStringRef cfname;

	cfname = CFStringCreateWithCString(0, fname, kCFStringEncodingASCII);

	bundle = CFBundleGetMainBundle();
	if(!(url = CFBundleCopyResourceURL(bundle, cfname, 0, 0))) {
		return 0;
	}

	if(!path) {
		path = buffer;
		sz = sizeof buffer;
	}

	if(!CFURLGetFileSystemRepresentation(url, 1, (unsigned char*)path, sz)) {
		return 0;
	}
	return path;
}
#endif
