using UnityEngine;
using System.Collections;

public class HandleBgMat : MonoBehaviour {

	public Material BgMat;
	public Material OverlayMat;
	public Material BloomMat;
	public DemoTimeline tl;

	void Update () {

		if (Camera.main != null) {
			BgMat.SetVector("_CamDir", Camera.main.transform.forward);
			BgMat.SetVector("_CamUp", Camera.main.transform.up);
			BgMat.SetVector("_CamRight", Camera.main.transform.right);
		}
		BgMat.SetFloat("_Z", 0.5f + tl.Beat() * 0.2f);
		BgMat.SetFloat("_T", Time.time);
		OverlayMat.SetFloat("_T", Time.time);
		//BloomMat.SetFloat("_Tex1Mul", 5.5f + tl.Beat2() * 1.0f);

		 

		bool totalFuckup = (tl.T() > 31000 && tl.T() < 32700) || (tl.T() > 60000 && tl.T() < 61700);
		if (totalFuckup) {
			BgMat.SetFloat("_T", Time.time * 100.0f);
			BgMat.SetFloat("_Mul", Mathf.Repeat(Time.time * 20.0f, 1.0f) * 5.0f);
		} else {
			BgMat.SetFloat("_T", Time.time);
			BgMat.SetFloat("_Mul", 1.0f);
		}

		//BgMat.SetFloat("_Mul", -5.0f + (1.0f - tl.Beat()) + 10.5f);

	}
}
