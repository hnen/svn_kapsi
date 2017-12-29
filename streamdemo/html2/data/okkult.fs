precision mediump float;

varying highp vec2 v_tex;
uniform sampler2D tex;
uniform float alpha;

void main() {
	vec2 uv = vec2(v_tex.x, 1.0 - v_tex.y);
	vec4 texcolor = texture2D(tex, uv);
	float i = (1.0 - texcolor.r) * alpha;
	vec4 color = vec4(i,i,i,1);
	gl_FragColor = color;
//	gl_FragColor = vec4(1,1,1,1);
}

