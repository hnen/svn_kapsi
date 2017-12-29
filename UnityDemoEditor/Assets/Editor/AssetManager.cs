using UnityEngine;
using UnityEditor;
using System.Collections.Generic;
using System.Linq;

namespace f.DemoExporter {

	public class AssetManager {

		int idCnt = 0;
		Dictionary<UnityEngine.Object, int> ids = new Dictionary<Object, int>();

		public IEnumerable<KeyValuePair<int, UnityEngine.Object>> Assets {
			get {
				return ids.Select(a => new KeyValuePair<int, UnityEngine.Object>(a.Value, a.Key));
			}
		}

		public int NumAssets {
			get {
				return ids.Count;
			}
		}

		public string RegisterAsset(UnityEngine.Object obj) {
			if (!ids.ContainsKey(obj)) {
				ids.Add(obj, idCnt++);
			}
			return string.Format("{{__assetRef: \"{0}\"}}", ids[obj]);
		}

	}


}
