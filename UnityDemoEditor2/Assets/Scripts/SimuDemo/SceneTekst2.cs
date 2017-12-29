using UnityEngine;
using System.Collections;
using Vector3 = f.Vector3;

public class SceneTekst2 : MonoBehaviour {

	public string [] tekstit;
	public GameObject textobj;
	public FontText text;
	public DemoTimeline tl;
	public SimuDemo demo;

	public GameObject cameraObj;

	public int timeInterval;

	private int textLen;

	void OnEnable() {
		SetText(0);
		RandomizeCamera();
	}

	void Update() {
		var t = tl.T() - demo.TimeSceneTekst2;

		for(int i = 0; i < tekstit.Length; i++) {
			for (int j = 0; j < 2; j++) {
				if (tl.PassedT(demo.TimeSceneTekst2 + i * this.timeInterval + j * this.timeInterval / 2)) {
					RandomizeCamera();
				}
			}
			if (tl.PassedT(demo.TimeSceneTekst2 + i * this.timeInterval)) {
				SetText(i);
			}
		}

		textobj.transform.rotation = Quaternion.AngleAxis(
			-90 + Mathf.Repeat( 4.0f * (float)t * 180.0f / timeInterval, 180.0f ), new Vector3(0,1,0));
	}

	void RandomizeCamera() {
		cameraObj.transform.position = new Vector3( 0, Random.Range(-1.5f, 2.5f), -textLen * 0.5f );
		cameraObj.transform.forward = Vector3.Normalized(new Vector3(0,0,0).Sub(cameraObj.transform.position));
	}

	void SetText(int i) {
		text.SetText(tekstit[i]);
		textLen = tekstit[i].Length;
	}

}
