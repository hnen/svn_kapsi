using UnityEditor;
using UnityEngine;
using System.Collections;

public class FontGenWindow : EditorWindow {

	private Mesh sourceMesh;
	private Material mat;
	private int letterCount;
	private string lettersString;
	private string meshPath;

	[MenuItem("f/FontGen")]
	public static void OpenWindow() {
		var window = ScriptableObject.CreateInstance<FontGenWindow>();
		window.Show();
	}


	void OnGUI() {
		var pmesh = sourceMesh;
		sourceMesh = (Mesh)EditorGUILayout.ObjectField("Source Mesh: ", sourceMesh, typeof(Mesh), false);
		mat = (Material)EditorGUILayout.ObjectField("Assign Material: ", mat, typeof(Material), false);

		EditorGUILayout.LabelField("Mesh path:");
		meshPath = EditorGUILayout.TextField(meshPath);

		if (this.sourceMesh != pmesh) {
			if (sourceMesh != null) {
				this.letterCount = FontGen.CountNumberOfLetters(this.sourceMesh);
				lettersString = "";
			} else {
				this.letterCount = 0;
			}
		}

		if (this.letterCount > 0) {
			GUILayout.Label(string.Format("Number of letters in mesh detected: {0}", this.letterCount));

			GUILayout.Label(string.Format("Letters: "));
			lettersString = GUILayout.TextField(lettersString);
			GUILayout.Label(string.Format("{0}/{1}", lettersString.Length, this.letterCount));

			if(GUILayout.Button("GENERATE")) {
				FontGen.GenerateLetters(sourceMesh, lettersString, meshPath, mat);
			}

		} else {

		}

	}

}
