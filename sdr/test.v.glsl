uniform mat4 matrix_modelview, matrix_projection, matrix_texture;

attribute vec4 attr_vertex, attr_color;
attribute vec2 attr_texcoord;

varying vec3 vpos;
varying vec4 var_color;
varying vec2 var_texcoord;

void main()
{
	mat4 mvp = matrix_projection * matrix_modelview;
	gl_Position = mvp * attr_vertex;
	vpos = (matrix_modelview * attr_vertex).xyz;
	var_color = attr_color;
	var_texcoord = (matrix_texture * vec4(attr_texcoord, 0.0, 1.0)).xy;
}
