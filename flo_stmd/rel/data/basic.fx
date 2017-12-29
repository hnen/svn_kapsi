
BlendState noblend {
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = FALSE;
};

cbuffer Constants {
};

Texture2D tex_diffuse;
Texture2D tex_normalmap;

cbuffer InstanceData {
	float4x4 v;
	float4x4 wv;
	float4x4 wvp;
	float4 color;
};


struct VS_OUTPUT {
    float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	//float4 tangent : TANGNENT;
	//float2 uv : TEXCOORD0;
};

struct PS_OUTPUT {
	float4 color : SV_Target0;
	float4 mask : SV_Target1;
};

SamplerState linearSampler {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

VS_OUTPUT VS( float4 vPos : POSITION, float4 vNormal : NORMAL ) {
    VS_OUTPUT Out;
	float3 pos = vPos.xyz;
	Out.Position = mul( float4( pos, 1.0f ), wvp );

	float3 normal = vNormal.xyz;
	Out.Normal = mul( float4(normal.xyz,0), wv ).xyz;
    return Out;
}

PS_OUTPUT PS( VS_OUTPUT In ) {
	PS_OUTPUT Out;

	float3 norm = normalize( In.Normal + float3(0,0,1) * 0.5f );
	float n = dot( normalize( float4(0,0,1,0).xyz ), norm );
	Out.color = float4( float3(1,1,1) * (saturate(n)), 1 );
	Out.mask = float4(1,1,1,1);

	return Out;
}

float4 PS_Emissive( VS_OUTPUT In ) : SV_Target0 {
	return color;
}
float4 PS_Blank( VS_OUTPUT In ) : SV_Target0 {
	return float4(0,0,0,0);
}


technique10 Alus {
    pass P0 {
		SetBlendState( noblend, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}


technique10 Sun {
    pass P0 {
		SetBlendState( noblend, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Emissive() ) );
    }
}

technique10 Subtract {
    pass P0 {
		SetBlendState( noblend, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Blank() ) );
    }
}
