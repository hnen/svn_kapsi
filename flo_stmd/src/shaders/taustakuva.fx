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

	float2 mouse = float2(0,0);

//		mouse.x = 100.5f + 100.5f * sin(time*1.31f);
//		mouse.y = 100.5f + 100.5f * cos(time);
		mouse.x = 2.5f + 2.5f * cos(time*1.31f);
		mouse.y = 7.5f + 7.5f * sin(time +0.2f*sin(time*0.521f) );

	float2 position = vert.uv.xy - mouse.xy;

	float px_ = position.x;
	float py_ = position.y;
	
	float a = time / 100.0 + mouse.x;
	
	float mx = (1+mouse.x)*3;
	float my = (1+mouse.y)*3;
	float px = cos(px_*my+a)*sin(py_*mx+a);
	float py = sin(px_*mx+a)*cos(py_*my+a);
	
	float color = cos(px*(mouse.x) / 100.0 );
	color *= sin( py * cos(py*1000.0) * 100.0 * mouse.y );
	
	float3 tmm = float3( sin(color), sin(color), sin(color + mouse.y*0.1*cos(time*10.0) ) );
	
//	float4 gl_FragColor = float4( tmm, tmm.z + tmm.x );
float4 gl_FragColor = tex.Sample( Trilinear, tmm.yz  );
gl_FragColor.a = tmm.x* volume;
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

