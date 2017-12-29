#pragma strict

var grr : FFT;
var k: int;
var mater : UnityEngine.Material;
var sh: UnityEngine.Shader;
var lt : int;

function Start () {
	grr = GameObject.Find( "Musa" ).GetComponent( FFT );
	mater = (this.GetComponent( MeshRenderer ) as MeshRenderer).material;
	sh = mater.shader;
	k = 0;
	lt = 0;
}

function Update () {
	var d = grr.getFFT( 3.0, 2.5, 0 );
	var d2 = grr.getFFT( 3.0, 2.5, -1 );
	
	if ( d2-d > 0.05 && Time.frameCount > lt + 5 )
	{
		lt = Time.frameCount;
		var tr = this.transform;
		switch ( k ) 
		{
			case 0:
				tr.rotation = Quaternion.Euler( 45.0, 45.0, 0.0 );
				tr.localScale = Vector3( 3.0, 3.0, 1000.0 );
				sh.SetGlobalFloat( "Slider", 10.1 );
				mater.SetFloat( "Slider", 10.1 );
				
				k = 1;
				break;
			case 1:
				tr.rotation = Quaternion.Euler( 0.0, 90.0, 0.0 );
				tr.localScale = Vector3( 3.0, 3.0, 1000.0 );
				sh.SetGlobalFloat( "Slider", 10.0 );
				mater.SetFloat( "Slider", 10.0 );
				k = 2;
				break;
			case 2:
				tr.rotation = Quaternion.Euler( 90.0, 0.0, 0.0 );
				tr.localScale = Vector3( 3.0, 3.0, 1000.0 );
				sh.SetGlobalFloat( "Slider", 2.0 );
				mater.SetFloat( "Slider", 2.0 );
				k = 3;
				break;
			case 3:
				tr.rotation = Quaternion.Euler( 0.0, 0.0, 90.0 );
				tr.localScale = Vector3( 3.0, 3.0, 1000.0 );
				sh.SetGlobalFloat( "Slider", 6.0 );
				mater.SetFloat( "Slider", 6.0 );
				k = 0;
				break;
				
		}
	}
}

