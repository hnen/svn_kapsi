precision mediump float;

varying highp vec2 v_tex;
uniform sampler2D tex;

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

float b(vec4 c) {
	return dot(c, vec4(0.333,0.333,0.333,0));
	//return dot(c, vec4(0,0,0,1));
}

void main() {
	float c0 = b(texture2D(tex, v_tex - vec2(1.0,0.0)/512.0));
	float c1 = b(texture2D(tex, v_tex - vec2(0.0,1.0)/512.0));
	float c2 = b(texture2D(tex, v_tex + vec2(1.0,0.0)/512.0));
	float c3 = b(texture2D(tex, v_tex + vec2(0.0,1.0)/512.0));

	float dx = c2-c0;
	float dy = c3-c1;

	float x = dy;
	float y =-dx;

	vec2 v = vec2(x,y) * 32.0;

	float v0 = (v.x+1.0)*0.5;
	float v1 = (v.y+1.0)*0.5;

	gl_FragColor = encode_vec(vec2(v0,v1));
//	gl_FragColor = vec4(v0, v1, v0, v1);

}

