#pragma strict

var timer : AudioSource;
var beat : Beats;

var obu0 : GameObject;
var obu1 : GameObject;

function Start() {
	obu0.SetActive(false);
	obu1.SetActive(false);
}

function Update () {
	var time = timer.time;
	
	if (beat.Beat('Beat1')) {
		this.Next();
	}
}


function Next() {
	if (!obu0.activeSelf) {
		obu0.SetActive(true);
		obu1.SetActive(false);
	} else {
		obu0.SetActive(false);
		obu1.SetActive(true);
	}
}

