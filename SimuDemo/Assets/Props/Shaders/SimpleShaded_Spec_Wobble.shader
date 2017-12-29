Shader "f/Materials/Simple Specular Wobble" {
	Properties {
		_MainCol ("Color", Color) = (1,1,1,1)
		_SpecCol ("SpecCol", Color) = (1,1,1,1)
		_Diff ("Diffuse Strength", Float) = 1.0
		_Spec ("Specular Strength", Float) = 1.0
		_SpecPow ("Specular Power", Float) = 1.0
		_D ("D Power", Range(0,10) ) = 0.0
		_E ("E Power", Range(0,3) ) = 0.0
		_T ("Speed", Range(0,1000) ) = 0.0
	}
	SubShader {
		Pass {
			CGPROGRAM

			#pragma vertex vert
			#pragma fragment frag
			#include "UnityCG.cginc"
			
			half4 _MainCol;
			half4 _SpecCol;
			half _Diff;
			half _Spec;
			half _SpecPow;
			half _D;
			half _E;
			half _T;

			struct v2f {
				float4 pos : SV_POSITION;
				float3 normal : TEXCOORD0;
				float3 light : TEXCOORD1;
				float3 vpos : TEXCOORD2;
				//float3 lv : TEXCOORD3;
			};

			v2f vert (appdata_base v) {
				v2f o;
				
				//float3 L = normalize(float3(0,1,-3));
				float3 L = normalize(float3(0,-1,-0.5));
				
				float3 pos = v.vertex.xyz;
				
				o.pos = mul(UNITY_MATRIX_MVP, float4(pos,1));

				float D = _D * _D;
				o.pos.y *= 1.0f + _E * sin( sin(pos.x) * D + _Time[0] * _T );
				o.pos.x *= 1.0f + _E * cos( sin(pos.y) * D + _Time[0] * _T );

				o.normal = mul(UNITY_MATRIX_MV, float4(normalize(v.normal),0)).xyz;
				o.vpos = normalize(mul(UNITY_MATRIX_MV, float4(pos, 1)).xyz);
				o.light = mul(UNITY_MATRIX_V, float4(L, 0)).xyz;
				
				return o;
			}

			half4 frag (v2f i) : COLOR {
				//return float4(_MainCol.rgb * i.normal.z * _Mul, _MainCol.a);
				
				float3 L = normalize(i.light);
				float3 N = normalize(i.normal);
				float3 V = normalize(i.vpos);
				float3 H = normalize(V+L);
				
				float LdotN = saturate(dot(L, N));
				//float NdotH = saturate(dot(N, H));
				float NdotH = dot(N, H);
				
				float3 diff = _MainCol.rgb * LdotN * _Diff;
				float3 spec = _SpecCol.rgb * pow(NdotH, _SpecPow) * _Spec;
				
				return float4(diff + spec, _MainCol.a);
			}
			ENDCG

		}
	}
	Fallback "VertexLit"
}
