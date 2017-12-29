#pragma strict

var jalat : Jalka[];
var timer : Timer;

var legsopen : boolean;
var legsopen_t : float;

function Start() {
	for(var jalka in this.jalat) {
		jalka.gameObject.SetActive(false);
	}
}

function OpenLegs() {
	if (!this.legsopen) {
		this.legsopen = true;
		this.legsopen_t = this.timer.t();
		Update();
	}
}

function Update() {
	if (this.legsopen) {
		var t = this.timer.t() - this.legsopen_t;
		var dt = 0.3;
		for(var i = 0; i < jalat.length; i+=2) {
			var jt = t - dt*i/2;
			if (jt > 0) {
				for(var j = 0; j < 2; j++) {
					jalat[i+j].OpenLegs();
				}
			}
		}
	}
}

