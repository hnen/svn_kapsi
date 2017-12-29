Shader "f/Simple" {
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
				float3 normal : TEXCOORD0;
			};

			v2f vert (appdata_base v) {
				v2f o;
				o.pos = mul(UNITY_MATRIX_MVP, v.vertex);
				o.normal = mul(UNITY_MATRIX_MV, float4(normalize(v.normal),0)).xyz;
				return o;
			}

			half4 frag (v2f i) : COLOR {
				return float4(_MainCol.rgb * i.normal.z, _MainCol.a);
			}
			ENDCG

		}
	}
	Fallback "VertexLit"
}
