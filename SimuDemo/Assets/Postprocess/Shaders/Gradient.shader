Shader "f/Postprocess/Gradient" {
Properties {
	_MainTex ("Base (RGB)", 2D) = "white" {}
	_Radius ("Radius", Float) = 0.025
	_Mul ("Mul", Float) = 0.025
}

CGINCLUDE

	#pragma target 3.0

	sampler2D _MainTex;
	float4 _MainTex_ST;
	
	static const int _Q = 80;
	
	float _Radius;
	float _Mul;
	

	struct appdata_t {
		float4 vertex : POSITION;
		float2 texcoord : TEXCOORD0;
	};

	struct v2f {
		float4 vertex : SV_POSITION;
		half2 texcoord : TEXCOORD0;
	};
	
	float F(float x) {
		return (3 - 2*x)*x*x;
	}
	
	float4 blur(sampler2D tex, float2 tx, float2 d, int Q) {
		float4 col = float4(0,0,0,0);
		float w = 0.0f;
		for (int i = 0; i < Q; i++) {
			float x = (float)i / (Q-1);
			float x0 = 2.0f * x;
			float x1 = (1.0f-x + 1.0f) * 0.5f;
			float w0 = x > 0.5f ? F(x1) : F(x0);
			float2 tx0 = tx + d * (-0.5f + ((float)i)/(Q-1));
			col += tex2D(tex, tx0) * w0;
			w += w0;
		}
		
		col /= w;
		return col;
	}
	
ENDCG

SubShader {
	Tags { "RenderType"="Opaque" }

	Pass {
		CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag

			#include "UnityCG.cginc"

			v2f vert (appdata_t v)
			{
				v2f o;
				o.vertex = mul(UNITY_MATRIX_MVP, v.vertex);
				o.texcoord = v.texcoord.xy;
				return o;
			}
			
			fixed4 frag (v2f i) : SV_Target {
				float w = _Radius;
				float2 h = float2(1, 0) * w;
				float2 v = float2(0, 16.0f/9.0f) * w;
				float4 th0 = tex2D(_MainTex, i.texcoord + h);
				float4 th1 = tex2D(_MainTex, i.texcoord - h);
				float4 tv0 = tex2D(_MainTex, i.texcoord + v);
				float4 tv1 = tex2D(_MainTex, i.texcoord - v);
				float bh0 = dot(th0, float4(1,1,1,0))/3;
				float bh1 = dot(th1, float4(1,1,1,0))/3;
				float bv0 = dot(tv0, float4(1,1,1,0))/3;
				float bv1 = dot(tv1, float4(1,1,1,0))/3;
				float dh = _Mul * (bh1 - bh0) / (2.0f * w);
				float dv = _Mul * (bv1 - bv0) / (2.0f * w);
				float a = dh > 1 || dh < -1 || dv > 1 || dv < -1 ? 0.0f : 1.0f;
				return float4( (dh + 1.0f) * 0.5f, (dv + 1.0f) * 0.5f, 0.5f, a);
			}
		ENDCG
	}
	
	
}

}
