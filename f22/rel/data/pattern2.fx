
Texture2D tex_image;
static float2 quad[4] = { 
	float2(-1,-1), float2(-1,1), float2(1,-1), float2(1,1)
};

cbuffer InstanceData {
	float4 prms;
	float aspect;
	float t;
	float3 camDir;
}

struct VS_OUTPUT {
    float4 Position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

SamplerState Trilinear {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

BlendState transparent {
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};


VS_OUTPUT VS( uint id : SV_VertexID ) {
	VS_OUTPUT Out;
	Out.Position = float4( quad[id], 0.9999f, 1 );
	Out.uv = quad[id] * float2(aspect,1);
	return Out;
}

int f_i( float r ) {
	// i = sqrt(6r)
	return floor( pow(r * prms[1], 1.0f/prms[2] ) );
}
float f_r( int i ) {
	// i = sqrt(6r)
	// i^2 = 6r
	// r = (1/6)i^2
	return pow( ((float)i), prms[2] ) / prms[1];
}

float4 PS( VS_OUTPUT In ) : SV_Target {

	float2 uv = In.uv;

	uv += -camDir.xy/(0.01f+abs(camDir.z));

	float r = length(uv) + prms[0];
	float a = atan2( uv.x, uv.y ) + t * 0.5f;

	a = prms[3] * a / 6.28f;

	int i = f_i(r);

	float x0 = f_r(i);
	float x1 = f_r(i+1);
	float x = 1.0f - (r-x0)/(x1-x0);


	//return float4(1,1,1,1);
	return tex_image.Sample( Trilinear, float2(a,x) ) * 1.0f;
}

technique10 Pattern {
    pass P0 {
		SetBlendState( transparent, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}


