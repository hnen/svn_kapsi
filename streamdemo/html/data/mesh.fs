precision mediump float;

varying highp vec3 v_norm;

void main() {
	float l = -(v_norm.z)*0.7 + 0.2;
	vec4 color = vec4( vec3(1,1,1) * l, 1 );
	gl_FragColor = vec4(vec3(l,l,l),1);
}


