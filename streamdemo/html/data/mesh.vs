precision mediump float;

attribute vec4 in_pos;
attribute vec4 in_norm;
varying highp vec3 v_norm;

uniform mat4 local_to_screen;
uniform mat4 local_to_view;

void main() {
	//vec4 p = (vec4(in_pos.xyz * 0.1, 1.0) * local_to_screen);
	vec4 p = (local_to_screen * vec4(in_pos.xyz, 1.0));
	vec3 n = normalize((local_to_view * vec4(in_norm.xyz,0.0)).xyz);
	gl_Position = p;
	v_norm = n;
}

