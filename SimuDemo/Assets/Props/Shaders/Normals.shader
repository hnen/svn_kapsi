Shader "f/Materials/Visualize Normals" {
	Properties {
		_MainCol ("Color", Color) = (1,1,1,1)
		_Mul ("Mul", Float) = 1.0
	}
	SubShader {
	
		Cull Off
	
		Pass {
			CGPROGRAM

			#pragma vertex vert
			#pragma fragment frag
			#include "UnityCG.cginc"
			
			half4 _MainCol;
			half _Mul;

			struct v2f {
				float4 pos : SV_POSITION;
				float3 normal : TEXCOORD0;
			};

			v2f vert (appdata_base v) {
				v2f o;
				o.pos = mul(UNITY_MATRIX_MVP, v.vertex);
				o.normal = mul(UNITY_MATRIX_MV, float4(normalize(v.normal),0)).xyz;
				//o.normal = v.normal;
				return o;
			}

			half4 frag (v2f i) : COLOR {
				return float4((i.normal + float3(1,1,1)) * 0.5f, _MainCol.a);
			}
			ENDCG

		}
	}
	Fallback "VertexLit"
}
