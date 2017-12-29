precision mediump float;

varying highp vec2 v_tex;
uniform sampler2D tex;

uniform float radius; 
uniform vec2 dir;
uniform float brightness;

const int QUAL = 64;

void main() {
	vec4 color = vec4(0,0,0,0);
	float rad = radius * 0.4;
	vec2 p = v_tex - dir * rad;
	float w = 0.0;
	for( int i = 0; i <= QUAL; i++ ) {
		float w0 = float(QUAL)/2.0 - abs(float(i) - (float(QUAL) / 2.0));
		color += texture2D( tex, p ) * w0; 
		w += w0;
		p += dir * rad * 2.0 / float(QUAL);
	} 
	color /= w;

	gl_FragColor = color * brightness;
}

