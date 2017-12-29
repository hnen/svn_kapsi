#pragma strict

var timer : AudioSource;
var time : float;
var ptime : float;
var startTime : float;

var timerMaterial : Material;

var kuva : Transform;
var kuva_activechild : int;

function Start() {
	if (timer != null) {
		timer.time = startTime;
	}	
}

function Update () {
	if (timer != null) {
		this.time = timer.time;
	} else {
		this.time = Time.time;
	}
	
	timerMaterial.SetFloat("_t", this.time);
	kuva.localScale = new Vector3(1,1,1) * (1.0f + 0.1f * Mathf.Repeat(this.time, 0.5f) / 0.5f);
	kuva.localRotation = Quaternion.AngleAxis(this.time * 20.0f, new Vector3(0,0,1));
	var kuvaT = Mathf.Repeat(this.time, 0.5f);
	var kuvaPT = Mathf.Repeat(this.ptime, 0.5f);
	
	if (kuvaT < kuvaPT) {
		kuva_setActiveChild(1-kuva_activechild);
	}

	
	this.ptime = time;
}


function kuva_setActiveChild(k : int) {
	for(var i = 0; i < kuva.childCount; i++) {
		kuva.GetChild(i).gameObject.SetActive(false);
	}
	kuva.GetChild(k).gameObject.SetActive(true);
	kuva_activechild = k;
}


