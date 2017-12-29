
Shader "Minua" {
	Properties {
		_Color ("Main Color", Color) = (1,1,1,1)
		_MainTex ("Base (RGB)", 2D) = "white" {}
		_Slider( "Slider", float )  = 8.0
	}
	SubShader {
		
        Tags { "RenderType"="Opaque" }
        LOD 200
		Cull Off
		ZWrite Off
		Blend SrcAlpha OneMinusSrcAlpha

		Pass 
		{
			CGPROGRAM
			// Upgrade NOTE: excluded shader from OpenGL ES 2.0 because it does not contain a surface program or both vertex and fragment programs.
			#pragma exclude_renderers gles
			#pragma fragment frag
			#pragma vertex vert
			#include "UnityCG.cginc"
 
			sampler2D _MainTex;
			fixed4 _Color;
			float _Slider;

			struct Input {
				float4 pos : SV_POSITION;
				float2 uv_MainTex : TEXCOORD0;
			};

			struct Vertex {
				float4 pos : POSITION;
//				float3 norm : NORMAL;
				float2 uv : TEXCOORD0; 
			};


			Input vert( Vertex IN )
			{
				Input OUT;
				OUT.pos = mul( UNITY_MATRIX_MVP, IN.pos );
				OUT.pos.z = 1;
/*				float2 d = normalize( IN.uv.xy );
				OUT.uv_MainTex = abs( d );*/
				OUT.uv_MainTex = IN.pos.xy;
				return OUT;
			}



			float4 frag( Input IN ) : COLOR 
			{
				float2 tex = IN.uv_MainTex * float2(_Slider, _SinTime[2] );
				//Fixed v = fmod(_SinTime[3]*1.0f, 1.0) * 10.0;
				tex.x += _SinTime[3];
				tex.y += _SinTime[3] / 100.0f;
				//tex.x = fmod( tex.x, 20.0 );
				//tex.y = fmod( tex.y, 0.1 );
				tex = frac(tex);
				float4 c = tex2D(_MainTex, tex ) * _Color;
				return c*c;
			}

			ENDCG
		}
	}

	Fallback "VertexLit"
}
