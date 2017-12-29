precision mediump float;
uniform sampler2D tex_particle;
varying highp vec2 v_tex;

void main() {
	vec4 texcol = texture2D(tex_particle, v_tex.yx);
	float diff = 1.0 - texcol.a;
	//gl_FragColor = vec4(fract(v_tex.x), fract(v_tex.y), 0, 1);
	//gl_FragColor = vec4(diff,diff,diff,1.0);
	gl_FragColor = texcol * texcol.a * vec4(1.0,1.0,1.0,0.4);
}

