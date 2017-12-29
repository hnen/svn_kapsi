using UnityEngine;
using System.Collections;

public class SceneLoppuLogo : MonoBehaviour {

	public GameObject cameraObj;

	void Update() {
		cameraObj.transform.position = cameraObj.transform.position.Add(new Vector3(0,0,1).Mul(-Time.smoothDeltaTime * 0.1f));
	}

}
