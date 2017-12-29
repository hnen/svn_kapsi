
struct VS_OUTPUT {
    float4 Position : SV_POSITION;
	float2 txCoord : TEXCOORD;
};

static float4 verts[4] = { float4(-1,-1, 0, 1), float4(-1, 1, 0, 1 ), float4( 1,-1, 0, 1 ), float4( 1, 1, 0, 1 ) };
static float2 txs[4] = { float2(0,1), float2(0,0), float2(1,1), float2(1,0) };

Texture2D tex;

SamplerState linearSampler {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

VS_OUTPUT VS( uint id : SV_VertexID ) {
    VS_OUTPUT Out;
	Out.Position = verts[id];
	Out.txCoord = txs[id];
    return Out;
}

float4 PS( VS_OUTPUT In ) : SV_Target {
	return tex.Sample( linearSampler, In.txCoord );
}

float intensity;

float4 PS0( VS_OUTPUT In ) : SV_Target {
	
	float3 s0 = tex.Sample( linearSampler, In.txCoord + float2(-1,0) * 0.03f * intensity * 0.1f ).rgb;
	float3 s1 = tex.Sample( linearSampler, In.txCoord + float2( 1,0) * 0.03f * intensity * 0.1f ).rgb;
	float3 s2 = tex.Sample( linearSampler, In.txCoord + float2( 0,-1) * 0.06f * intensity * 0.1f ).rgb;
	float3 s3 = tex.Sample( linearSampler, In.txCoord + float2( 0, 1) * 0.06f * intensity * 0.1f ).rgb;
	float l0 = dot( s0.rgb, float3(0.30, 0.59, 0.11) );
	float l1 = dot( s1.rgb, float3(0.30, 0.59, 0.11) );
	float l2 = dot( s2.rgb, float3(0.30, 0.59, 0.11) );
	float l3 = dot( s3.rgb, float3(0.30, 0.59, 0.11) );
	float diff0 = abs(l0-l1);
	float diff1 = abs(l2-l3);
	float d = diff0 > 0.05f || diff1 > 0.05;

	float3 infs = min(max(s0,s1),max(s2,s3));
	//infs = pow( infs, 5 ) * 2.0f;

	return float4(infs,1);
}

technique10 Fill {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

technique10 Effect {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS0() ) );
    }
}
