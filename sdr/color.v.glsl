attribute vec4 attr_vertex, attr_color;

uniform mat4 matrix_modelview, matrix_projection;

varying vec4 vcolor;

void main()
{
	mat4 mvp = matrix_projection * matrix_modelview;
	gl_Position = mvp * attr_vertex;
	vcolor = attr_color;
}
