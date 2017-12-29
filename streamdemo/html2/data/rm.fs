precision mediump float;
varying highp vec2 v_tex;

void main() {



	vec4 color = texture2D(tex, v_tex);
	gl_FragColor = color;
}

