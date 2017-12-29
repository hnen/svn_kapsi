using UnityEngine;
using System.Collections;

public class DemoTimeline : MonoBehaviour {

	public AudioSource DemoSong;
	public int DemoLength;
	public int currTime;

	public int startTime;
	private int prevTime;

	private float timer;

	public int interval;

	public void Start() {
		this.timer = (float)startTime / 1000;
		DemoSong.time = (float)startTime / 1000;
		prevTime = 0;
	}

	public void Update() {
		//this.prevTime = T();
		//timer += UnityEngine.Time.smoothDeltaTime;


		currTime = T();
	}

	public float Beat() {
		var t = T();
		if (t > 34650 && t < 48100) {
			return 0;
		} else
		if (t < 157500) {
			var it = t % interval;
			var ft = (float)it / interval;
			return ft;
		} else {
			return 0;
		}
	}
	public float Beat2() {
		var t = T();
		if (t < 157500) {
			var it = t % (interval / 4);
			var ft = (float)it / (interval/4);
			return ft;
		} else {
			return 0;
		}
	}

	void LateUpdate() {
		prevTime = T();
	}

	public float Time() {
		return DemoSong.time;
		//return timer;
	}


	public int T() {
		return Mathf.FloorToInt(Time() * 1000);
	}

	public bool PassedT(int t) {
		//Debug.Log(string.Format("{0} > {1} > {2} ?", this.prevTime, t, this.T()));
		return this.prevTime < t && this.T() >= t;
	}


}
