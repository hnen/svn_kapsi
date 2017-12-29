
//#define BLUR_SAMPLES 75
#define BLUR_SAMPLES 35
#define BLUR_RADIUS 0.035f

Texture2D tex_input;
static float2 quad[4] = { 
	float2(-1,-1), float2(-1,1), float2(1,-1), float2(1,1)
};

//float radius;

struct VS_OUTPUT {
    float4 Position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

SamplerState Trilinear {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

BlendState opaq {
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = FALSE;
};


VS_OUTPUT VS( uint id : SV_VertexID ) {
	VS_OUTPUT Out;
	Out.Position = float4( quad[id], 0, 1 );
	Out.uv = (float2( quad[id].x, - quad[id].y ) + float2(1,1)) * 0.5f;
	return Out;
}

float G(float x, float r3) {
	return exp(-(x*x)/(2*r3*r3));
}



float4 PS( VS_OUTPUT In, uniform float2 dir, uniform uint samples, uniform float radius, uniform bool weightIntensity, uniform float darkWeight, uniform float lightWeight, uniform float mul ) : SV_Target {

	float4 sum = float4(0,0,0,0);
	float s = 0;
	
	//float4 midsample = tex_input.Sample( Trilinear, In.uv );
	//float br = dot(midsample.xyz, float3(1,1,1) );

	//[unroll]
	for ( uint i = 0; i < samples; i++ ) {
		float x = ((((float)i)/(samples-1))-0.5f) * radius * 2.0f;
		float2 p = In.uv + dir * x; 
		float4 sample = tex_input.Sample( Trilinear, p );
		float br = 1;
		if ( weightIntensity ) {
			br = dot( sample.xyz, float3(1,1,1) ) / 3.0f;
			br = lerp( darkWeight,lightWeight, br );
		}
		float g = G(x, radius/3) * br;
		s += g;
		sum += sample * g;
	}

	return mul * sum / s;
}


technique10 Normal_Horiz {
    pass P0 {
		SetBlendState( opaq, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS( float2(1,0), 4*BLUR_SAMPLES, 1.0f * 4.5 * BLUR_RADIUS,						false, 0.35f, 1.0f, 3.25f ) ) );
    }
}

technique10 Normal_Vert {
    pass P0 {
		SetBlendState( opaq, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS( float2(0,1), 4*BLUR_SAMPLES, 1.0 * 4.5 * BLUR_RADIUS * (1280.0f / 720),	false, 0.35f, 1.0f, 3.25f ) ) );
    }
}


technique10 Shrink_Horiz {
    pass P0 {
		SetBlendState( opaq, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS( float2(1,0), BLUR_SAMPLES, 0.5f * BLUR_RADIUS, false, 1.0f, 0.05f, 1.0f ) ) );
    }
}

technique10 Shrink_Vert {
    pass P0 {
		SetBlendState( opaq, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS( float2(0,1), BLUR_SAMPLES, 0.5f * BLUR_RADIUS * (1280.0f / 720), false, 1.0f, 0.05f, 1.0f ) ) );
    }
}


technique10 Expand_Horiz {
    pass P0 {
		SetBlendState( opaq, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS( float2(1,0), BLUR_SAMPLES, BLUR_RADIUS, true, 0.05f, 1.0f, 1.0f ) ) );
    }
}

technique10 Expand_Vert {
    pass P0 {
		SetBlendState( opaq, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS( float2(0,1), BLUR_SAMPLES, BLUR_RADIUS * (1280.0f / 720), true, 0.05f, 1.0f, 1.0f ) ) );
    }
}

technique10 ExpandMore_Horiz {
    pass P0 {
		SetBlendState( opaq, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS( float2(1,0), BLUR_SAMPLES, 3.0f * BLUR_RADIUS, true, 0.05f, 2.0f, 1.0f ) ) );
    }
}

technique10 ExpandMore_Vert {
    pass P0 {
		SetBlendState( opaq, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS( float2(0,1), BLUR_SAMPLES, 3.0f * BLUR_RADIUS * (1280.0f / 720), true, 0.05f, 2.0f, 1.0f ) ) );
    }
}
