precision mediump float;

varying mediump vec4 color;
varying mediump vec3 pos;

//uniform sampler2D tex0;

void main() {
		float vol = pow( sin(abs(dot(pos,vec3(0.0,0.0,1.0)))*355.0), 15.0 );
		vol = clamp( vol, 0.0, 1.0 );
		gl_FragColor = vec4( 1.0*vol, 1.0*vol, 0.0, 0.6);
//	gl_FragColor = color;
}

