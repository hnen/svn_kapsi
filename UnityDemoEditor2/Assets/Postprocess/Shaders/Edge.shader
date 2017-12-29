Shader "f/Postprocess/Edge Detect" {
Properties {
	_MainTex ("Base (RGB)", 2D) = "white" {}
	_t ("T", Float) = 0
	_m ("m", Float) = 0
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
			half4 _c1;
			float _t, _m;

			v2f vert (appdata_t v)
			{
				v2f o;
				o.vertex = mul(UNITY_MATRIX_MVP, v.vertex);
				//o.texcoord = TRANSFORM_TEX(v.texcoord, _MainTex);
				o.texcoord = v.texcoord.xy;
				return o;
			}
			
			fixed4 frag (v2f i) : SV_Target
			{
				float aspect = 16.0f / 9.0f;
				float dist = _t;
				float4 cols[4] = {
					tex2D(_MainTex, i.texcoord + float2(-1, 0) * dist * aspect),
					tex2D(_MainTex, i.texcoord + float2( 1, 0) * dist * aspect),
					tex2D(_MainTex, i.texcoord + float2( 0,-1) * dist),
					tex2D(_MainTex, i.texcoord + float2( 0, 1) * dist)
				};
				float b[4] = {
					dot(cols[0], float4(1,1,1,0)) / 3,
					dot(cols[1], float4(1,1,1,0)) / 3,
					dot(cols[2], float4(1,1,1,0)) / 3,
					dot(cols[3], float4(1,1,1,0)) / 3,
				};
				float v = abs(cols[0] - cols[1]);
				float h = abs(cols[2] - cols[3]);
				if (v+h > 0.15f) {
					return _c1;
				} else {
					return float4(0,0,0,0);
				}
			}
			
			
		ENDCG
	}
}

}
