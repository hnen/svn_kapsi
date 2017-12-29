precision mediump float;

attribute vec4 in_Pos;
attribute vec3 in_Normal;
//attribute vec4 in_Color;

uniform highp mat4 projection;
uniform highp mat4 view;
uniform highp mat4 world;

varying mediump vec4 color;
varying mediump vec3 pos;

void main() {
	color = vec4(in_Normal,1);
	pos = in_Pos.xyz;
	gl_Position = projection * view * world * in_Pos;
}

