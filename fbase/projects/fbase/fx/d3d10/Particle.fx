
#include "Common.fxh"

struct GS_Output {
	float4 pos 	: SV_POSITION;
	float2 tx0  : TEXCOORD0;
};

struct VS_Input {
	float3 pos 	: POSITION;
};

struct VS_Output {
	float4 pos	: SV_POSITION;
	float viewZ : TEXCOORD0;
};

struct PS_Output {
	float4 col	: SV_Target;
};

VS_Output VS( VS_Input vin ) {
	VS_Output vout; 
	vout.pos = mul( float4( vin.pos.xyz, 1 ), worldViewProj );
	vout.viewZ = dot( float4( vin.pos.xyz, 1 ), worldView._13_23_33_43 );
	return vout;
}

[maxvertexcount(6)]
void GS( point VS_Output input[1], inout TriangleStream<GS_Output> TriStream ) {
	GS_Output i;
	float4 pos = input[0].pos;
	float4 viewZ = input[0].viewZ;
	
	float size = 0.3f;

	float2 d = float2( proj._11, proj._22) * size / (proj._34 * viewZ);
	float4 p0 = pos + float4(-d.x,-d.y, 0, 0 );
	float4 p1 = pos + float4( d.x,-d.y, 0, 0 );
	float4 p2 = pos + float4(-d.x, d.y, 0, 0 );
	float4 p3 = pos + float4( d.x, d.y, 0, 0 );
	
	i.pos = p1;
	i.tx0 = float2(1,1);
	TriStream.Append( i );
	i.pos = p0;
	i.tx0 = float2(0,1); 
	TriStream.Append( i );
	i.pos = p2;
	i.tx0 = float2(0,0);
	TriStream.Append( i );

	TriStream.RestartStrip();

	i.pos = p3;
	i.tx0 = float2(1,0);
	TriStream.Append( i );
	i.pos = p1;
	i.tx0 = float2(1,1);
	TriStream.Append( i );
	i.pos = p2;
	i.tx0 = float2(0,0);
	TriStream.Append( i );

	TriStream.RestartStrip();
}


PS_Output PS( GS_Output vin ) {
	PS_Output pout;
	pout.col = diffuse.Sample( sampler_linear, vin.tx0 );
	pout.col.rgb *= pout.col.a;
	pout.col.a *= 1.2f;
	return pout;
}


technique10 Particle {
	pass P0 {
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( CompileShader( gs_4_0, GS() ) );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
		SetBlendState( blend_alpha, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		//SetDepthStencilState( depth_disable, 0 );
	}
}

