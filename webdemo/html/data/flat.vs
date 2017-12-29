precision mediump float;

attribute vec4 in_Pos;
//attribute vec4 in_Color;

uniform highp mat4 projection;
uniform highp mat4 view;
uniform highp mat4 world;

//varying mediump vec4 color;

void main() {
	gl_Position = projection * view * world * in_Pos;
}

