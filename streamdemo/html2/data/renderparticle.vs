precision mediump float;

attribute vec4 in_id;
attribute vec4 in_corner;
attribute vec2 in_uv;

uniform sampler2D tex;
uniform sampler2D tex_field;
uniform float aspect;
uniform float tex_size_x;
uniform float tex_size_y;
uniform float mul_len;
uniform float paska;

varying highp vec2 v_tex;
varying highp vec2 v_pos;

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

float decode_val(vec2 px) {
	return (px.x * 255.0) / 65536.0 + (px.y * 255.0) / 256.0;
}
vec2 decode_vec(vec4 px) {
	return vec2(decode_val(px.xy), decode_val(px.zw));
}

void main() {
	vec3 c = in_corner.xyz;

	float id = in_id.x;

	vec2 p = prt_pos(id);
	vec2 uv = (p + vec2(1.0,1.0)) * 0.5;
	vec4 dir_raw = texture2D(tex_field, uv);

	vec2 dir = (decode_vec(dir_raw) - 0.5) * 2.0;
	vec2 dirn = normalize(dir);
	float dirl = min(length(dir), 0.3);

	c.y *= 0.005 * 2.0;
	c.x *= 0.06 * dirl * 2.0 * mul_len;
	c = vec3(c.x * dirn.x - c.y * dirn.y, c.x * dirn.y + c.y * dirn.x, c.z);

	p += vec2(dirn.y, -dirn.x) * paska*0.03;

	gl_Position = vec4(vec3(p,0)+c*vec3(1.0/aspect,1,1), 1.0);
	v_tex = in_uv.xy;
	v_pos = (p.xy + 1.0) * 0.5;
}

