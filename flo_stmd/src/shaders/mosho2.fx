
Texture2D tex;
Texture2D scene;
Texture2D envmap;

static float2 quad[4] = { 
	float2(0,0), float2(0,1), float2(1,0), float2(1,1)
};

cbuffer InstanceData {
	float4 posSize;
	float amount = 0.1f;		// for mix
}

struct VS_OUTPUT {
    float4 Position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

SamplerState Trilinear {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

BlendState transparent {
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};

BlendState noblend {
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = FALSE;
};


float3 getNormal( float2 n )
{
	float3 n1;
	n1.x = (n.x-0.5f)*2.0f;
	n1.y = (n.y-0.5f)*2.0f;
	n1.z = sqrt(-n1.x*n1.x - n1.y*n1.y + 1 );
	return n1;
//	return normalize(n1);
}


VS_OUTPUT VS( uint id : SV_VertexID ) {
	VS_OUTPUT Out;
	Out.Position = float4( posSize.xy + quad[id] * posSize.zw, 0, 1 );
	Out.uv = float2( quad[id].x, 1.0f - quad[id].y );
	return Out;
}

float3 cosify( float3 c )
{
	float PI = 3.14159265358979323846264;
	float PI2 = 3.14159265358979323846264 / 2.0f;
	c = c * PI;
	return float3( cos(c.x), cos(c.y), cos(c.z) );
	return float3( sin(c.x), sin(c.y), sin(c.z) );
}

float4 PS( VS_OUTPUT In ) : SV_Target {
	float4 p_ = scene.Sample( Trilinear, In.uv ).rgba;

		float3 normal = getNormal( p_.xy );
normal = cosify(normal);

		//	float2 uv = float2( 0.5f + 0.5f * normal.x / p.z, 0.5f + 0.5f * normal.y / p.z );*/
float gk = p_.z;
//	return float4(gk,gk,gk,1); 
//return float4( scene.Sample( Trilinear, In.uv  ).rgb, 1 ) * gk;
	/*
	float2 uvx = float2(0.01f,0.0f);
	float2 uvy = float2(0.0f,0.01f);
	float4 p1 = scene.Sample( Trilinear, In.uv + uvx ).rgba;
	float4 p2 = scene.Sample( Trilinear, In.uv - uvx ).rgba;
	float4 p3 = scene.Sample( Trilinear, In.uv + uvy ).rgba;
	float4 p4 = scene.Sample( Trilinear, In.uv - uvy ).rgba;

	float2 p = float2( p1.z - p2.z, p3.z - p4.z );

	float4 col = envmap.Sample( Trilinear, p.xy ).rgba;*/
	float4 col = envmap.Sample( Trilinear, (normal.xy)  );
//	float4 col = envmap.Sample( Trilinear, p_.xy  );

col = pow(col*2.0f,2.0f);
//col = pow(col*2.0f,2.0f);
//return float4(0,0,0,0);
return col;
		//		return col;
//	float n = dot(  float2(p.x,p.z), float2(p.y,p.z) );
	float n = dot( float3(1,0,-1), normal );
//	return float4(c,c,c,1);
	n *= n;
	col = float4(n,n,n,1);

//	col *= min( (p_.z*p_.z) + 0.5f, 1 );

//	float g = 0.5f;
//	col += float4(g,g,g,1) * n;
//	if ( gk < 0.01f )
//		clip(-1);
//		return float4(0,0,0,0);
//		return float4(1,0,1,0);
	float3 lz = col.rgb;//* gk;
	return float4( lz, 1 ) * gk;
	
}

float4 mixem( float4 p1, float4 p2 )
{
	float3 n1 = getNormal(p1.xy);
	float3 n2 = getNormal(p2.xy);
	n1 = normalize( n1+n2 );
	return float4( n1.x, n1.y, (p1.z+p2.z)/2.0f, 1.0f );
}


//	x/y = normal		z = 1/z
float4 PSMix( VS_OUTPUT In ) : SV_Target {
	float mdel = 0.001f;
//	mdel = amount * 0.01f;
	float2 uvx = float2(mdel,0.0f);
	float2 uvy = float2(0.0f,mdel);
//	float4 p1 = scene.Sample( Trilinear, In.uv ).rgba;
//	return float4( p1.x, p1.y, z, 0.01f );


	float4 p = scene.Sample( Trilinear, In.uv );
	float4 o = tex.Sample( Trilinear, In.uv );
	
	float4 p1 = scene.Sample( Trilinear, In.uv + uvx ).rgba;
	p1 = p1 + scene.Sample( Trilinear, In.uv + uvy ).rgba;
	p1 = p1 + scene.Sample( Trilinear, In.uv - uvx ).rgba;
	p1 = p1 + scene.Sample( Trilinear, In.uv - uvy ).rgba;
	p = (p1 * 0.25f+p*1.5f)*0.4f;
	float4 o1 = tex.Sample( Trilinear, In.uv + uvx ).rgba;
	o1 = o1 + tex.Sample( Trilinear, In.uv + uvy ).rgba;
	o1 = o1 + tex.Sample( Trilinear, In.uv - uvx ).rgba;
	o1 = o1 + tex.Sample( Trilinear, In.uv - uvy ).rgba;
	o = (o1 * 0.25f + o*1.5f)* 0.4f;
	

	float ggg = amount * 0.01f + 0.01f;
	ggg = 0.1f;
	float3 norm = getNormal(p.xy) * ggg + getNormal(o.xy) * (1-ggg);
//	norm = getNormal( p.xy );
	float l = length(norm);

//	norm = normalize(norm);

//	if ( norm.z > 0 ) 
//		return float4(norm.y,0,0,1);


//	norm = abs(normalize( norm ));

	norm.x = 0.5f + norm.x * 0.5f;
	norm.y = 0.5f + norm.y * 0.5f;
	norm.z = p.z * ggg + o.z * (1-ggg);
	norm.z *= 0.99f;
	return float4(norm,1);
//	no.xy = normalize(no.xy);
//	return float4( abs(getNormal( no.xy )), 1 ); 

/*	if ( isnan(o.x) || isnan(o.y) ||isnan(o.z) ||o.z==0)
	{
//		clip(-1);
		return float4(1,1,1,0);
	}
	if ( isnan(p.x) || isnan(p.y) ||isnan(p.z)||p.z==0 )
	{
//		clip(-1);
		return float4(1,1,1,0);
	}*/
	// 0.09
	float t = 0.009f;//.009f;
	float4 g = p * t + o * (1.0f-t);
	// 0.14
		float m_ = 0.14f;
	p.z = max(0,p.z-m_);
	o.z = max(0,o.z-m_);
	g.z = max(0,g.z-m_);
/*	return p;

	if ( p.z < o.z )
		return p;*/
	return g;
//	return float4( g.x, g.y, min(p.z-0.04f,o.z-0.04f), 1.0f);
//	return float4( g.x, g.y, min(p.z-0.04f,o.z-0.04f), 1.0f);


/*
	float2 uvx = float2(0.001f,0.0f);
	float2 uvy = float2(0.0f,0.001f);

float4 p_ = scene.Sample( Trilinear, In.uv ).rgba * 0.5f;

	float3 n1;
	n1.x = (p1.x-0.5f)*2.0f;
	n1.y = (p1.y-0.5f)*2.0f;
	n1.z = sqrt(-p1.x*p1.x - p1.y*p1.y + 1 );
	float3 n2;
	n2.x = (p2.x-0.5f)*2.0f;
	n2.y = (p2.y-0.5f)*2.0f;
	n2.z = sqrt(-p2.x*p2.x - p2.y*p2.y + 1 );

	float a2 = 1.0f-amount;
	n1 = normalize(n1*amount+n2*a2);
	n1.x = 0.5f + 0.5f * n1.x;
	n1.y = 0.5f + 0.5f * n1.y;
	
	float z = p_.z;

	return float4( n1.x, n1.y, z, 1 );
	*/
}


technique10 Mix {
    pass P0 {
		SetBlendState( noblend, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSMix() ) );
    }
}

technique10 Out {
    pass P0 {
		SetBlendState( noblend, float4(1,1,1,1), 0xffffffff );
//		SetBlendState( transparent, float4(1,1,1,1), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
