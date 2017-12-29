
BlendState noblend {
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = FALSE;
};

cbuffer Constants {
};

Texture2D tex_back;
Texture2D tex_front;
Texture2D tex_test;
	float dir;		// 1 or -1

cbuffer InstanceData {
	float4x4 v;
	float4x4 light;
	float3 lightPos;
	float4x4 wv;
	float4x4 wvp;
	float4x4 p;
};


struct VSDUAL_OUTPUT {
    float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float3 zd: TEXCOORD0;
	//float4 tangent : TANGNENT;
	//float2 uv : TEXCOORD0;
};

SamplerState Sampler {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

VSDUAL_OUTPUT VSDual( float4 vPos : POSITION, float4 vNormal : NORMAL )
{
    VSDUAL_OUTPUT Out;

	float4 pos = mul( vPos, light );
	float l = length( pos.xyz );
//	pos /= pos.w;
	pos /= l;

	pos.xy = pos.xy / (pos.z+1);
	pos.z *= dir;

//	Out.Position = mul( vPos, wvp );

	Out.Position.xy = pos.xy / (pos.z+1);
	Out.Position.z = (l-0.01f) / (1000.0f-0.01f);
//	Out.Position.z = pos.z;
	Out.Position.w = 1.0f;

//	pos.z = Out.Position.z;
//	pos.z = (l-0.01f) / (10.0f-0.01f);
	pos.w = 1.0f;


//	Out.zd.y = Out.Position.z;
	Out.zd = pos.xyz;
//	Out.zd = Out.Position.xyz;

	float3 normal = vNormal.xyz;
	Out.Normal = mul( float4(normal.xyz,0), wv ).xyz;
    return Out;
}

struct VS_OUTPUT {
    float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float3 Light : TEXCOORD0;
	float3 PosWorld : TEXCOORD1;
	//float4 tangent : TANGNENT;
	//float2 uv : TEXCOORD0;
};

float4 mapDepthToARGB32(const float value)
{
    const float4 bitSh = float4(256.0 * 256.0 * 256.0, 256.0 * 256.0, 256.0, 1.0);
    const float4 mask = float4(0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);
    float4 res = frac(value * bitSh);
    res -= res.xxyz * mask;
    return res;
}

float4 PSDual( VSDUAL_OUTPUT In ) : SV_Target {

	//	n *= 35.0f;
//	n *= n;
//	clip( 1.0f-length(In.zd.xy));
	float z = In.zd.z * dir;
	z = 1.0f/ z;
//	z *= 35.0f;
//	z *= z;
//	clip( In.zd.x );
	return float4( In.zd.x, In.zd.y, z, 1 );
//	return (In.zd.x + In.zd.y) * 35.0f;
}



VS_OUTPUT VS( float4 vPos : POSITION, float4 vNormal : NORMAL ) 
{
    VS_OUTPUT Out;
	float3 pos = vPos.xyz;
	Out.Position = mul( float4( pos, 1.0f ), wvp );

	float3 normal = vNormal.xyz;
	Out.Normal = mul( float4(normal.xyz,0), wv ).xyz;

	Out.PosWorld = pos.xyz;
	Out.Light = normalize( pos.xyz - lightPos );
	return Out;
}


float4 PS( VS_OUTPUT In ) : SV_Target {

	float  g = 1.0f / In.Position.z;

	float3 posl = mul( float4( In.PosWorld, 1 ), light ).xyz;
	float l = length( posl );
	posl /= l;

	float z;

	float dm = 0;

		

//	if ( In.PosWorld.z-lightPos.z < 0.0f )
	if ( posl.z > 0 )
	{
		float2 uv;
/*posl.z = posl.z + 1;
posl.x = posl.x / posl.z;
posl.y = posl.y / posl.z;
posl.z = l;

uv.x = .5f * posl.x + .5f;
uv.y = -.5f * posl.y + .5f;
*/
		uv.x = posl.x / (1.0f + posl.z) * 0.5f + 0.5f;
		uv.y = 1.0f - posl.y / (1.0f + posl.z) * 0.5f + 0.5f;
		z = tex_front.SampleLevel( Sampler, uv, 0 ).z;

	}
	else
	{
		float2 uv;
/*posl.z = 1-posl.z;
posl.x = posl.x / posl.z;
posl.y = posl.y / posl.z;
posl.z = l;

uv.x = .5f * posl.x + .5f;
uv.y = -.5f * posl.y + .5f;*/
		uv.x = posl.x / (1.0f - posl.z) * 0.5f + 0.5f;
		uv.y = 1.0f - posl.y / (1.0f - posl.z) * 0.5f + 0.5f;

		dm=0.2f;
		z = tex_back.SampleLevel( Sampler, uv, 0 ).z;
	}


	float zs = (l - 0.01f) / (1000.0f - 0.01f );


	float sl = 1 / length( In.PosWorld.xyz - lightPos.xyz );

//	sl = (0.5f+sl);
//	sl *= sl;

//	if ( z + 0.005f < sl )
//		return float4(0,0,1,1);


/*	if ( z + 0.0006f < sl ) 
	{
		return float4(sl,dm,0,1);
	}*/

	float3 norm = normalize( In.Normal );
//	float n = 1-(1+dot( In.Light, norm ))/2.0f;
	float n = 1-(1+dot( lightPos, norm ))/2.0f;

	n = 0.8f + n * 0.2f;
/*	float nn = clamp(abs(1.0f-sl),0,1);
	nn *= nn;
	n += nn * 0.1f;
*/

	float smm = sl - z + 0.005f;
//	if ( smm < -0.005f )
	if ( z+0.005f > zs )
		n *= 0.1f;

	return float4( float3(1,1,1)*n, 1 );

}

technique10 Obj {
    pass P0 {
		SetBlendState( noblend, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

technique10 Dual {
    pass P0 {
		SetBlendState( noblend, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VSDual() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSDual() ) );
    }
}

