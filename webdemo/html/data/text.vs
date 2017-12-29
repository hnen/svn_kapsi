precision mediump float;

attribute vec4 in_Pos;
varying highp vec2 v_tex;

uniform float img_aspect;
uniform vec2 tex_size;
uniform float scr_aspect;
uniform float draw_size;
uniform vec2 draw_pos;


void main() {
	vec3 pos = in_Pos.xyz;
	vec3 p = (vec3(draw_pos.xy,0.0) + pos * vec3(tex_size,1.0) * vec3(img_aspect,1,1) * draw_size );
	p.y *= scr_aspect;
	gl_Position = vec4( p, 1.0 );
	v_tex = ((in_Pos.xy + 1.0) * 0.5) * tex_size;
	v_tex.y = tex_size.y - v_tex.y;
}

