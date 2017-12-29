using UnityEngine;
using System.Collections.Generic;

public static class MeshBatcher {


	public static void Batch(GameObject go) {
		Material mat;
		var rnd = go.GetComponentInChildren<MeshRenderer>();
		mat = rnd.sharedMaterial;
		var mfs = go.GetComponentsInChildren<MeshFilter>(true);

		GameObject batched = new GameObject("batch");
		batched.transform.parent = go.transform;
		var nrnd = batched.AddComponent<MeshRenderer>();
		nrnd.sharedMaterial = mat;

		List<int> ninds = new List<int>();
		List<Vector3> nverts = new List<Vector3>();

		for(int i = 0; i < mfs.Length; i++) {
			var mf = mfs[i];
			var mesh = mf.sharedMesh;
			var verts = mesh.vertices;
			var inds = mesh.GetIndices(0);

			if (nverts.Count + verts.Length >= 65000) {
				Mesh nmesh = new Mesh();
				nmesh.vertices = nverts.ToArray();
				nmesh.SetIndices(ninds.ToArray(), MeshTopology.Triangles, 0);
				nmesh.RecalculateNormals();
				var nmf = batched.AddComponent<MeshFilter>();
				nmf.sharedMesh = nmesh;

				batched = new GameObject("batch");
				batched.transform.parent = go.transform;
				nrnd = batched.AddComponent<MeshRenderer>();
				nrnd.sharedMaterial = mat;

				ninds = new List<int>();
				nverts = new List<Vector3>();
			}

			var indOffs = nverts.Count;
			var localToWorld = mf.gameObject.transform.localToWorldMatrix;

			for (int j = 0; j < verts.Length; j++) {
				nverts.Add(localToWorld.MultiplyPoint(verts[j]));
			}
			for (int j = 0; j < inds.Length; j++) {
				ninds.Add(inds[j] + indOffs);
			}
		}

		Mesh nmesh0 = new Mesh();
		nmesh0.vertices = nverts.ToArray();
		nmesh0.SetIndices(ninds.ToArray(), MeshTopology.Triangles, 0);
		nmesh0.RecalculateNormals();
		var nmf0 = batched.AddComponent<MeshFilter>();
		nmf0.sharedMesh = nmesh0;

		for(int i = 0; i < mfs.Length; i++) {
			GameObject.DestroyImmediate(mfs[i].gameObject);
		}


	}


}
