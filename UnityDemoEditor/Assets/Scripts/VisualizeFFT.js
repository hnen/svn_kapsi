
public var fft : FFT;
private var mat : Material;

function OnPostRender() {
	// Create a shader that renders white only to alpha channel
	if(!mat) {
		mat = new Material( "Shader \"Hidden/SetAlpha\" {" +
			"SubShader {" +
			"	Pass {" +
			"		ZTest Always Cull Off ZWrite Off" +
			"		ColorMask RGBA" +
			"		Color (1,1,1,1)" +
			"	}" +
			"}" +
			"}"
		);
	}
	// Draw a quad over the whole screen with the above shader
	GL.PushMatrix ();
	GL.LoadOrtho ();
	GL.Begin( GL.QUADS );
	for (var i = 0; i < mat.passCount; ++i) {
		mat.SetPass(i);
		for(var j : float = 0; j < fft.fftFreqMaxKhz; j+=0.35) {
			//var v0 = fft.getFFT(j, 1.5, 1);
			//var v1 = fft.getFFT(j, 1.5, -1);
			//var val = Mathf.Exp(v0)-Mathf.Exp(v1);
			//var val = v1-v0;
			var val : float = 0;
			for (var k = 0; k < 1; k++) {
				var v = fft.getFFT(j, 1.5, -k);
				if (v > val) val = v;
			}
			drawQuad(j * 0.04, 0.0 + Mathf.Max(val, 0) * 0.4);
		}
	}
	GL.End();
	GL.PopMatrix ();	
}

function drawQuad(x, y) {
	GL.Vertex3( x+0,     y+0.05, 0.1 );
	GL.Vertex3( x+0.01, y+0.05, 0.1 );
	GL.Vertex3( x+0.01, y+0, 0.1 );
	GL.Vertex3( x+0,     y+0, 0.1 );
}









