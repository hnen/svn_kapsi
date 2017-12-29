precision mediump float;

attribute vec4 in_pos;
attribute vec4 in_norm;
varying highp vec3 v_norm;

uniform mat4 local_to_screen;
uniform mat4 local_to_view;

uniform float t;

void main() {
	//vec4 p = (vec4(in_pos.xyz * 0.1, 1.0) * local_to_screen);
	float y = abs(in_pos.z * 2.0);
	vec3 xy = vec3(normalize(in_pos.xy),0);
	float rad = atan(xy.x, xy.y);
	float off = sin(t*1.5+y*0.2 + rad*2.0)*y*y*0.04;
	off *= off * 1.0;
	vec3 objpos = in_pos.xyz + xy * off;

	vec4 p = (local_to_screen * vec4(objpos, 1.0));
	vec3 n = normalize((local_to_view * vec4(in_norm.xyz,0.0)).xyz);
	gl_Position = p;
	v_norm = n;
}

