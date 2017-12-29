using UnityEngine;
using System.Collections;
using Vector3 = f.Vector3;

public class SceneAlkuLogo : MonoBehaviour {

	public DemoTimeline tl;

	public GameObject node0a;
	public GameObject node0b;
	public int t0a;
	public int t0b;

	public GameObject node1a;
	public GameObject node1b;
	public int t1a;
	public int t1b;
	
	public GameObject node2a;
	public GameObject node2b;
	public int t2a;
	public int t2b;
	
	public GameObject node3a;
	public GameObject node3b;
	public int t3a;
	public int t3b;
	
	public GameObject node4a;
	public GameObject node4b;
	public int t4a;
	public int t4b;
	
	public GameObject node5a;
	public GameObject node5b;
	public int t5a;
	public int t5b;
	
	public GameObject node6a;
	public GameObject node6b;
	public int t6a;
	public int t6b;
	
	public GameObject node7a;
	public GameObject node7b;
	public int t7a;
	public int t7b;

	public GameObject cameraObject;

	public GameObject textObj;

	private Vector3 origPos;
	private Quaternion origRot;

	void Awake() {
		origPos = cameraObject.transform.position;
		origRot = cameraObject.transform.rotation;
	}

	void Update() {
		var t = tl.T();

		textObj.transform.rotation = Quaternion.AngleAxis(tl.Time() * 425, new Vector3(0,1,0));

		var b = tl.Beat();
		var m = 1.0f - b * 1.0f;
		if (m < 0) m = 0;

		textObj.transform.position = new Vector3(0, Mathf.Cos(b*5) * m * 0.2f, 0);


		if (t >= t0a && t < t0b) {
			var pos0 = node0a.transform.position;
			var pos1 = node0b.transform.position;
			var rot0 = node0a.transform.rotation;
			var rot1 = node0b.transform.rotation;

			var ft = (float)(t - t0a) / (t0b - t0a);

			cameraObject.transform.position = pos0.Add(pos1.Sub(pos0).Mul(ft));
			cameraObject.transform.rotation = Quaternion.Slerp(rot0, rot1, ft);

		} else if (t >= t1a && t < t1b) {
			var pos0 = node1a.transform.position;
			var pos1 = node1b.transform.position;
			var rot0 = node1a.transform.rotation;
			var rot1 = node1b.transform.rotation;
			
			var ft = (float)(t - t1a) / (t1b - t1a);
			
			cameraObject.transform.position = pos0.Add(pos1.Sub(pos0).Mul(ft));
			cameraObject.transform.rotation = Quaternion.Slerp(rot0, rot1, ft);

		} else if (t >= t2a && t < t2b) {
			var pos0 = node2a.transform.position;
			var pos1 = node2b.transform.position;
			var rot0 = node2a.transform.rotation;
			var rot1 = node2b.transform.rotation;
			
			var ft = (float)(t - t2a) / (t2b - t2a);
			
			cameraObject.transform.position = pos0.Add(pos1.Sub(pos0).Mul(ft));
			cameraObject.transform.rotation = Quaternion.Slerp(rot0, rot1, ft);

		} else if (t >= t3a && t < t3b) {
			var pos0 = node3a.transform.position;
			var pos1 = node3b.transform.position;
			var rot0 = node3a.transform.rotation;
			var rot1 = node3b.transform.rotation;
			
			var ft = (float)(t - t3a) / (t3b - t3a);
			
			cameraObject.transform.position = pos0.Add(pos1.Sub(pos0).Mul(ft));
			cameraObject.transform.rotation = Quaternion.Slerp(rot0, rot1, ft);

		} else if (t >= t4a && t < t4b) {
			var pos0 = node4a.transform.position;
			var pos1 = node4b.transform.position;
			var rot0 = node4a.transform.rotation;
			var rot1 = node4b.transform.rotation;
			
			var ft = (float)(t - t4a) / (t4b - t4a);
			
			cameraObject.transform.position = pos0.Add(pos1.Sub(pos0).Mul(ft));
			cameraObject.transform.rotation = Quaternion.Slerp(rot0, rot1, ft);

		} else if (t >= t5a && t < t5b) {
			var pos0 = node5a.transform.position;
			var pos1 = node5b.transform.position;
			var rot0 = node5a.transform.rotation;
			var rot1 = node5b.transform.rotation;
			
			var ft = (float)(t - t5a) / (t5b - t5a);
			
			cameraObject.transform.position = pos0.Add(pos1.Sub(pos0).Mul(ft));
			cameraObject.transform.rotation = Quaternion.Slerp(rot0, rot1, ft);
			
		} else if (t >= t6a && t < t6b) {
			var pos0 = node6a.transform.position;
			var pos1 = node6b.transform.position;
			var rot0 = node6a.transform.rotation;
			var rot1 = node6b.transform.rotation;
			
			var ft = (float)(t - t6a) / (t6b - t6a);
			
			cameraObject.transform.position = pos0.Add(pos1.Sub(pos0).Mul(ft));
			cameraObject.transform.rotation = Quaternion.Slerp(rot0, rot1, ft);
		} else if (t >= t7a && t < t7b) {
			var pos0 = node7a.transform.position;
			var pos1 = node7b.transform.position;
			var rot0 = node7a.transform.rotation;
			var rot1 = node7b.transform.rotation;
			
			var ft = (float)(t - t7a) / (t7b - t7a);
			
			cameraObject.transform.position = pos0.Add(pos1.Sub(pos0).Mul(ft));
			cameraObject.transform.rotation = Quaternion.Slerp(rot0, rot1, ft);

		} else {
			cameraObject.transform.position = origPos;
			cameraObject.transform.rotation = origRot;
		}

	}



}
