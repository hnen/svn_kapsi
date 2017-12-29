﻿using UnityEngine;
using UnityEditor;
using System.Collections;
using System.IO;

public class TexGen : EditorWindow {

	enum Tex {
		Kolmio
	}

	Tex textype;
	int w = 512, h = 512;

	[MenuItem("f/texgen")]
	static void OpenWindow() {
		var exporterWindow = EditorWindow.GetWindow<TexGen> ();
	}

	void OnGUI() {
		textype = (Tex)EditorGUILayout.EnumPopup("Kyrpeli: ", textype);

		EditorGUILayout.LabelField("width");
		w = EditorGUILayout.IntField(w);
		EditorGUILayout.LabelField("height");
		w = EditorGUILayout.IntField(h);

		if(GUILayout.Button("truut")) {
			Gen();
		}
	}

	void Gen() {
		Texture2D kuva = new Texture2D(w, h);

		Color [] pixels = null;
		if (textype == Tex.Kolmio) {
			pixels = GenKolmio(w,h);
		}

		kuva.SetPixels(pixels);
		kuva.Apply();

		var png = kuva.EncodeToPNG();
		File.WriteAllBytes("Assets/gentex.png", png);
	}

	Color [] GenKolmio(int w, int h) {
		Color [] cols = new Color[w*h];
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				// y(x) = kx + b
				// y(w/2) = h
				// y(w) = 0




				bool inside = y < 2*x*h/w && y < -2*h/w*x + 2*h;
				cols[y*w+x] = inside ? new Color(1,1,1,1) : new Color(0,0,0,0);
			}
		}
		return cols;
	}


}