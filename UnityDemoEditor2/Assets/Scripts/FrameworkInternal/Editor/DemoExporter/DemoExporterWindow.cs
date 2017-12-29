using UnityEngine;
using UnityEditor;
using System.Collections;

namespace f.DemoExporter {
	public class DemoExporterWindow : EditorWindow {

		DemoExporter de = new DemoExporter();

		GameObject rootObject;

		[MenuItem("f/f")]
		static void OpenWindow() {
			var exporterWindow = EditorWindow.GetWindow<DemoExporterWindow> ();
		}

		void OnGUI() {

			if (rootObject == null) {
				var demoObj = (GameObject)GameObject.Find("Demo");
				if (demoObj != null) {
					rootObject = demoObj;
				}
			}

			EditorGUILayout.LabelField("Root Object");
			rootObject = (GameObject)EditorGUILayout.ObjectField(rootObject, typeof(GameObject), true);

			if (GUILayout.Button ("EXPORT")) {
				de.Export(rootObject);
			}
		}


}
}
