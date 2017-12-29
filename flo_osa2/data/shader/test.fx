
cbuffer Constants {
	float4x4 vp;
	float4x4 v;
	float lodZ;
	float boundingSphere;
};

Texture2D tex_diffuse;
Texture2D tex_normalmap;

cbuffer InstanceData {
	float4x4 w[1024];
};


struct VS_OUTPUT {
    float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float4 tangent : TANGNENT;
	float2 uv : TEXCOORD0;
};

SamplerState linearSampler {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

VS_OUTPUT VS(
		uint id : SV_InstanceID,
		float4 vPos : POSITION,
		float4 vNormal : NORMAL,
		float4 vTangent: TANGENT,
		float2 vUv : TEXCOORD0,
		float3 lodPos : LODPOS,
		float3 lodNormal : LODNORMAL ) {
    VS_OUTPUT Out;
	uint _id = id;

	float4x4 wv = mul(w[_id], v);
	float z = mul( vPos, wv ).z;
	float t_z = lodZ - lodZ/1.5f + boundingSphere;
	float a = boundingSphere - t_z;
	float b = (boundingSphere - lodZ) / (boundingSphere - t_z);
	float lodT = saturate( -z/a + b );

	float3 pos = lerp( lodPos, vPos.xyz, lodT );
	//float3 pos = vPos.xyz;

	Out.Position = mul( float4( pos, 1.0f ), mul(w[_id], vp) );

	float3 normal = lerp( lodNormal, vNormal.xyz, lodT );
	//float4x4 wv = v;
	Out.Normal = mul( float4(normal.xyz,0), wv ).xyz;
	Out.tangent = float4( mul( float4(vTangent.xyz,0), wv ).xyz, vTangent.w );
	Out.uv = vUv;
    return Out;
}

float4 PS( VS_OUTPUT In ) : SV_Target {
	float3 tan0 = normalize( In.tangent.xyz );
	float3 tan1 = -normalize( cross( In.Normal, tan0 ) * In.tangent.w );

	//return float4( ( tan0 + float3(1,1,1) ) * 0.5f, 1 );

	In.uv *= 8.0f;

	float3 ntex = tex_normalmap.Sample( linearSampler, In.uv ).rgb;
	float3 nmap = float3( (ntex.rg - 0.5f) * 2.0f, ntex.b );

	float3 norm = normalize( (tan0 * nmap.x + tan1 * nmap.y) * 1.0f + In.Normal * nmap.z );
	//float3 norm = In.Normal;

	float n = dot( float3(1,0,0), norm );
	//return float4( tex_normalmap.Sample( linearSampler, In.uv ).rgb, 1 );
	return float4( tex_diffuse.Sample( linearSampler, In.uv ).rgb * (n + 0.1f), 1 );
	//return float4( float3(1,1,1) * (n + 0.1f), 1 );
	//return float4( tex_diffuse.Sample( linearSampler, In.uv ).rgb, 1 );
	//return float4( float3(1,1,1) * n, 1 );
	//return float4( tex_diffuse.Sample( linearSampler, In.uv ).rgb, 1 );
}

technique10 Poop {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
