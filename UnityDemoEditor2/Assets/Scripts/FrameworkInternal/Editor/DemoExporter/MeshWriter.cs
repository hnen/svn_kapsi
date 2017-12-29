using UnityEngine;
using System.Collections;
using System.IO;


public class MeshWriter {

	public static void WriteMesh(Mesh mesh, string fileName) {
		FileStream fs = new FileStream(fileName, File.Exists(fileName) ? FileMode.Truncate : FileMode.CreateNew);
		BinaryWriter bw = new BinaryWriter (fs);

		bw.Write (mesh.vertexCount);
		bw.Write ("POS");
		foreach(var p in mesh.vertices) {
			bw.Write(-p.x);
			bw.Write(-p.y);
			bw.Write(-p.z);
		}
		if (mesh.normals != null) {
			bw.Write ("NRM");
			foreach (var p in mesh.normals) {
					bw.Write (-p.x);
					bw.Write (-p.y);
					bw.Write (-p.z);
			}
		}
		if (mesh.uv != null && mesh.uv.Length == mesh.vertexCount) {
			bw.Write ("UV0");
			foreach (var p in mesh.uv) {
				bw.Write (p.x);
				bw.Write (p.y);
			}
		}

		bw.Write ("IND");

		bw.Write(mesh.subMeshCount);
		for (int i = 0; i < mesh.subMeshCount; i++) {
			var inds = mesh.GetIndices(i);
			bw.Write(inds.Length);
			foreach(var j in inds) {
				bw.Write(j);
			}
		}

		Debug.Log(string.Format("Wrote mesh '{0}' to {1}", mesh.name, fileName));

		fs.Close();
	}



}
