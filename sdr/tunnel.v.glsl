attribute vec4 attr_vertex, attr_color;
attribute vec2 attr_texcoord;
attribute vec4 attr_tangent;
attribute vec3 attr_normal;

uniform mat4 matrix_modelview, matrix_projection, matrix_texture;
uniform mat3 matrix_normal;
uniform float t;
uniform vec4 light_pos;

varying vec3 vpos, normal, tangent;
varying vec4 tc;
varying vec3 lpos;

void main()
{
	mat4 mvp = matrix_projection * matrix_modelview;
	gl_Position = mvp * attr_vertex;

	vpos = (matrix_modelview * attr_vertex).xyz;

	normal = matrix_normal * attr_normal;
	tangent = matrix_normal * attr_tangent.xyz;

	lpos = (matrix_modelview * light_pos).xyz;

	tc = matrix_texture * vec4(attr_texcoord, 0.0, 1.0);
}
