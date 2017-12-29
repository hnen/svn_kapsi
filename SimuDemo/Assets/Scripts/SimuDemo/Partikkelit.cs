using UnityEngine;
using System.Collections;

public class Partikkelit : MonoBehaviour {

	public GameObject particlePref;
	public Camera dstCamera;
	public RenderTexture srcTex;

	private Texture2D data;

	public void Start() {
		for(int i = 0; i < 1000; i++) {
			SpawnParticle();
		}
	}

	void Update() {
		var img = srcTex.
	}

	public void SpawnParticle() {
		var obj = GameObject.Instantiate(particlePref);
		var h = dstCamera.orthographicSize;
		var w = dstCamera.orthographicSize * Screen.width / Screen.height;
		var x = Random.Range(-w, w);
		var y = Random.Range(-h, h);
		obj.transform.position = dstCamera.transform.position + new Vector3(x, y, 5.0f);
	}

}
