precision mediump float;
varying highp vec2 v_tex;
uniform sampler2D tex;

uniform float radius;
uniform vec2 dir;

const int NUM_SAMPLES = 64;

vec2 encode_val(float val) {
	val *= 65536.0;
	val = floor(val);
	float x = mod(val, 256.0) / 255.0;
	float y = floor(val / 256.0) / 255.0;
	return vec2(x,y);
}

vec4 encode_vec(vec2 p) {
	p.x = min(1.0,p.x);
	p.y = min(1.0,p.y);
	p.x = max(0.0,p.x);
	p.y = max(0.0,p.y);
	vec2 v0 = encode_val(p.x);
	vec2 v1 = encode_val(p.y);
	return vec4(v0.x, v0.y, v1.x, v1.y);
}

float decode_val(vec2 px) {
	return (px.x * 255.0) / 65536.0 + (px.y * 255.0) / 256.0;
}
vec2 decode_vec(vec4 px) {
	return vec2(decode_val(px.xy), decode_val(px.zw));
}

void main() {
	vec2 vec = vec2(0,0);
	float rad = radius * 0.4;
	vec2 p = v_tex - dir * rad;
	float w = 0.0;
	for( int i = 0; i <= NUM_SAMPLES; i++ ) {
		float w0 = float(NUM_SAMPLES) - abs(float(i) - (float(NUM_SAMPLES) / 2.0));
		vec += decode_vec(texture2D(tex, p)) * w0;
		w += w0;
		p += dir * rad * 2.0 / float(NUM_SAMPLES);
	}
	vec /= w;
	gl_FragColor = encode_vec(vec);
}

