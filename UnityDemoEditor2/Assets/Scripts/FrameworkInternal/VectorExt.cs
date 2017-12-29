using System;
using System.Collections.Generic;
using System.Linq;

public static class VectorExt {

	public static UnityEngine.Vector3[] ToUnityArray(this List<f.Vector3> arr) {
		return arr.Select(a => (UnityEngine.Vector3)a).ToArray();
	}

	public static UnityEngine.Vector3 Mul(this UnityEngine.Vector3 v, float m) {
		return v*m;
	}
	
	public static UnityEngine.Vector3 Add(this UnityEngine.Vector3 v, UnityEngine.Vector3 a) {
		return v+a;
	}
	public static UnityEngine.Vector3 Sub(this UnityEngine.Vector3 v, UnityEngine.Vector3 a) {
		return v-a;
	}

}


namespace f {

	[Serializable]
	public class Vector3 {

		public float x, y, z;

		public Vector3() {}
		public Vector3(float x, float y, float z) {
			this.x = x;
			this.y = y;
			this.z = z;
		}

		public Vector3 Add(Vector3 other) {
			return new Vector3(this.x + other.x, this.y + other.y, this.z + other.z);
		}
		public Vector3 Sub(Vector3 other) {
			return new Vector3(this.x - other.x, this.y - other.y, this.z - other.z);
		}

		public Vector3 Mul(float a) {
			return new Vector3(this.x * a, this.y * a, this.z * a);
		}

		public static float Magnitude(Vector3 v) {
			return UnityEngine.Mathf.Sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
		}

		public float SqrMagnitude {
			get {
				return x*x + y*y + z*z;
			}
		}

		public static Vector3 Normalized(Vector3 v) {
			return v.Mul (1.0f / UnityEngine.Mathf.Sqrt(v.x*v.x + v.y*v.y + v.z*v.z));
		}

		public static implicit operator UnityEngine.Vector3(f.Vector3 a) {
			return new UnityEngine.Vector3(a.x, a.y, a.z);
		}
		public static implicit operator f.Vector3(UnityEngine.Vector3 a) {
			return new f.Vector3(a.x, a.y, a.z);
		}

	}

}
