precision mediump float;

varying highp vec2 v_tex;
uniform sampler2D tex;

void main() {
	vec4 color = texture2D(tex, v_tex);
	gl_FragColor = vec4( color.r, 1.0, 1.0, 1.0);
}

