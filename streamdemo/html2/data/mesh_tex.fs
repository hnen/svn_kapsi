precision mediump float;
uniform sampler2D tex;

varying highp vec3 v_norm;
varying highp vec2 v_uv;

void main() {
	float l = -(v_norm.z)*0.7 + 0.2;
	vec4 color = texture2D(tex, v_uv);
	gl_FragColor = color * color.a;
//	gl_FragColor = vec4(vec3(1,1,1),1);
}


