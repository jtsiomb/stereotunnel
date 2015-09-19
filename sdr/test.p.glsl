#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D tex;

varying vec3 vpos;
varying vec4 var_color;
varying vec2 var_texcoord;

void main()
{
	vec4 fog_color = vec4(0.6, 0.6, 0.6, 1.0);

	float fog = exp(-(0.2 * -vpos.z));

	vec4 texel = texture2D(tex, var_texcoord);
	texel.w = 1.0;

	vec4 col = var_color * texel;
	gl_FragColor = mix(fog_color, col, fog);
}
