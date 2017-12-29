precision mediump float;
varying highp vec2 v_tex;
uniform sampler2D tex;

uniform float radius;
uniform float bias;
uniform vec2 dir;

const int NUM_SAMPLES = 64;

void main() {
	vec4 color = vec4(0,0,0,0);
	float rad = radius * 0.4;
	vec2 p = v_tex - dir * rad;
	float w = 0.0;
	for(int i = 0; i <= NUM_SAMPLES; i++) {
		float w0 = float(NUM_SAMPLES) - abs(float(i) - (float(NUM_SAMPLES) / 2.0));
		color += texture2D( tex, p ) * w0;
		w += w0;
		p += dir * rad * 2.0 / float(NUM_SAMPLES);
	}
	color /= pow(w, 0.99);
	gl_FragColor = color * 1.0;
//	gl_FragColor = texture2D(tex, v_tex);

//	gl_FragColor = vec4(1,1,1,1);
}

