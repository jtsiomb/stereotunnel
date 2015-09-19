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

#ifndef CAM_H_
#define CAM_H_

enum {
	CAM_CENTER,
	CAM_LEFT,
	CAM_RIGHT
};

/* reset to the initial state */
void cam_reset(void);			/* all */
void cam_reset_view(void);		/* view parameters */
void cam_reset_proj(void);		/* projection parameters */
void cam_reset_stereo(void);	/* stereo parameters */

/* camera viewing parameters */
void cam_pan(int dx, int dy);		/* pan across X/Z plane */
void cam_height(int dh);			/* move verticaly */
void cam_rotate(int dx, int dy);	/* rotate around local Y and X axis */
void cam_zoom(int dz);				/* dolly the camera fwd/back */

/* camera projection parameters */
void cam_clip(float n, float f);	/* set clipping planes */
void cam_fov(float f);				/* vertical field of view in degrees */
void cam_aspect(float a);			/* aspect ratio (width / height) */

/* stereo parameters */
void cam_separation(float s);
void cam_focus_dist(float d);

/* multiply the camera view matrix on top of the current matrix stack
 * (which should be GL_MODELVIEW)
 */
void cam_view_matrix(void);
void cam_stereo_view_matrix(int eye);

/* multiply the camera projection matrix on top of the current matrix stack
 * (which should be GL_PROJECTION)
 */
void cam_proj_matrix(void);
void cam_stereo_proj_matrix(int eye);

#endif	/* CAM_H_ */
