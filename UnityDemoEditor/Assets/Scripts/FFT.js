#pragma strict

public var fftData : TextAsset;
public var fftSamplesPerSecond : int;
public var fftFreqMaxKhz : float;

//public var reconstruction : Texture2D; 

private var fftSampleCount : int;
private var fftFreqCount : int;

private var data : byte[];

function Start () {
	this.data = this.fftData.bytes;

	var sampleCount = this.data[0] + (this.data[1] << 8) + (this.data[2] << 16) + (this.data[3] << 24);
	var freqCount = this.data[4] + (this.data[5] << 8) + (this.data[6] << 16) + (this.data[7] << 24);
	
	this.fftSampleCount = sampleCount;
	this.fftFreqCount = freqCount;
	
//	GenReconstruction();
	
}

function getFFT(freqKhz : float, freqWindowLength : float, dt : int) {

	var audioSource : AudioSource = this.GetComponent(AudioSource);
	var t = audioSource.time;
	var sample = t * this.fftSamplesPerSecond;
	
	var minFreq = freqKhz - freqWindowLength;
	var maxFreq = freqKhz + freqWindowLength;
	
	var minSample = Mathf.Round((minFreq / this.fftFreqMaxKhz) * this.fftFreqCount);
	var maxSample = Mathf.Round((maxFreq / this.fftFreqMaxKhz) * this.fftFreqCount);
	
	var result = 0.0;
	var w = 0.0;
	
	//Debug.Log(String.Format("{0} -> {1} ==> {2} -> {3}", minFreq, maxFreq, minSample, maxSample));
	
	for(var i = minSample; i <= maxSample; i+=1) {
		var freq = (i / this.fftFreqCount) * this.fftFreqMaxKhz;
		var d = Mathf.Abs(freq - freqKhz);
		var x = 1.0 - d / freqWindowLength;
		w += x;
		result += this.getSampleInterpolated(sample + dt, 5, i) * x;
	}
	
	//Debug.Log(String.Format("{0}/{1} = {2}", result, w, result/w));
	
	return result/w;
}

function getSampleInterpolated(sample : float, sampleWindow : int, freq : int) : float {
	var minSample = Mathf.Floor(sample) - sampleWindow;
	var maxSample = Mathf.Floor(sample) + sampleWindow;

	var result = 0.0;
	var w = 0.0;

	//for (var i = minSample; i <= maxSample; i++) {
	//	var d = Mathf.Abs(i - sample);
	//	var x = d / sampleWindow;
	//	w += x;
	//	result += getSample(i, freq) * x;
	//}
	//return result / w;
	return this.getSample(sample, freq);
}


function getSample(sample : int, freq : int) : float {
	if (sample < 0 || sample >= this.fftSampleCount || freq < 0 || freq >= this.fftFreqCount) {
		return 0;
	}
	//return (this.data[8 + (this.fftSampleCount - sample - 1) * this.fftFreqCount + freq]) / 255.0f;
	return (this.data[8 + (freq) * this.fftSampleCount + Mathf.Floor(sample)]) / 255.0f;
}
/*
function GenReconstruction() {
	this.reconstruction = new Texture2D(this.fftSampleCount, this.fftFreqCount, TextureFormat.ARGB32, false, true);
	var cols = new Color[this.fftSampleCount * this.fftFreqCount];
	for (var x = 0; x < this.fftSampleCount; x++) {
		for (var y = 0; y < this.fftFreqCount; y++) {
			var v = getSample(x,y);
			//var v = this.data[8 + y * this.fftSampleCount + x] / 255.0;
			cols[y * this.fftSampleCount + x] = new Color(v,v,v,1);
		}
	}
	this.reconstruction.SetPixels(cols);
	this.reconstruction.Apply(false, false);
}
*/
