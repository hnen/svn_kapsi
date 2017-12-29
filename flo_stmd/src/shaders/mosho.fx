
BlendState noblend {
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = FALSE;
};
BlendState add {
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = ONE;
};

cbuffer Constants {
	float4 posSize;
	float mur;
};


cbuffer InstanceData {
	float4x4 v;
	float4x4 wv;
	float4x4 wvp;
	float4x4 p;
};

struct VS_OUTPUT {
    float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	//float3 Light : TEXCOORD0;
	float3 PosWorld : TEXCOORD1;
	//float4 tangent : TANGNENT;
	//float2 uv : TEXCOORD0;
};



SamplerState Sampler {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};



VS_OUTPUT VS(
		uint id : SV_InstanceID
		,float4 vPos : POSITION
		,float4 vNormal : NORMAL
		//,float4 vTangent: TANGENT 
		) 
{
    VS_OUTPUT Out;
	float mu = sin(mur * 3);
	float le = (1-mu) + length(vPos.xyz) / 2.5f * mu;
	float4 p_ = float4( vPos.xyz / le, 1 ) ;//* (1+mu);
	float4 pos = mul( p_, wv );
	Out.Position = mul( pos, p );
//	Out.Normal = ( mul( float4(vNormal.xyz,0), wv ).xyz );
//	Out.Normal = mul( vNormal, wvp ).xyz;
	Out.Normal = vNormal.xyz;
	Out.PosWorld = Out.Position.xyz;
	return Out;
}

float4 PS( VS_OUTPUT In ) : SV_Target {
	float x = 0.5f + 0.5f * In.Normal.x;
	float y = 0.5f + 0.5f * In.Normal.y;
	float z = 1.0f / In.PosWorld.z;
	return float4( x,y,z,1.0f );
}



technique10 Liima {
    pass P0 {
		SetBlendState( noblend, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
