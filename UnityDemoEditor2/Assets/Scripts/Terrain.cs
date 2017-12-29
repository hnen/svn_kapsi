using UnityEngine;
using System.Collections.Generic;
using System.Linq;
using Vector3 = f.Vector3;

public class Terrain : MonoBehaviour {

	private double pbase = 10.0;
	private const int QUAL = 50;

	public Camera camera;
	public float planetRadius = 6.778f;
	//public double height; 
	public float height = 0.0f;
	public int scale = 0;

	private float genH;

	public MeshFilter mf;

	// farplane dist x = sqrt((r+h)^2-r^2) = sqrt(h^2+2rh)
	// visible planet half-angle: atan (x/r)

	void Start() {
		Generate();
	}

	void Update() {
		if (genH != this.height) {
			Generate();
		}
	}
	
	void Generate() {
		double H = System.Math.Pow(pbase, height);
		double R = System.Math.Pow(pbase, planetRadius);
		double x = System.Math.Sqrt(H*H + 2.0 * R*H);
		double a = System.Math.Atan(x/R);
		double l = a * R;

		this.genH = height;

		if (mf.sharedMesh != null) {
			GameObject.Destroy(mf.sharedMesh);
		}

		double r = R * System.Math.Sin(a);

		this.scale = (int)System.Math.Floor(System.Math.Log10(r));
		var scaleF = System.Math.Pow(10, -scale);

		camera.farClipPlane = (float)(x * scaleF);
		camera.transform.position = new Vector3(0, 0, -(float)(H * scaleF));
		camera.nearClipPlane = Mathf.Abs(camera.transform.position.z) * 0.5f;
		mf.sharedMesh = GenMesh((float)(r * scaleF), (float)(R * scaleF));
	}

	Mesh GenMesh(float l, float R) {
		// Mathf.Sqrt(R*R-l*l)

		Vector3 I = new Vector3(1,0,0);
		Vector3 J = new Vector3(0,1,0);
		Vector3 K = new Vector3(0,0,-1);
		List <Vector3> verts = new List<Vector3>();
		for(int y = -QUAL; y <= QUAL; y++) {
			for(int x = -QUAL; x <= QUAL; x++) {
				var p2 = I.Mul((float)y / QUAL * l).Add(J.Mul((float)x / QUAL * l));
				if (Mathf.Log10(R) > -2) {
					if (Vector3.Magnitude(p2) > R) {
						var p3 = Vector3.Normalized(p2).Mul(R - (Vector3.Magnitude(p2) - R));
						float z = -Mathf.Sqrt((R*R) - p3.SqrMagnitude) - R;
						verts.Add(p2.Add(K.Mul(z)));
					} else {
						float z = Mathf.Sqrt((R*R) - p2.SqrMagnitude) - R;
						verts.Add(p2.Add(K.Mul(z)));
					}
				} else {
					verts.Add(p2);
				}
			}
		}

		List<int> inds = new List<int>();
		for(int y = -QUAL; y < QUAL; y++) {
			for(int x = -QUAL; x < QUAL; x++) {
				int y0 = (y+QUAL) * (2*QUAL+1);
				int y1 = (y+QUAL+1) * (2*QUAL+1);
				int x0 = x+QUAL;
				int x1 = x+QUAL+1;
				inds.Add(y0 + x0);
				inds.Add(y0 + x1);
				inds.Add(y1 + x1);

				inds.Add(y1 + x1);
				inds.Add(y1 + x0);
				inds.Add(y0 + x0);
			}
		}

		Mesh m = new Mesh();
		m.vertices = verts.ToUnityArray();
		m.triangles = inds.ToArray();
		m.RecalculateNormals();

		return m;
	}

}
