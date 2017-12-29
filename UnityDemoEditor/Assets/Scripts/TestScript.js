#pragma strict

public var fft : FFT;

function Start () {

}

function Update () {
	var t = Time.time;
	this.transform.localPosition.y = Mathf.Sin(t) * 2.5;
	
	var f : float;
	f = 1.0f + this.fft.getFFT(1, 1.5f, 0) * 10;
	this.transform.localScale = new Vector3(f,f,f);
	
}

function OnRenderObject() {



}
