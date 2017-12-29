#pragma strict

var timer : AudioSource;

var time : float;
var startFromTime : float;

var beats : Beats;

var scene1 : scene1;
var scene2 : scene2;
var scene3 : scene3;
var scene : int;

function Start() {
	if (timer != null) {
		timer.time = startFromTime;
	}
	
	scene1.gameObject.SetActive(true);
	scene = 1;
}

function Update () {
	if (timer != null) {
		time = timer.time;
	} else {
		time = Time.time;
	}
	
	if (this.beats.Beat("start_scene_2")) {
		scene1.gameObject.SetActive(false);
		scene2.gameObject.SetActive(true);
		scene3.gameObject.SetActive(false);
		scene = 2;
	}
	
	if (this.beats.Beat("start_scene_3")) {
		scene1.gameObject.SetActive(false);
		scene2.gameObject.SetActive(false);
		scene3.gameObject.SetActive(true);
		scene = 3;
	}
	
	if (scene == 1) {
		this.runScene1();
	}
	if (scene == 2) {
		this.runScene2(this.beats.TimeSince("start_scene_2"));
	}
	if (scene == 3) {
		this.runScene3(this.beats.TimeSince("start_scene_3"));
	}
}

function runScene1() {
	var path = this.scene1.path;
	var p = time * 0.1;
	var pos = path.GetPosition(p);
	var rot = path.GetRotation(p);
	this.scene1.scenecamera.transform.position = pos;
	this.scene1.scenecamera.transform.rotation = rot;
	
}

function runScene2(time : float) {
	var path = this.scene2.path;
	var p = time * 0.33;
	var pos = path.GetPosition(p);
	var rot = path.GetRotation(p);
	this.scene2.scenecamera.transform.position = pos;
	this.scene2.scenecamera.transform.rotation = rot;	
}

function runScene3(time : float) {
	var pos = this.scene3.campos.position;
	var rot = this.scene3.campos.rotation;
	this.scene3.scenecamera.transform.position = pos;
	this.scene3.scenecamera.transform.rotation = rot;
	
	if (this.beats.Beat("scene3_open_legs")) {
		this.scene3.hamis.OpenLegs();
	}
	
}

