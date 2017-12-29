#ifdef GL_ES
precision mediump float;
#endif


float volume;
float time;
Texture2D tex;


static float2 quad[4] = { 
	float2(-1,-1), float2(-1,1), float2(1,-1), float2(1,1)
};



BlendState transparent {
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = 0;
};

BlendState noblend {
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = FALSE;
};

struct VS_OUTPUT {
    float4 Position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

SamplerState Trilinear {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};


VS_OUTPUT VS( uint id : SV_VertexID ) {
	VS_OUTPUT Out;
	Out.Position = float4( quad[id], 0, 1 );
	Out.uv = (float2( quad[id].x, - quad[id].y ) + float2(1,1)) * 0.5f;
	return Out;
}


float4 PS( VS_OUTPUT vert ) : SV_Target
{

	float4 gl_FragColor = tex.Sample( Trilinear, vert.uv);
	gl_FragColor.a = volume;
	return gl_FragColor;

}


technique10 Taustakuva {
    pass P0 {
		SetBlendState( transparent, float4(1,1,1,1), 0xffffffff );
		//SetBlendState( noblend, float4(1,1,1,1), 0xffffffff );
        SetDepthStencilState( DisableDepth, 0 );
        SetGeometryShader( NULL );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

