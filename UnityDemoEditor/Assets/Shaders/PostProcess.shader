Shader "f/Postprocess" {
Properties {
	_MainTex ("Base (RGB)", 2D) = "white" {}
	_t ("T", Float) = 0
	_c1 ("c1", Color) = (1,1,1,1)
	_c2 ("c2", Color) = (1,1,1,1)
}

SubShader {
	Tags { "RenderType"="Opaque" }
	LOD 100

	Pass {
		CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag

			#include "UnityCG.cginc"

			struct appdata_t {
				float4 vertex : POSITION;
				float2 texcoord : TEXCOORD0;
			};

			struct v2f {
				float4 vertex : SV_POSITION;
				half2 texcoord : TEXCOORD0;
			};

			sampler2D _MainTex;
			float4 _MainTex_ST;
			float _t;
			
			float4 _c1, _c2;

			v2f vert (appdata_t v)
			{
				v2f o;
				o.vertex = mul(UNITY_MATRIX_MVP, v.vertex);
				o.texcoord = TRANSFORM_TEX(v.texcoord, _MainTex);
				return o;
			}
			
			float4 blink(float4 col) {
				//col *= 0.2f + (1.0f - fmod(_t, 0.09f)/0.09f) * 0.8f;
				//col *= (1.0f - fmod(_t, 0.075f)/0.075f) > 0.65f;
				return col * ((1.0f - fmod(_t, 0.075f)/0.075f) > 0.65f);
				//return col * (0.0f + (1.0f - fmod(_t, 0.075f)/0.075f) * 1.2f);
			}
			
			float4 colorize(float4 col) {
				float t = (fmod(_t, 0.085f)/0.085f) > 0.5f;
				return col * (t * _c1 + (1-t) * _c2);
			}

			fixed4 frag (v2f i) : SV_Target
			{
				float4 col = tex2D(_MainTex, i.texcoord);
				//col = col > 0.8f;
				col = blink(col);
				col = colorize(col);
				return col;
			}
			
			
		ENDCG
	}
}

}
