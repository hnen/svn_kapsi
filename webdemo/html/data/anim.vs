precision mediump float;

attribute vec4 in_Pos;
varying highp vec2 v_tex;

void main() {
	vec3 p = in_Pos.xyz;
	gl_Position = vec4( p, 1.0 );
	v_tex = (in_Pos.xy + 1.0) * 0.5;
	v_tex.y = 1.0 - v_tex.y;
}

