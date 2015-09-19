attribute vec4 attr_vertex, attr_texcoord, attr_color;

uniform mat4 matrix_modelview, matrix_projection;
uniform float idx;

varying vec4 color;
varying vec3 vpos;
varying vec2 tc;

void main()
{
	mat4 mvp = matrix_projection * matrix_modelview;
	gl_Position = mvp * attr_vertex;

	vpos = (matrix_modelview * attr_vertex).xyz;

	float sz = 1.0 / 17.0;
	tc = vec2(attr_texcoord.x, (attr_texcoord.y + idx) * sz);

	color = attr_color;
}
