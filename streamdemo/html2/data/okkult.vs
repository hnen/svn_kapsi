precision mediump float;

attribute vec4 in_pos;
varying highp vec2 v_tex;
uniform float aspect;
uniform mat4 local_to_screen;


void main() {
	//vec4 p = (local_to_screen * vec4(in_pos.xyz * vec3(size,1.0), 1.0));
	vec4 p = local_to_screen * vec4(in_pos.xyz * vec3(aspect,1.0,1.0) * 3.0, 1.0);
//	vec4 p = vec4(in_pos.xyz * vec3(aspect,1.0,1.0) * 0.1, 1.0);
	gl_Position = p;
	v_tex = ((in_pos.xy + 1.0) * 0.5);
}

