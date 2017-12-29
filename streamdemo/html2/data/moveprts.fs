precision highp float;
precision highp int;

varying highp vec2 v_tex;
uniform sampler2D tex;
uniform sampler2D tex_color;
uniform sampler2D vfield;

uniform int spawn_only_visible;

uniform int current_frame;
uniform int particlelifetime;
uniform float tex_size_x;
uniform float tex_size_y;
uniform float paska;

uniform int chain_length;
const int chain_spacing = 64;

float decode_val(vec2 px) {
	return (px.x * 255.0) / 65536.0 + (px.y * 255.0) / 256.0;
}

vec2 encode_val(float val) {
	val *= 65536.0;
	val = floor(val);
	float x = mod(val, 256.0) / 255.0;
	float y = floor(val / 256.0) / 255.0;
	return vec2(x,y);
}

vec2 decode_vec(vec4 px) {
	return vec2(decode_val(px.xy), decode_val(px.zw));
}

vec4 encode_vec(vec2 p) {
	vec2 v0 = encode_val(p.x);
	vec2 v1 = encode_val(p.y);
	return vec4(v0.x, v0.y, v1.x, v1.y);
}

float noise(int id) {
	return fract(sin(float(id*100)/100000.0)*231.0);
}

void main() {
	vec4 px = texture2D(tex, v_tex);
	vec2 pos = decode_vec(px);

	int pixel_y = int(v_tex.y * tex_size_y);
	int pixel_x = int(v_tex.x * tex_size_x);
	int id = pixel_y * int(tex_size_x) + pixel_x;

	vec4 vfpx = texture2D(vfield, pos);
	vec2 vec = (decode_vec(vfpx) - 0.5) * 2.0;

	// (x+iy)*(c+is) = xc + ixs + iyc - ys = xc - ys + i(xs+yc)
	float a = paska;
	float s = sin(a);
	float c = cos(a);
	vec = vec2(vec.x*c-vec.y*s,vec.x*s+vec.y*c);

	vec = normalize(vec) * min(max((length(vec) ), 0.2), 0.6);
	//vec = normalize(vec) * min(max((length(vec) ), 0.2), 0.6);

//	vec2 vec = (vfpx.zw - 0.5) * 2.0;
	vec2 rad = (pos - 0.5) * 10.0;

	int lt = 100;
	int frame = current_frame + int(noise(id) * 13121.0);
	int life_t = frame - (frame / lt) * lt;
		if (life_t == lt-1 || pos.x < 1.0 / 1024.0 || pos.y < 1.0 / 1024.0 || pos.x > (1.0 - 1.0/1024.0) || pos.y > (1.0 - 1.0/1024.0)) {
			pos = (vec2(noise(id*id+current_frame*id), noise(id*id+23141)) - vec2(0.5,0.5)) * 1.0 + vec2(0.5,0.5);
			if (dot(texture2D(tex_color, pos).rgb, vec3(0.33,0.33,0.33)) < 0.01) {
				pos = spawn_only_visible != 0 ? vec2(0,0) : pos;
			}
		} else {
			pos += vec * 0.006 ;
			//pos += vec2(vec.y, -vec.x) * paska*10.0;
		}

	gl_FragColor = encode_vec(pos);
}

