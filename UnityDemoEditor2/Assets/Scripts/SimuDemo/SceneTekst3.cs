using UnityEngine;
using System.Collections.Generic;
using Vector3 = f.Vector3;

public class SceneTekst3 : MonoBehaviour {

	public DemoTimeline tl;
	public SimuDemo demo;
	public FFont font;

	public GameObject cameraObject;

	public Material letterMat;

	[Multiline]
	public string text;

	private List<Vector3> path;
	bool inited = false;

	void Awake() {
		inited = true;
		path = GeneratePath();

		//GameObject container = new GameObject("letters");
		//container.transform.parent = this.transform;

		float x = 0;
		for(int i = 0; i < text.Length; i++) {
			char c = text[i];
			var letter = font.GetLetter(c);
			if (letter != null) {
				x += letter.xOffset0 * 1.25f;
				var obj = (GameObject)GameObject.Instantiate(letter.Obj);
				obj.GetComponent<MeshRenderer>().sharedMaterials = new Material[] { letterMat };
				//obj.transform.parent = container.transform;
				obj.transform.parent = this.transform;

				var p0 = GetPathPos(path, x);
				var p1 = GetPathPos(path, x + 0.1f);
				var d = Vector3.Normalized(p1.Sub(p0));

				obj.transform.position = GetPathPos(path, x);
				obj.transform.right = d;

				x += letter.xOffset1 * 1.25f;
			} else {
				x += font.SpaceSpacing * 1.25f;
			}
		}

		//MeshBatcher.Batch(container);

	}

	void Update() {
		//if (!inited) {
		//	Start();
		//}
		var t = tl.T() - demo.TimeSceneTekst3;

		var x = t * 0.01f;

		var p0 = GetPathPos(path, x);
		var p1 = GetPathPos(path, x + 0.1f);
		var p2 = GetPathPos(path, x + Mathf.Sin(t * 0.01f));
		var d = Vector3.Normalized(p1.Sub(p0));

		cameraObject.transform.position = p0;
		cameraObject.transform.right = d;
		var f = cameraObject.transform.forward;


		var b = tl.Beat();
		var m = 1.0f - b * 1.5f;
		if (m < 0) m = 0;
		var yoff = Mathf.Sin(b*10) * m * 0.5f;

		cameraObject.transform.position = p0.Sub(f.Mul(4.0f)).Add((new Vector3(0,0,0)).Sub(cameraObject.transform.up.Mul(yoff)));

	}

	List<Vector3> GeneratePath() {
		var sentences = GetSentences(text);
		var npath = new List<Vector3>();

		Vector3 p = new Vector3(0, 0, 0);
		Vector3 dir = new Vector3(1,0,0);
		for(int i = 0; i < 160; i++) {
			p = p.Add(dir.Mul(5.0f));
			var rnd = new Vector3(Random.Range(-1.0f,1.0f),Random.Range(-1.0f,1.0f),Random.Range(-1.0f,1.0f));
			dir = Vector3.Normalized(dir.Add(Vector3.Normalized(rnd.Mul(0.25f))));
			npath.Add(p);
		}
		return npath;
	}

	Vector3 GetPathPos(List<Vector3> path, float p) {
		if (p < 0) {
			return path[0];
		}
		for(int i = 1; i < path.Count; i++) {
			var E = path[i].Sub(path[i-1]);
			var e = Vector3.Magnitude(E);
			if (p < e) {
				//return path[i-1].Add(E.Mul(p/e));
				var p0 = i > 1 ? path[i-2] : path[0];
				var p1 = path[i-1];
				var p2 = path[i];
				var p3 = i < path.Count - 1 ? path[i+1] : path[i];
				return catmull(p0, p1, p2, p3, p/e);
			}
			p -= e;
		}

		return path[path.Count-1];
	}

	Vector3 catmull(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, float t) {
		return new Vector3(
			catmullf(p0.x, p1.x, p2.x, p3.x, t),
			catmullf(p0.y, p1.y, p2.y, p3.y, t),
			catmullf(p0.z, p1.z, p2.z, p3.z, t));
	}

	float catmullf(float p0, float p1, float p2, float p3, float t) {
		return p1 + 0.5f * ((-p0 + p2) + 
		                    (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t + 
		                    (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t) * t;
	}

	List<string> GetSentences(string txt) {
		List<string> s = new List<string>();
		int N = 0;
		string sentence = "";
		while (N < txt.Length) {
			var c = txt[N];
			if (c == '\n') {
				while (txt[N] == '\n') {
					N++;
				}
				s.Add(sentence);
				sentence = "";
				c = txt[N];
			}
			sentence += c;
			N++;
		}
		s.Add(sentence);
		return s;
	}

}
