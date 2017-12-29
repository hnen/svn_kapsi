precision mediump float;
uniform sampler2D tex_particle;
uniform sampler2D tex_bg_color;
uniform int colorize;
uniform float mul_brig;
varying highp vec2 v_tex;
varying highp vec2 v_pos;

void main() {
	vec4 texcol = texture2D(tex_particle, v_tex.yx);
	vec4 texbg = texture2D(tex_bg_color, v_pos);
	float diff = 1.0 - texcol.a;
	vec4 mul = colorize != 0 ? texbg : vec4(1,1,1,1);
	gl_FragColor = texcol * vec4(1.0,1.0,1.0,0.045) * mul * mul_brig;
}

