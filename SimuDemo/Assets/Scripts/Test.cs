using UnityEngine;
using System.Collections;
using Vector3 = f.Vector3;

public class Test : MonoBehaviour {

	public float kakki;

	Vector3 origin;
	
	void Start () {
		origin = this.transform.position;
	}

	void Update () {
		var t = Time.time;
		this.transform.position = origin.Add(new Vector3(Mathf.Sin(t), 0, 0));
	}
}
