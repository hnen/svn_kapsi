using UnityEngine;
using UnityEditor;
using System.Collections;
using System.Collections.Generic;
using System.IO;

namespace f.DemoExporter {

	public class ExportGameObject {
		public int ReferenceId;
		public GameObject GameObject;
		public List<ExportGameObject> children = new List<ExportGameObject>();
		public ExportGameObject parent;
	}

	public interface IObjectContainer {
		int GetObjectRefId(UnityEngine.Object obj);
		JsScript [] GetAllScripts();
	}

	public class DemoExporter : IObjectContainer {

		public const string BUILD_PATH = "Build";
		public const string JS_PATH = "Build/js";
		public const string DATA_PATH = "Build/data";

		private int refIdCounter = 0;
		private Dictionary<int, UnityEngine.Object> refIdToObject;
		private Dictionary<UnityEngine.Object, int> objectToRefId;
		private List<JsScript> scripts;

		public int GetObjectRefId(UnityEngine.Object obj) {
			if (objectToRefId.ContainsKey(obj)) {
				return objectToRefId[obj];
			} else {
				return -1;
			}
		}

		public JsScript [] GetAllScripts() {
			return scripts.ToArray();
		}

		public void Export(GameObject rootObject) {

			refIdCounter = 0;
			refIdToObject = new Dictionary<int, Object>();
			objectToRefId = new Dictionary<Object, int>();
			scripts = new List<JsScript>();

			CreateDirectory();

			var rootObj = GatherScene(rootObject, null);

			AssetManager assetManager = new AssetManager();

			SceneExporter sceneExporter = new SceneExporter();
			sceneExporter.Export(string.Format("{0}/scene.js", JS_PATH), assetManager, rootObj, this);

			AssetExporter assetExporter = new AssetExporter();
			assetExporter.Export("assets_data.js", JS_PATH + "/", DATA_PATH + "/", assetManager);

			ScriptExporter.Export(string.Format("{0}/scripts.js", JS_PATH), assetManager);
			

			CopyEngine();

			IndexHtmlGenerator htmlGen = new IndexHtmlGenerator();
			htmlGen.Export(string.Format("{0}/index.html", BUILD_PATH));

			OpenDirectory();
		}

		void CopyEngine() {
			foreach (var srcFile in Directory.GetFiles("JsEngine")) {
				if (!srcFile.EndsWith("DS_Store")) {
					string fileName = srcFile.Substring("JsEngine/".Length);
					string dstFile = JS_PATH + "/" + fileName;
					if (File.Exists(dstFile)) {
						File.Delete(dstFile);
					}
					File.Copy(srcFile, dstFile, true);
				}
			}
		}

		public static ExportGameObject PrefabToExportGameObject(GameObject rootObject, ExportGameObject parent) {
			var root = new ExportGameObject();
			root.ReferenceId = -1;
			root.GameObject = rootObject;
			root.parent = parent;

			List<ExportGameObject> children = new List<ExportGameObject>();
			foreach(Transform tf in rootObject.transform) {
				children.Add(PrefabToExportGameObject(tf.gameObject, root));
			}
			
			root.children = children;
			return root;
		}


		ExportGameObject GatherScene(GameObject rootObject, ExportGameObject parent) {
			var root = new ExportGameObject();
			root.ReferenceId = this.refIdCounter++;
			root.GameObject = rootObject;
			root.parent = parent;

			objectToRefId.Add(rootObject, root.ReferenceId);
			refIdToObject.Add(root.ReferenceId, rootObject);

			AssignRefIdsToComponents(rootObject);

			List<ExportGameObject> children = new List<ExportGameObject>();
			foreach(Transform tf in rootObject.transform) {
				children.Add(GatherScene(tf.gameObject, root));
			}

			root.children = children;
			return root;
		}

		void AssignRefIdsToComponents(GameObject rootObject) {
			foreach(var cmp in rootObject.GetComponents<Component>()) {
				int refId = this.refIdCounter++;
				objectToRefId.Add(cmp, refId);
				refIdToObject.Add(refId, cmp);
				if (cmp is MonoBehaviour) {
					var script = JsCodeExtractor.GetScript(cmp as MonoBehaviour);
					if (script != null) {
						scripts.Add(script);
					}
				}
			}
		}

		static string DebugString(ExportGameObject ego, int indent = 0) {
			string ret = "";
			for (int i = 0; i < indent; i++) {
				ret += "  ";
			}
			if (ego.GameObject == null) {
				ret += "<ROOT>\n";
			} else {
				ret += ego.GameObject.name + "\n";
			}
			foreach(var ch in ego.children) {
				ret += DebugString(ch, indent+1);
			}
			return ret;
		}

		void CreateDirectory() {
			if (!System.IO.Directory.Exists(BUILD_PATH)) {
				System.IO.Directory.CreateDirectory(BUILD_PATH);
			}
			if (!System.IO.Directory.Exists(DATA_PATH)) {
				System.IO.Directory.CreateDirectory(DATA_PATH);
			}
			if (!System.IO.Directory.Exists(JS_PATH)) {
				System.IO.Directory.CreateDirectory(JS_PATH);
			}
		}

		void OpenDirectory() {
			System.Diagnostics.Process.Start(BUILD_PATH);
		}

	}
}