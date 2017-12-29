precision mediump float;

attribute vec4 in_pos;
varying highp vec2 v_tex;
uniform vec2 size;

void main() {
	vec3 p = in_pos.xyz;
	gl_Position = vec4( p, 1.0 );
	v_tex = ((in_pos.xy + 1.0) * 0.5);// * size;
}

