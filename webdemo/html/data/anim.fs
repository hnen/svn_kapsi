precision mediump float;

varying highp vec2 v_tex;
uniform sampler2D tex0;
uniform float ip;
uniform float mul;

void main() {
	vec4 color = texture2D( tex0, v_tex );
	color.a = ip * 3.0;
	color.rgb *= mul;
	gl_FragColor = color;
}

