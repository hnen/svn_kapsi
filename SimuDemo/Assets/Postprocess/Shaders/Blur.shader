Shader "f/Postprocess/Blur" {
Properties {
	_MainTex ("Base (RGB)", 2D) = "white" {}
	_Radius ("Radius", Float) = 0.025
}

CGINCLUDE

	#pragma target 3.0

	sampler2D _MainTex;
	float4 _MainTex_ST;
	
	static const int _Q = 80;
	
	float _Radius;
	

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
				//o.texcoord = TRANSFORM_TEX(v.texcoord, _MainTex);
				o.texcoord = v.texcoord.xy;
				return o;
			}
			
			fixed4 frag (v2f i) : SV_Target {
				float2 v = float2(9.0f / 16.0f,0) * _Radius;
			
				return blur(_MainTex, i.texcoord.xy, v, (int)_Q);
			}
		ENDCG
	}
	
	GrabPass {}
	
	Pass {
		CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag

			#include "UnityCG.cginc"
			
			sampler2D _GrabTexture;

			v2f vert (appdata_t v)
			{
				v2f o;
				o.vertex = mul(UNITY_MATRIX_MVP, v.vertex);
				//o.texcoord = TRANSFORM_TEX(v.texcoord, _MainTex);
				o.texcoord = v.texcoord.xy;
				return o;
			}
			
			fixed4 frag (v2f i) : SV_Target {
				float2 v = float2(0,1) * _Radius;
			
				return blur(_GrabTexture, i.texcoord.xy, v, (int)_Q);
				//return float4(1,0,1,1);
			}
		ENDCG
	}
	
	
}

}
