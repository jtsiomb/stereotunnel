#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D tex;

varying vec4 color;
varying vec3 vpos;
varying vec2 tc;

void main()
{
	vec4 tcol = texture2D(tex, tc);

	float fog = exp(-(0.2 * -vpos.z));

	float alpha = fog * color.a;

	vec4 color = vec4(1.0, 0.3, 0.2, tcol.x * alpha);

	gl_FragColor = color;
}
