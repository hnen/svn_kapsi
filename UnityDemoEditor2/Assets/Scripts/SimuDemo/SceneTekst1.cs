using UnityEngine;
using System.Collections;
using Vector3 = f.Vector3;

public class SceneTekst1 : MonoBehaviour {

	public DemoTimeline tl;

	public GameObject cameraObj;

	public GameObject Tekst1;
	public GameObject Tekst2;
	public GameObject Tekst3;

	public GameObject txt;

	public int [] times0;

	void OnEnable() {
		SetTekst(Tekst1);
	}

	void Update() {
		if (tl.PassedT(41600)) {
			SetTekst(Tekst2);
		}
		if (tl.PassedT(48500)) {
			SetTekst(Tekst3);
		}

		for (int i = 0; i < times0.Length; i++) {
			if (tl.PassedT(times0[i] - 70) || tl.PassedT(times0[i] + 6950 - 215) || tl.PassedT(times0[i] + 2*6950 - 350)) {
				RandomizeCamera();
			}
		}

		//if (Input.GetKeyDown(KeyCode.Space)) {
		//	Debug.Log("" + tl.T ());
		//}
	}

	void RandomizeCamera() {
		cameraObj.transform.position = this.txt.transform.position.Add(new Vector3(
			Random.Range(-1.0f, 1.0f),
			Random.Range(-1.0f, 1.0f),
			-1.0f));

		cameraObj.transform.forward = (this.txt.transform.position.Sub(cameraObj.transform.position));
	}

	void SetTekst(GameObject txtObj) {
		Tekst1.SetActive(false);
		Tekst2.SetActive(false);
		Tekst3.SetActive(false);
		txtObj.SetActive(true);
		this.txt = txtObj;
	}

}
