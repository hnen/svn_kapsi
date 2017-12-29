using UnityEngine;
using System.Collections;
using Vector3 = f.Vector3;

public class SceneTekst4 : MonoBehaviour {


	public string [] tekstit;
	public GameObject textobj;
	public GameObject cameraObj;
	public FontText text;
	public DemoTimeline tl;
	public SimuDemo demo;

	public int timeInterval;

	void OnEnable() {
		SetText(0);
	}

	void Update() {
		var t = tl.T() - demo.TimeSceneTekst4;

		for(int i = 0; i < tekstit.Length; i++) {
			if (tl.PassedT(demo.TimeSceneTekst4 + i * this.timeInterval - timeInterval/10)) {
				textobj.transform.localScale = new Vector3(1,1,1);
				SetText(i);
				RandomizeCamera();
			}
		}

		var it = (float)((tl.T() - demo.TimeSceneTekst4) % timeInterval) / timeInterval;

		var s = 1.0f;
		if (it < 0.1f) {
			it /= 0.1f;
			s = 1.0f - (1.0f - it)*(1.0f - it);
		} else {
			it = (it - 0.3f) / 0.5f;
			if (it < 0) it = 0;
			if (it > 1) it = 1;
			s = 1.0f - it;
			s *= s;
		}
		textobj.transform.localScale = (new Vector3(1,1,1)).Mul(s);


	}

	
	void RandomizeCamera() {
		cameraObj.transform.position = this.textobj.transform.position.Add(new Vector3(
			Random.Range(-1.0f, 1.0f),
			Random.Range(-1.0f, 1.0f),
			-1.0f));
		
		cameraObj.transform.forward = (this.textobj.transform.position.Sub(cameraObj.transform.position));
	}


	void SetText(int i) {
		text.SetText(tekstit[i]);
	}

}
