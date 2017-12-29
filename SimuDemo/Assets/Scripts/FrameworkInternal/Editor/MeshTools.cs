using UnityEngine;
using UnityEditor;
using System.Collections.Generic;

public class MeshTools {

	class Edge {
		public int i0, i1;
		public int t0, t1;

		public Edge(int i0, int i1) {
			if (i0 < i1) {
				this.i0 = i0;
				this.i1 = i1;
			} else {
				this.i0 = i1;
				this.i1 = i0;
			}
			t0 = -1;
			t1 = -1;
		}

		public override bool Equals(object a) {
			if (a is Edge) {
				var ea = (Edge)a;
				return ea.i0 == this.i0 && ea.i1 == this.i1;
			} else {
				return false;
			}
		}

		public override int GetHashCode() {
			return i0 * 23121 + i1 * 2311;
		}

	}


	public static void SeparateSharpCorners(int [] inds, Vector3 [] verts, float thresholdAngle, out int [] out_inds, out Vector3 [] out_verts) {

		Dictionary<Edge, int> edges = new Dictionary<Edge, int>();
		List<Edge> edgeContainer = new List<Edge>();

		for(int i = 0; i < inds.Length; i += 3) {
			var e0 = new Edge(inds[i+0], inds[i+1]);
			var e1 = new Edge(inds[i+1], inds[i+2]);
			var e2 = new Edge(inds[i+2], inds[i+0]);
			if (!edges.ContainsKey(e0)) {
				e0.t0 = i/3;
				edges.Add(e0, edgeContainer.Count);
				edgeContainer.Add(e0);
			} else {
				edgeContainer[edges[e0]].t1 = i/3;
			}
			if (!edges.ContainsKey(e1)) {
				e1.t0 = i/3;
				edges.Add(e1, edgeContainer.Count);
				edgeContainer.Add(e1);
			} else {
				edgeContainer[edges[e1]].t1 = i/3;
			}
			if (!edges.ContainsKey(e2)) {
				e2.t0 = i/3;
				edges.Add(e2, edgeContainer.Count);
				edgeContainer.Add(e2);
			} else {
				edgeContainer[edges[e2]].t1 = i/3;
			}
		}

		HashSet<Edge> separateEdges = new HashSet<Edge>();
		for (int i = 0; i < edges.Count; i++) {
			var edge = edgeContainer[i];
			if (edge.t0 == -1) {
				throw new UnityException("shenanigans");
			}
			if (edge.t1 == -1) {
				continue;
			}
			var pa0 = verts[inds[edge.t0*3+0]];
			var pa1 = verts[inds[edge.t0*3+1]];
			var pa2 = verts[inds[edge.t0*3+2]];
			var pb0 = verts[inds[edge.t1*3+0]];
			var pb1 = verts[inds[edge.t1*3+1]];
			var pb2 = verts[inds[edge.t1*3+2]];

			var va0 = pa1 - pa0;
			var va1 = pa2 - pa0;
			var vb0 = pb1 - pb0;
			var vb1 = pb2 - pb0;
			var n0 = Vector3.Cross(va0, va1).normalized;
			var n1 = Vector3.Cross(vb0, vb1).normalized;

			var ang = Mathf.Acos(Vector3.Dot(n0, n1));
			if (ang > thresholdAngle) {
				separateEdges.Add(edge);
			}
		}

		Debug.Log(string.Format("Separating {0}/{1} edges", separateEdges.Count, edges.Count));

		List<Vector3> nverts = new List<Vector3>(verts);

		List<int> ninds = new List<int>();

		for (int i = 0; i < inds.Length; i += 3) {
			var e0 = new Edge(inds[i+0], inds[i+1]);
			var e1 = new Edge(inds[i+1], inds[i+2]);
			var e2 = new Edge(inds[i+2], inds[i+0]);

			int ni0 = inds[i+0];
			int ni1 = inds[i+1];
			int ni2 = inds[i+2];

			bool b0 = separateEdges.Contains(e0);
			bool b1 = separateEdges.Contains(e1);
			bool b2 = separateEdges.Contains(e2);

			if ((b0 && b1) || (b0 && b2) || (b1 && b2)) {
				ni0 = nverts.Count;
				nverts.Add(verts[inds[i+0]]);
				ni1 = nverts.Count;
				nverts.Add(verts[inds[i+1]]);
				ni2 = nverts.Count;
				nverts.Add(verts[inds[i+2]]);
			} else {
				if (b0) {
					ni0 = nverts.Count;
					nverts.Add(verts[inds[i+0]]);
					ni1 = nverts.Count;
					nverts.Add(verts[inds[i+1]]);
				}
				if (b1) {
					ni1 = nverts.Count;
					nverts.Add(verts[inds[i+1]]);
					ni2 = nverts.Count;
					nverts.Add(verts[inds[i+2]]);
				}
				if (b2) {
					ni2 = nverts.Count;
					nverts.Add(verts[inds[i+2]]);
					ni0 = nverts.Count;
					nverts.Add(verts[inds[i+0]]);
				}
			}

			ninds.Add(ni0);
			ninds.Add(ni1);
			ninds.Add(ni2);
		}


		out_verts = nverts.ToArray();
		out_inds = ninds.ToArray();
	}

	public static void MergeDoubleVertices(int [] inds, Vector3 [] verts, out int [] out_inds, out Vector3 [] out_verts) {
		Dictionary<int, int> mergedVerts = new Dictionary<int, int>();
		
		for(int i = 0; i < verts.Length; i++) {
			if (mergedVerts.ContainsKey(i)) {
				continue;
			}
			var v0 = verts[i];
			for(int j = i+1; j < verts.Length; j++) {
				if (mergedVerts.ContainsKey(j)) {
					continue;
				}
				var v1 = verts[j];
				var d = (v0-v1).sqrMagnitude;
				if (d < 0.001f * 0.001f) {
					mergedVerts.Add(j, i);
				}
			}
		}
		
		List<Vector3> nverts = new List<Vector3>();
		for(int i = 0; i < verts.Length; i++) {
			nverts.Add(verts[i]);
		}
		
		int [] ninds = new int[inds.Length];
		for(int i = 0; i < inds.Length; i++) {
			if (mergedVerts.ContainsKey(inds[i])) {
				ninds[i] = mergedVerts[inds[i]];
			} else {
				ninds[i] = inds[i];
			}
		}
		
		out_verts = nverts.ToArray();
		out_inds = ninds;
	}

	
	public static Dictionary<int, HashSet<int>> SolveVertexConnections(int [] inds) {
		Dictionary<int, HashSet<int>> vertConns = new Dictionary<int, HashSet<int>>();
		for (int i = 0; i < inds.Length; i += 3) {
			var i0 = inds[i+0];
			var i1 = inds[i+1];
			var i2 = inds[i+2];
			if (!vertConns.ContainsKey(i0)) {
				vertConns.Add(i0, new HashSet<int>());
			}
			if (!vertConns.ContainsKey(i1)) {
				vertConns.Add(i1, new HashSet<int>());
			}
			if (!vertConns.ContainsKey(i2)) {
				vertConns.Add(i2, new HashSet<int>());
			}
			vertConns[i0].Add(i1);
			vertConns[i0].Add(i2);
			vertConns[i1].Add(i0);
			vertConns[i1].Add(i2);
			vertConns[i2].Add(i0);
			vertConns[i2].Add(i1);
		}
		return vertConns;
	}

}
