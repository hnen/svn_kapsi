
precision mediump float;

attribute vec4 in_Pos;

uniform float img_aspect;
uniform mat4 wvp;

void main() {
	vec3 pos = in_Pos.xyz;
	gl_Position = vec4(pos,1) * vwp;
}

