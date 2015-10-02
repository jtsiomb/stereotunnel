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


#ifndef ISTEREO_H_
#define ISTEREO_H_

int init(void);
void cleanup(void);
void redraw(void);
void reshape(int x, int y);
void mouse_button(int bn, int press, int x, int y);
void mouse_motion(int x, int y);

void playpause(void);
void seektime(long msec);

#endif	/* ISTEREO_H_ */
