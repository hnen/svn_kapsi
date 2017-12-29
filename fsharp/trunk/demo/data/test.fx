
static float2 quad[4] = { float2(-1,-1), float2(-1,1), float2(1,-1), float2(1,1) };

cbuffer VS : register(b0) {
	float vs_aspectRatio : packoffset(c0.x);
}

struct VS_OUT {
	float4 col : COLOR;
	float4 pos : SV_Position;
};

struct PS_IN {
	float4 col : COLOR;
};

//float4 VS( float4 pos : POSITION, uint id : SV_VertexID  ) : SV_Position {
VS_OUT VS( float4 pos : POSITION, uint id : SV_VertexID  )  {
	VS_OUT ret;
	ret.col = pos + float4(0.5,0.5,0.5,0.5);
	ret.pos = float4( pos.xy * float2( vs_aspectRatio, 1 ), 0, 1 );
	return ret;

	//return float4( quad[id%4], 0, 1 );
	//return pos;
}

float4 PS( PS_IN inp ) : SV_Target0 {
	return inp.col;
}

