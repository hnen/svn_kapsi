Shader "f/Materials/Solid Color" {
	Properties {
		_MainCol ("Color", Color) = (1,1,1,1)
	}
	SubShader {
		Pass {
			CGPROGRAM

			#pragma vertex vert
			#pragma fragment frag
			#include "UnityCG.cginc"
			
			half4 _MainCol;

			struct v2f {
				float4 pos : SV_POSITION;
				//float3 color : COLOR0;
			};

			v2f vert (appdata_base v) {
				v2f o;
				o.pos = mul(UNITY_MATRIX_MVP, v.vertex);
				//o.color = v.normal * 0.5 + 0.5;
				return o;
			}

			half4 frag (v2f i) : COLOR {
				return _MainCol;
				//return half4 (1,1,1, 1);
			}
			ENDCG

		}
	}
	Fallback "VertexLit"
}
