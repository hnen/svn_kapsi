#pragma strict

enum BeatType { FFT_ABSOLUTE, FFT_RELATIVE, BPM, ONE_SHOT };
class Beat extends System.Object {
	var name : String;
	var type : BeatType;
	var startTime : float;
	var endTime : float;
	
	var fftMidFreq : float;
	var fftFreqWindow : float;
	var fftThershold : float;
	
	var bpmTimeSecs : float;
	
	var hitThisFrame;
	var hitTime = 0.0f;
}

public var beats : Beat[];
public var audioSource : AudioSource;

private var fft : FFT;
private var prevTime : float;

function Start() {
	this.fft = this.GetComponent(FFT);
}

function Beat(name : String) {
	for(var i = 0; i < this.beats.length; i++) {
		var beat = beats[i];
		if (name == beat.name) {
			if (beat.hitThisFrame) {
				return true;
			}
		}
	}
	return false;
}


function TimeSince(name : String) {
	var time = this.GetTime();
	var best : float;
	best = 100000;
	for(var i = 0; i < this.beats.length; i++) {
		var beat = beats[i];
		if (name == beat.name) {
			var dt = time - beat.hitTime;
			if (dt < best) {
				best = dt;
			}
		}
	}
	return best;
}


function Update() {
	var time = this.GetTime();
	
	for(var i = 0; i < this.beats.length; i++) {
		var beat = beats[i];
		beat.hitThisFrame = false;
		if (beat.type == BeatType.BPM) {
			this.HandleBpmBeat(beat, time, prevTime);
		} else if (beat.type == BeatType.ONE_SHOT) {
			this.HandleOneShot(beat, time, prevTime);
		}
	}
	
	
	prevTime = time;
}

function GetTime() {
	var time : float;
	if (this.audioSource != null) {
		time = this.audioSource.time;
	} else {
		time = Time.time;
	}
	return time;
}

function HandleOneShot(beat : Beat, time : float, prevTime : float) {
	if (time >= beat.startTime && prevTime < beat.startTime) {
		beat.hitThisFrame = true;
		beat.hitTime = beat.startTime;
	}
}

function HandleBpmBeat(beat : Beat, time : float, prevTime : float) {
	var t = time - beat.startTime;
	var pt = prevTime - beat.startTime;
	if (t >= 0) {
		t = Mathf.Repeat(t, beat.bpmTimeSecs);
		pt = Mathf.Repeat(pt, beat.bpmTimeSecs);
		if (pt > t) {
			beat.hitThisFrame = true;
			beat.hitTime = time;
		}
	}
}


