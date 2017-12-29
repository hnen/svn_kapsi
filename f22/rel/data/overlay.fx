

Texture2D tex_image;
static float2 quad[4] = { 
//	float2(1,1), float2(0,1), float2(1,0), float2(0,0)
//	float2(0,0), float2(1,0), float2(0,1),	float2(1,1),
	float2(0,0), float2(0,1), float2(1,0), float2(1,1)
};

cbuffer InstanceData {
	float4 posSize;
}

struct VS_OUTPUT {
    float4 Position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

SamplerState Trilinear {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

BlendState transparent {
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};


VS_OUTPUT VS( uint id : SV_VertexID ) {
	VS_OUTPUT Out;
	Out.Position = float4( posSize.xy + quad[id] * posSize.zw, 0, 1 );
	Out.uv = float2( quad[id].x, 1.0f - quad[id].y );
	return Out;
}

float4 PS( VS_OUTPUT In ) : SV_Target {
	return tex_image.Sample( Trilinear, In.uv ) * 0.5f;
	//return float4(1,1,1,1);
}

technique10 Transparent {
    pass P0 {
		SetBlendState( transparent, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}


