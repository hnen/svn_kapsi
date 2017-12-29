Shader "f/Postprocess/Combine Transparent2" {
Properties {
	_Tex0 ("Tex0", 2D) = "white" {}
	_Tex1 ("Tex1", 2D) = "white" {}
	_Tex1Add ("Tex1Add", Float) = 0.0
	_Tex1Mul ("Tex1Mul", Float) = 1.0
}

CGINCLUDE
	sampler2D _Tex0;
	sampler2D _Tex1;
	
	float4 _Tex0_ST;
	float4 _Tex1_ST;
	
	float _Tex1Add;
	float _Tex1Mul;
	
	float _T;
	
	struct appdata_t {
		float4 vertex : POSITION;
		float2 texcoord : TEXCOORD0;
	};

	struct v2f {
		float4 vertex : SV_POSITION;
		half2 texcoord : TEXCOORD0;
	};
		
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
				float4 c0 = tex2D(_Tex0, i.texcoord.xy);
				float4 c1 = tex2D(_Tex1, i.texcoord.xy + float2(1,1)*_T);
			
				return c0*((c1 + 0.75f)/1.75f) * 1.5f;
			}
		ENDCG
	}
}

}
