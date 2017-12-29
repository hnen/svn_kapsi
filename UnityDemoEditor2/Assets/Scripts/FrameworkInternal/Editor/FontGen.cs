using UnityEngine;
using UnityEditor;
using System.Collections.Generic;

public class FontGen : EditorWindow {


	[System.Serializable]
	public class LetterMesh {
		public Mesh mesh;
		public float x_offset0;
		public float x_offset1;
		public char letter;
	}

	public class Group {
		public HashSet<int> Vertices;
		public Bounds Bounds;
		public float Offset0;
		public float Offset1;
	}


	public static int CountNumberOfLetters(Mesh fontMesh) {
		var inds = fontMesh.GetIndices(0);
		var verts = fontMesh.vertices;
		
		MeshTools.MergeDoubleVertices(inds, verts, out inds, out verts);
		
		var vertConns = MeshTools.SolveVertexConnections(inds);
		var groups = GetVertexGroups(inds, vertConns);
		GetGroupBounds(groups, verts);
		MergeOverlappingGroups(groups, 0.9f);

		return groups.Count;
	}

	public static void GenerateLetters(Mesh FontMesh, string letters, string meshPath, Material mat) {
		var inds = FontMesh.GetIndices(0);
		var verts = FontMesh.vertices;

		MeshTools.MergeDoubleVertices(inds, verts, out inds, out verts);

		var vertConns = MeshTools.SolveVertexConnections(inds);
		var groups = GetVertexGroups(inds, vertConns);
		GetGroupBounds(groups, verts);

		MergeOverlappingGroups(groups, 0.9f);

		GetGroupBounds(groups, verts);
		var meshes = GenerateMeshes(groups, verts, inds, letters);

		if (!meshPath.EndsWith("/") && !meshPath.EndsWith("\\")) {
			meshPath += "/";
		}

		GameObject container = new GameObject("Font");
		var fontObj = container.AddComponent<FFont>();
		List<FFont.LetterObj> lobjs = new List<FFont.LetterObj>();
		int N = 0;
		foreach(var mesh in meshes) {
			AssetDatabase.CreateAsset(mesh.mesh, string.Format("{0}letter_{1}.mesh", meshPath, N));
			GameObject letter = new GameObject("" + mesh.letter);
			letter.transform.parent = container.transform;
			var mr = letter.AddComponent<MeshRenderer>();
			var mf = letter.AddComponent<MeshFilter>();
			mf.sharedMesh = mesh.mesh;
			mr.sharedMaterial = mat;
			lobjs.Add(new FFont.LetterObj {
				Obj = letter,
				Letter = mesh.letter,
				xOffset0 = mesh.x_offset0,
				xOffset1 = mesh.x_offset1
			});
			N++;
		}

		fontObj.Letters = lobjs.ToArray();

		AssetDatabase.SaveAssets();

		//this.lmeshes = meshes;
	}



	static LetterMesh [] GenerateMeshes(List<Group> groups, Vector3 [] vertices, int [] origTriangles, string letters) {
		groups.Sort((a,b) => {
			var x_a = a.Bounds.center.x;
			var x_b = b.Bounds.center.x;
			return -x_a.CompareTo(x_b);
		});

		LetterMesh [] meshes = new LetterMesh[groups.Count];
		int i = 0;
		float prev_x = 0;
		for (int j = 0; j < groups.Count; j++) {
			var group = groups[j];
			var lmesh = GenerateMesh(group, vertices, origTriangles);
			lmesh.RecalculateBounds();
			meshes[i] = new LetterMesh {
				mesh = lmesh, 
				x_offset0 = -(group.Bounds.center.x - prev_x),
				x_offset1 = -(group.Bounds.min.x - group.Bounds.center.x),
				letter = letters[j]
			};
			prev_x = group.Bounds.min.x;
			i++;
		}
		return meshes;
	}

	static Mesh GenerateMesh(Group group, Vector3 [] vertices, int [] origTriangles) {
		HashSet<int> triangles = new HashSet<int>();
		foreach(var v in group.Vertices) {
			for(int i = 0; i < origTriangles.Length; i++) {
				if (origTriangles[i] == v) {
					triangles.Add(i/3);
				}
			}
		}

		int L = 0;
		Dictionary<int, int> usedVertices = new Dictionary<int, int>();
		List<Vector3> nverts = new List<Vector3>();

		int [] inds = new int[triangles.Count*3];
		int j = 0;
		foreach(var t in triangles) {
			var v0 = origTriangles[t*3+0];
			var v1 = origTriangles[t*3+1];
			var v2 = origTriangles[t*3+2];
			if (usedVertices.ContainsKey(v0)) {
				inds[j++] = usedVertices[v0];
			} else {
				inds[j++] = L;
				usedVertices.Add(v0, L);
				nverts.Add(vertices[v0]);
				L++;
			}
			if (usedVertices.ContainsKey(v1)) {
				inds[j++] = usedVertices[v1];
			} else {
				inds[j++] = L;
				usedVertices.Add(v1, L);
				nverts.Add(vertices[v1]);
				L++;
			}
			if (usedVertices.ContainsKey(v2)) {
				inds[j++] = usedVertices[v2];
			} else {
				inds[j++] = L;
				usedVertices.Add(v2, L);
				nverts.Add(vertices[v2]);
				L++;
			}
		}

		for (int i = 0; i < nverts.Count; i++) {
			nverts[i] -= new Vector3(group.Bounds.center.x, 0, 0);
			nverts[i] = new Vector3(-nverts[i].x, nverts[i].y, -nverts[i].z);
		}

		Vector3 [] sverts;
		int [] sinds;
		MeshTools.SeparateSharpCorners(inds, nverts.ToArray(), 0.0f / 180.0f * Mathf.PI, out sinds, out sverts);

		var mesh = new Mesh();
		//mesh.vertices = nverts.ToArray();
		//mesh.SetTriangles(inds, 0);
		mesh.vertices = sverts;
		mesh.SetTriangles(sinds, 0);
		mesh.RecalculateNormals();
		return mesh;
	}

	static void MergeOverlappingGroups(List<Group> groups, float overlappingThreshold) {

		for(int i = 0; i < groups.Count; i++) {
			float ax0 = groups[i].Bounds.min.x;
			float ax1 = groups[i].Bounds.max.x;
			for(int j = 0; j < i; j++) {
				float bx0 = groups[j].Bounds.min.x;
				float bx1 = groups[j].Bounds.max.x;

				float cx0 = Mathf.Max(ax0, bx0);
				float cx1 = Mathf.Min(ax1, bx1);

				var a = cx1-cx0;
				if (a > 0) {
					var ca = a / (ax1-ax0);
					var cb = a / (bx1-bx0);
					if (ca >= overlappingThreshold || cb >= overlappingThreshold) {
						Debug.Log(string.Format("Merging {0} with {1}. a = {2}, ca = {3}, cb = {4}", i, j, a, ca, cb));
						foreach(var ivert in groups[i].Vertices) {
							groups[j].Vertices.Add(ivert);
						}
						groups.RemoveAt(i);
						i--;
					}
				}
			}
		}
	}

	static void GetGroupBounds(List<Group> groups, Vector3 [] positions) {
		for(int i = 0; i < groups.Count; i++) {
			GetGroupBounds(groups[i], positions);
		}
	}

	static void GetGroupBounds(Group group, Vector3 [] positions) {
		Vector3 min = new Vector3(float.MaxValue, float.MaxValue, float.MaxValue);
		Vector3 max = new Vector3(float.MinValue, float.MinValue, float.MinValue);
		foreach(var vert in group.Vertices) {
			min = Vector3.Min(min, positions[vert]);
			max = Vector3.Max(max, positions[vert]);
		}
		group.Bounds = new Bounds((min+max) * 0.5f, max - min);
	}

	static List<Group> GetVertexGroups(int [] inds, Dictionary<int, HashSet<int>> vertConns) {
		HashSet<int> availableVertices = new HashSet<int>();
		foreach(var vert in inds) {
			availableVertices.Add(vert);
		}
		List<Group> groups = new List<Group>();
		foreach(var vert in inds) {
			if (availableVertices.Contains(vert)) {
				HashSet<int> groupVerts = new HashSet<int>();
				Queue<int> bfs = new Queue<int>();
				bfs.Enqueue(vert);
				while (bfs.Count > 0) {
					var v = bfs.Dequeue();
					if (!groupVerts.Contains(v) && availableVertices.Contains(v)) {
						availableVertices.Remove(v);
						groupVerts.Add(v);
						foreach(var v0 in vertConns[v]) {
							bfs.Enqueue(v0);
						}
					}
				}
				groups.Add(new Group {
					Vertices = groupVerts
				});
			}
		}
		return groups;
	}


}

