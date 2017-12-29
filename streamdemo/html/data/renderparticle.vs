precision mediump float;

attribute vec4 in_id;
attribute vec4 in_corner;
attribute vec2 in_uv;

uniform sampler2D tex;
uniform float aspect;
uniform float tex_size_x;
uniform float tex_size_y;

varying highp vec2 v_tex;

const float block_size = 16.0;

vec2 id_to_uv(float id) {
	float tex_x = mod(id,tex_size_x)/tex_size_x + 0.5/tex_size_x;
	float tex_y = floor(id/tex_size_y)/tex_size_y + 0.5/tex_size_y;
	return vec2(tex_x, tex_y);
}

vec2 prt_pos(float id) {
	vec4 p_raw = texture2D(tex, id_to_uv(id));
	float px = p_raw.x / 255.0 + p_raw.y;
	float py = p_raw.z / 255.0 + p_raw.w;
	return (vec2(px, py) - 0.5) * 2.0;
}

void main() {
	vec3 c = in_corner.xyz;

	float id = in_id.x;

	vec2 p = prt_pos(id);

	vec2 np = prt_pos(id+1.0);
	vec2 d = normalize(np-p);

	c = vec3(d.y,-d.x,0)*c.x;

	gl_Position = vec4(vec3(p,0)+c*0.010*vec3(1.0/aspect,1,1), 1.0);
	v_tex = in_uv.xy;
}

