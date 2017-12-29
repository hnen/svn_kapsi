
cbuffer Constants {
};

Texture2D tex_diffuse;
Texture2D tex_normalmap;

cbuffer InstanceData {
	float4x4 v;
	float4x4 wv;
	float4x4 wvp;
};


struct VS_OUTPUT {
    float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	//float4 tangent : TANGNENT;
	//float2 uv : TEXCOORD0;
};

SamplerState linearSampler {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

VS_OUTPUT VS(
		uint id : SV_InstanceID
		,float4 vPos : POSITION
		,float4 vNormal : NORMAL
		//,float4 vTangent: TANGENT 
		) {
    VS_OUTPUT Out;
	float3 pos = vPos.xyz;
	Out.Position = mul( float4( pos, 1.0f ), wvp );
	float3 normal = vNormal.xyz;
	Out.Normal = mul( float4(normal.xyz,0), wv ).xyz;
	//Out.tangent = float4( mul( float4(vTangent.xyz,0), wv ).xyz, vTangent.w );
	//Out.uv = vUv;
    return Out;
}

float4 PS( VS_OUTPUT In ) : SV_Target {
	//float3 tan0 = normalize( In.tangent.xyz );
	//float3 tan1 = -normalize( cross( In.Normal, tan0 ) * In.tangent.w );

	//float3 ntex = tex_normalmap.Sample( linearSampler, In.uv ).rgb;
	//float3 nmap = float3( (ntex.rg - 0.5f) * 2.0f, ntex.b );

	//float3 norm = normalize( (tan0 * nmap.x + tan1 * nmap.y) * 1.0f + In.Normal * nmap.z );
	float3 norm = In.Normal;

	float n = dot( normalize( mul( float4(1,1,1,0), v ).xyz ), norm );
	//return float4( tex_diffuse.Sample( linearSampler, In.uv ).rgb * (n*0.9f + 0.1f), 1 );
	return float4( float3(1,1,1) * (saturate(n) + 0.25f), 1 );
}

technique10 Poop {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
