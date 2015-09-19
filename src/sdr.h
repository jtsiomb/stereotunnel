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

#ifndef SDR_H_
#define SDR_H_

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

/* ---- shaders ---- */
unsigned int create_vertex_shader(const char *src);
unsigned int create_pixel_shader(const char *src);
unsigned int create_shader(const char *src, unsigned int sdr_type);
void free_shader(unsigned int sdr);

unsigned int load_vertex_shader(const char *fname);
unsigned int load_pixel_shader(const char *fname);
unsigned int load_shader(const char *src, unsigned int sdr_type);

unsigned int get_vertex_shader(const char *fname);
unsigned int get_pixel_shader(const char *fname);
unsigned int get_shader(const char *fname, unsigned int sdr_type);

int add_shader(const char *fname, unsigned int sdr);
int remove_shader(const char *fname);

/* ---- gpu programs ---- */
unsigned int create_program(void);
unsigned int create_program_link(unsigned int vs, unsigned int ps);
unsigned int create_program_load(const char *vfile, const char *pfile);
void free_program(unsigned int sdr);

void attach_shader(unsigned int prog, unsigned int sdr);
int link_program(unsigned int prog);
int bind_program(unsigned int prog);

int set_uniform_int(unsigned int prog, const char *name, int val);
int set_uniform_float(unsigned int prog, const char *name, float val);
int set_uniform_float3(unsigned int prog, const char *name, float x, float y, float z);
int set_uniform_float4(unsigned int prog, const char *name, float x, float y, float z, float w);
int set_uniform_matrix4(unsigned int prog, const char *name, float *mat);
int set_uniform_matrix4_transposed(unsigned int prog, const char *name, float *mat);

int get_attrib_loc(unsigned int prog, const char *name);
void set_attrib_float3(int attr_loc, float x, float y, float z);

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* SDR_H_ */
