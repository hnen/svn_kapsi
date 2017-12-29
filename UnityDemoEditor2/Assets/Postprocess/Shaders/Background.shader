Shader "f/Postprocess/Simudemo Background" {
Properties {
	_MainTex  ("Base Tex", 2D) = "white" {}
	_CamDir ("CameraDir", Vector) = (0,0,1,0)
	_CamUp ("CameraUp", Vector) = (0,0,1,0)
	_CamRight ("CameraRight", Vector) = (0,0,1,0)
	_Z ("Z", Range(0,2)) = 1.
	_Mul ("mul", Range(0,2)) = 1
}

SubShader {
	Tags { "RenderType"="Opaque" }
	LOD 100

	ZWrite Off

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
			float _Z;
			
			float _T;
			
			float4 _CamDir;
			float4 _CamUp;
			float4 _CamRight;
			float _Mul;

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
				float2 d = (i.texcoord - float2(0.5f,0.5f)) * 2.0f;
				float3 dir = normalize(_CamDir * _Z + _CamRight * d.x + _CamUp * d.y);
				//float3 dir = normalize(float3(d.x, d.y, _Z));
				float x = acos(dir.x);// + atan2(_CamDir.x, _CamDir.z);
				float y = acos(dir.y);// + atan2(_CamDir.y, _CamDir.z);
				//float3 p = dir * 5.0f;
				float4 tx1 = (tex2D(_MainTex, float2(x,y+_T*0.5) * float2(0.4,1)) * 0.5);
				float4 tx2 = (tex2D(_MainTex, float2(x+0.5f,y-_T*0.5) * float2(0.4,1)) * 0.5);
				
				return pow(tx1 * tx2 ,1.0f) * 1.5f * _Mul;
				//return pow(tx1,2) * pow(tx2,2) * 8.0f;
			}
			
			
		ENDCG
	}
}

}
