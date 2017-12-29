#pragma strict

var timer : Timer;
private var startTime : float = 1000;

var osat : GameObject[];

function OpenLegs() {
	if (!this.gameObject.activeSelf) {
		this.gameObject.SetActive(true);
		this.startTime = timer.t();
		Update();
	}
}

function Update() {

	if (this.startTime == 1000) {
		this.startTime = this.timer.t();
	}

	var t = this.timer.t() - this.startTime;
	
	var dt = 0.5f;
	
	for(var i = 0; i < this.osat.length; i++) {
		var obu = this.osat[i];
		var jt = t - dt * i;
		if (jt > 0) {
			obu.SetActive(true);			
			var scale = FMath.Lerp(0,1,jt);
			obu.transform.localScale = new Vector3(scale, scale, scale);
			obu.transform.localRotation = Quaternion.AngleAxis(FMath.Lerp(180, 0, jt), new Vector3(1,0,0));
		} else {
			obu.SetActive(false);
		}
	}

}

