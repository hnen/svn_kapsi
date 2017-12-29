
static const float2 points[4] = { float2(-1, -1), float2(-1, 1), float2(1, -1), float2(1, 1) };

float4 VS( uint i : SV_VertexID ) : SV_Position {
	return float4( points[i] * 0.5f, 0.5f, 1.0f );
}

float4 PS() : SV_Target0 {
	return float4(1,1,1,1);
}

technique10 {
	pass P0 {
		SetPixelShader( CompileShader( ps_5_0, PS() ) );
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
	}
}
