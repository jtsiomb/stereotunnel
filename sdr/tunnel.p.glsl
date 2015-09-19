#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D tex, tex_norm;

varying vec3 vpos, normal, tangent;
varying vec4 color, tc;
varying vec3 lpos;

void main()
{
	vec4 fog_color = vec4(0.0, 0.0, 0.0, 1.0);
	vec3 tcol = texture2D(tex, tc.xy * vec2(1.0, -1.0)).xyz;
	vec3 tnorm = texture2D(tex_norm, tc.xy * vec2(1.0, -1.0)).xyz;

	float fog = exp(-(0.2 * -vpos.z));
	
	vec3 ldir = lpos - vpos;
	float ldist = length(ldir);

	/* bring the light direction to tangent space */
	vec3 norm = normalize(normal);
	vec3 tang = normalize(tangent);
	vec3 bitan = cross(norm, tang);

	mat3 tbn_xform = mat3(tang.x, bitan.x, norm.x,
			tang.y, bitan.y, norm.y,
			tang.z, bitan.z, norm.z);

	vec3 l = normalize(tbn_xform * ldir);

	/* grab normal from the normalmap */
	vec3 n = normalize(tnorm * 2.0 - 1.0);

	float diffuse = max(dot(n, l), 0.0);

	/* blinn-phong specular */
	vec3 v = normalize(-vpos);
	vec3 h = normalize(v + l);
	float specular = pow(max(dot(n, h), 0.0), 60.0);

	const vec3 amb = vec3(0.02, 0.02, 0.02);

	float att = clamp(1.0 / (0.5 * (ldist * ldist)), 0.0, 1.0);

	vec3 dif = tcol * diffuse * att;
	vec3 spec = vec3(0.6, 0.6, 0.6) * specular * att;

	gl_FragColor = vec4(fog * (amb + dif + spec), 1.0);
}
