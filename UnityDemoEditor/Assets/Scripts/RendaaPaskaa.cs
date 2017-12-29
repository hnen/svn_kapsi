using System.Collections.Generic;
using UnityEngine;


public class RendaaPaskaa : MonoBehaviour {
	//var paska : MeshFilter;
	//var src : RenderTextureContainer;
	//var numpaskas : int;

	public MeshFilter paska;
	public RenderTextureContainer src;
	public int numpaskas;

	void LateUpdate () {

		if (src.RenderTexture == null) return;

		if (paska.sharedMesh == null) {
			paska.sharedMesh = new Mesh();
		}

		var pixels = this.src.RenderTexture.GetPixels();
		
		var w = this.src.RenderTexture.width;
		var h = this.src.RenderTexture.height;
		
		var verts = new List<Vector3>();
		var uv = new List<Vector2>();
		var inds = new List<int>();
		var cols = new List<Color>();

		for (var i = 0; i < this.numpaskas; i++) {
			var x = Random.Range(0,w);
			var y = Random.Range(0,h);
			var pixel = pixels[y*w+x];
			if (pixel.r > 0.5f)	{
				cols.Add(pixel);
				cols.Add(pixel);
				cols.Add(pixel);
				cols.Add(pixel);

				uv.Add(new Vector3(1,0));
				uv.Add(new Vector3(1,1));
				uv.Add(new Vector3(0,1));
				uv.Add(new Vector3(0,0));

				Quad(new Vector2((float)x/w, (float)y/h), -new Vector2(x-w/2,y-h/2).normalized, verts, inds);
			}
		}


		paska.sharedMesh.Clear();
		var biverts = verts.ToArray();


		paska.sharedMesh.vertices = biverts;
		paska.sharedMesh.uv = uv.ToArray();
		//paska.sharedMesh.colors = cols.ToBuiltin(Color) as Color [];
		paska.sharedMesh.SetIndices(inds.ToArray(), MeshTopology.Triangles, 0);
		
	}

	void Quad(Vector2 p,Vector2 d, List<Vector3> verts, List<int> inds) {

		p -= new Vector2(0.5f, 0.5f);
		//p.y = -p.y;
		//d.x = -d.x;
		d.y *= 0.1f;

		p.x *= 1.9f;

		p.x *= 0.9f;
		p.y *= 1.0f;

	 	//d = new Vector2(-1,0); 

		var w = 0.4f;
		var h = 4.0f;
		
		var Y = -d;
		var X = new Vector2(d.y, -d.x);

		p *= 10.0f;

		var p0 = p + X*w * 0.5f;
		var p1 = p + X*w * 0.5f + Y*h;
		var p2 = p - X*w * 0.5f + Y*h;
		var p3 = p - X*w * 0.5f;

		//p0.x /= 16.0f / 9.0f;
		//p1.x /= 16.0f / 9.0f;
		//p2.x /= 16.0f / 9.0f;
//		p3.x /= 16.0f / 9.0f;
	
		verts.Add(p0);
		verts.Add(p1);
		verts.Add(p2);
		verts.Add(p3);
		
		inds.Add(verts.Count-4);
		inds.Add(verts.Count-3);
		inds.Add(verts.Count-2);
		
		inds.Add(verts.Count-2);
		inds.Add(verts.Count-1);
		inds.Add(verts.Count-4);
	}

}

