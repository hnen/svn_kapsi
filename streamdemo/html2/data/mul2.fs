precision mediump float;

varying highp vec2 v_tex;
uniform sampler2D tex;
uniform sampler2D tex2;
uniform float t;

void main() {
	float val0 = texture2D(tex, v_tex+vec2(1.0,0.5)*t*0.01).a;
	float val1 = texture2D(tex2, v_tex+vec2(-1.0,0.5)*t*0.01).a;
	float val = val0*val1;
	gl_FragColor = vec4(vec3(1,1,1)*val,1);
}

