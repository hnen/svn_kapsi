precision mediump float;

varying highp vec2 v_tex;
uniform sampler2D tex;
uniform float alpha;

void main() {
	vec4 color = texture2D( tex, v_tex );
	gl_FragColor = vec4( color.rgb, color.a * alpha);
	//gl_FragColor = vec4( 1,1,1,1 );
}

