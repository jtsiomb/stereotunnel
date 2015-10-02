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
#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "sanegl.h"
#include "istereo.h"
#include "sdr.h"

void disp(void);
void keyb(unsigned char key, int x, int y);
void mouse(int bn, int st, int x, int y);
void motion(int x, int y);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(920, 640);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("test");

	glutDisplayFunc(disp);
	glutIdleFunc(glutPostRedisplay);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyb);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glewInit();

	if(init() == -1) {
		return 1;
	}

	glutMainLoop();
	return 0;
}

void disp(void)
{
	redraw();

	glutSwapBuffers();
}

extern int stereo;
extern int use_bump;

void keyb(unsigned char key, int x, int y)
{
	switch(key) {
	case 27:
		exit(0);

	case 's':
		stereo = !stereo;
		break;

	case 'b':
		use_bump = !use_bump;
		break;

	case '`':
		{
			int xsz = glutGet(GLUT_WINDOW_WIDTH);
			int ysz = glutGet(GLUT_WINDOW_HEIGHT);

			glutReshapeWindow(ysz, xsz);
		}
		break;

	case ' ':
		playpause();
		break;

	case '.':
		seektime(250);
		break;

	case ',':
		seektime(-250);
		break;

	default:
		break;
	}
}

void mouse(int bn, int st, int x, int y)
{
	mouse_button(bn - GLUT_LEFT_BUTTON, st == GLUT_DOWN ? 1 : 0, x, y);
}

void motion(int x, int y)
{
	mouse_motion(x, y);
}

/* dummy ad banner functions */
void ad_banner_show(void)
{
}

void ad_banner_hide(void)
{
}
