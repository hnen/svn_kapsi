precision mediump float;

uniform sampler2D tex;

varying highp vec2 v_tex;

void main() {
	vec4 color = texture2D( tex, v_tex );
	gl_FragColor = vec4(color.a,color.a,color.a,1.0);
}

