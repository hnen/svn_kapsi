precision mediump float;

varying highp vec2 v_tex;
uniform sampler2D tex;

uniform vec2 fb_size;

uniform vec2 tex_pal_size;
uniform sampler2D tex_pal;

uniform vec2 tex_vign_size;
uniform sampler2D tex_vign;

uniform float edgeradius;
uniform float edgestrength;
uniform float bgstrength;
uniform float edgethreshold;
uniform float bgpow;
uniform float colorscale;

float edge( float rad, vec2 _tex ) {
	vec4 color1 = texture2D( tex, _tex - vec2(1,0) * rad );
	vec4 color2 = texture2D( tex, _tex + vec2(1,0) * rad );
	vec4 color3 = texture2D( tex, _tex - vec2(0,1) * rad );
	vec4 color4 = texture2D( tex, _tex + vec2(0,1) * rad );

	vec2 d = vec2( (color2.r - color1.r) / 0.001,(color4.r - color3.r) / 0.001);
	return length(d);
}

void main() {
	vec4 color = texture2D( tex, v_tex );
	vec4 ocol = color;


	float grad = edge(edgeradius,v_tex);

	float col = (grad > edgethreshold ? bgstrength : edgestrength) * pow(color.r,bgpow) * colorscale;
	//float col = grad * edgestrength * pow(color.r*3.5,bgpow*5.0) * colorscale;

	//color.r = pow( color.r, 25.0 ) * 20.0;
	//color.r = (grad > 2.0 ? 3.0 : 0.0);
	//color.r = pow( c0.r, 5.0 ) * 20.0;
	color.r = col;
	color.g = col;
	color.b = col;

	vec4 pcol = texture2D( tex_pal, vec2( tex_pal_size.x * clamp(pow(col,0.3)*1.0,0.005,0.995), 0.01 ) );
	vec4 vign = texture2D( tex_vign, v_tex * tex_vign_size / fb_size );

	gl_FragColor = pcol * (1.0 - vign.a*0.8);
	//gl_FragColor = ocol;
}

