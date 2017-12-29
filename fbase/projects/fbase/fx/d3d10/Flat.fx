
Texture2D diffuse;
SamplerState linearSam {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_Input {
	float4 pos 	: POSITION;
	float2 tx0  : TEXCOORD0;
};

struct VS_Output {
	float4 pos	: SV_POSITION;
	float2 tx0  : TEXCOORD0;
};

struct PS_Output {
	float4 col	: SV_Target;
};

VS_Output VS( VS_Input vin ) {
	VS_Output vout; 
	vout.pos = vin.pos;
	vout.tx0 = vin.tx0;
	return vout;
}


PS_Output PS( VS_Output vin ) {
	PS_Output pout;
	pout.col = diffuse.Sample( linearSam, vin.tx0 );
	pout.col.rgb *= pout.col.a;
	return pout; 
}

technique10 Flat { 
	pass P0 {
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}

