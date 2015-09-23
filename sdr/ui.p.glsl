#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D tex;

varying vec4 vcolor;
varying vec2 vtexcoord;

void main()
{
	vec4 texel = texture2D(tex, vtexcoord);
	gl_FragColor = vcolor * texel;
}
