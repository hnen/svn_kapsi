using UnityEngine;
using System.Collections;
using Vector3 = f.Vector3;


public class SceneJoro : MonoBehaviour {

	public GameObject [] pathit0;
	public GameObject [] pathit1;

	public DemoTimeline tl;
	public SimuDemo demo;

	public GameObject cameraObj;

	public int interval;

	void Update() {

		var t = tl.T() - demo.TimeSceneJoro;
		if (t < 0 ) return;

		var pathi = Mathf.FloorToInt(t / interval);
		pathi %= pathit0.Length;

		var g0 = pathit0[pathi];
		var g1 = pathit1[pathi];

		var pt = t % interval;
		var ft = (float)pt / interval;

		cameraObj.transform.position = g0.transform.position.Add((g1.transform.position.Sub(g0.transform.position)).Mul(ft));
		cameraObj.transform.rotation = Quaternion.Slerp(g0.transform.rotation, g1.transform.rotation, ft);

		cameraObj.transform.position = cameraObj.transform.position.Add(
			new Vector3(
			Mathf.Sin((float)t) + Mathf.Cos((float)t*1.1f),
			Mathf.Cos((float)t) + Mathf.Cos((float)t*1.3f),
			Mathf.Sin((float)t) + Mathf.Cos((float)t*0.8f)).Mul(0.01f));
		      

	}

}
