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
#include <GL/glew.h>
#include <GL/glut.h>
#include "sanegl.h"
#include "istereo.h"
#include "sdr.h"

void disp(void);
void keyb(unsigned char key, int x, int y);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(640, 920);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("test");

	glutDisplayFunc(disp);
	glutIdleFunc(glutPostRedisplay);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyb);

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

	default:
		break;
	}
}
