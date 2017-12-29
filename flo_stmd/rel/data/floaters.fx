
#define LOW_QUALITY

Texture2D tex_field;
Texture2D tex_intensity;
Texture2D tex_intensityPixel;
Texture2D tex_noise;
Texture2D tex_floater;

float g_intensity;

static float2 quad[4] = { 
	float2(0,0), float2(0,1), float2(1,0), float2(1,1)
};

cbuffer InstanceData {
	float t;
}

struct VS_OUTPUT {
	uint id : VID;
};

struct GS_OUTPUT {
    float4 Position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float2 uvScr : TEXCOORD1;
	float a : ALPHA;
};

SamplerState Trilinear {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

BlendState transparent {
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};


VS_OUTPUT VS( uint id : SV_VertexID ) {
	VS_OUTPUT Out;
	//Out.Position = float4( 0, 0, 0, 1 );
	//Out.uv = float2( 0, 0 );
	Out.id = id;
	return Out;
}

float noise( int seed ) {
	//uint w, h, tmp;
	//tex_noise.GetDimensions( 0, w, h, tmp );
	uint w = 1024;
	uint h = 1024;

	uint x = seed%w;
	uint y = (seed/w)%h;

	return tex_noise.Load( int3(x,y,0) ).x;
}


float2 field( float2 pos ) {
	pos = (pos + 1.0f) * 0.5f;
	pos.y = 1.0f - pos.y;
	return tex_field.SampleLevel( Trilinear, pos, 0 ).xy;
}


float f( float x, float A, float h ) {
	return 4.0f * h * x * ( 1.0f - x/A );
}

[maxvertexcount(16)]
void GS( point VS_OUTPUT In[1],  inout TriangleStream<GS_OUTPUT> outStream ) {
	GS_OUTPUT Out;

	uint id = In[0].id;

	float dur = 6.0f;
	float len = 2.0f;

	float it = fmod(t + noise(id + 2000) * dur, dur+len);
	//float it = fmod(t, dur);
	//float it = len;

#ifdef LOW_QUALITY
	float dt = 0.1f;
#else
	float dt = 0.05f;
#endif

	float v = 4.0f * (1+noise(id+5000)) * 0.02f * dt;

	float W = 0.01f;

	float tail = max(0,it - len);
	float head = min(it,dur);


	float x = 0;
	float2 pos = (float2( noise(id), noise(id+1000) ) - 0.5f) * 2.0f;
	Out.a = tex_intensity.SampleLevel( Trilinear, (pos * float2(1,-1) + 1.0f) * 0.5f, 0 ).x;
	float2 prevp = pos;
	uint n = 0;
	while ( x < head ) {
		n++;
		float2 vt = field(pos);
		vt = float2(vt.y,-vt.x);
		prevp = pos - vt*v;

		float2 d2 = normalize(float2(vt.y,-vt.x));

		if ( x+dt > tail ) {
			if ( x < tail ) {
				float2 npos = pos + vt*v * fmod(tail,dt)/dt;
				Out.Position = float4( npos+d2*W, 0, 1 );
				Out.uv = float2(1,0);
				outStream.Append( Out );
				Out.Position = float4( npos-d2*W, 0, 1 );
				Out.uvScr = (Out.Position.xy * float2(1,-1) + 1.0f) * 0.5f;
				Out.uv = float2(0,0);
				outStream.Append( Out );
			} else {
#ifdef LOW_QUALITY
				if ( (n%8) == 0 ) {
#else
				if ( (n%2) == 0 ) {
#endif
					float ty = (x-tail)/(head-tail);
					Out.Position = float4( pos+d2*W, 0, 1 );
					Out.uv = float2( 1, ty );
					outStream.Append( Out );
					Out.Position = float4( pos-d2*W, 0, 1 );
					Out.uvScr = (Out.Position.xy * float2(1,-1) + 1.0f) * 0.5f;
					Out.uv = float2( 0, ty );
					outStream.Append( Out );
				}
			}
		}

		pos += vt * v;
		x += dt;
	}
	float2 vt = field(pos);
	vt = float2(vt.y,-vt.x);

	float2 d2 = normalize(float2(vt.y,-vt.x));

	pos += vt * v * (1.0f - (x-head)/dt);

	Out.Position = float4( pos+d2*W, 0, 1 );
	Out.uv = float2( 1, 1 );
	outStream.Append( Out );
	Out.Position = float4( pos-d2*W, 0, 1 );
	Out.uvScr = (Out.Position.xy * float2(1,-1) + 1.0f) * 0.5f;
	Out.uv = float2( 0, 1 );
	outStream.Append( Out );
	
	outStream.RestartStrip();
}

float4 PS( GS_OUTPUT In ) : SV_Target {
	float4 tx = tex_floater.SampleLevel( Trilinear, In.uv, 0 );

	float i = In.a * (1.0f-tex_intensityPixel.SampleLevel( Trilinear, In.uvScr, 0 ).x);
	//return float4( tx.rgb * tx.a, 1 );
	//return float4(1,1,1,0.35);
	return float4( tx.rgb * tx.a * tx.a, tx.a * 0.25f * i * g_intensity ) * float4( 0.2f, 1.0f, 0.9f, 1 );
}

technique10 Floaters {
    pass P0 {
		SetBlendState( transparent, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() )  );
		//SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}


