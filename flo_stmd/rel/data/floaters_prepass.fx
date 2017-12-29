
Texture2D tex_field_perlin;
Texture2D tex_field_geom;

Texture2D tex_vectors_geom;

Texture2D tex_intensity_base;
Texture2D tex_intensity_mul;

static float2 quad[4] = { 
	float2(-1,-1), float2(-1,1), float2(1,-1), float2(1,1)
};

float t;

struct VS_OUTPUT {
    float4 Position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

SamplerState Trilinear {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

BlendState opaq {
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = FALSE;
};


VS_OUTPUT VS( uint id : SV_VertexID ) {
	VS_OUTPUT Out;
	Out.Position = float4( quad[id], 0, 1 );
	Out.uv = (float2( quad[id].x, -quad[id].y ) + float2(1,1)) * 0.5f;
	return Out;
}

float PS_field( VS_OUTPUT In ) : SV_Target {
	float f0 = tex_field_perlin.Sample( Trilinear, In.uv * 0.5f + float2(-1,0) * t * 0.005f ).x;
	float f1 = tex_field_perlin.Sample( Trilinear, In.uv + float2(0,1) * t * 0.01f ).x;
	float g0 = tex_field_geom.Sample( Trilinear, In.uv ).x;
	//return lerp( f0 * f1, g0, saturate( 0.75f * pow(abs(g0), 1.5 ) ) );
	return f0 * f1 * (1.0f-g0);
}

float2 PS_vectors( VS_OUTPUT In ) : SV_Target {
	float s1 = tex_vectors_geom.Sample( Trilinear, In.uv + float2(1,0) * (1.0f / 1280.0f) ).x;
	float s2 = tex_vectors_geom.Sample( Trilinear, In.uv + float2(0,1) * (1.0f / 720.0f) ).x;
	float s3 = tex_vectors_geom.Sample( Trilinear, In.uv - float2(1,0) * (1.0f / 1280.0f) ).x;
	float s4 = tex_vectors_geom.Sample( Trilinear, In.uv - float2(0,1) * (1.0f / 720.0f) ).x;
	float2 v = float2( s1-s3, s2-s4 ) * 100.0f;
	//return v;
	return float2( v.x, -v.y );
}

float PS_intensity( VS_OUTPUT In ) : SV_Target {
	return tex_intensity_base.Sample( Trilinear, In.uv ).x * (1.0f - tex_intensity_mul.Sample( Trilinear, In.uv ).x * 2.0f);
}


technique10 Field {
    pass P0 {
		SetBlendState( opaq, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_field() ) );
    }
}

technique10 Vectors {
    pass P0 {
		SetBlendState( opaq, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_vectors(  ) ) );
    }
}

technique10 Intensity {
    pass P0 {
		SetBlendState( opaq, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_intensity(  ) ) );
    }
}



