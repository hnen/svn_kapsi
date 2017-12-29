using UnityEngine;
using UnityEditor;
using System.Collections;
using System.IO;

namespace f.DemoExporter {
	public class SoxSpectrographConverter : EditorWindow {

		public Texture2D soxImage;

		[MenuItem("f/sox spectrogram to bin")]
		static void OpenWindow() {
			var exporterWindow = EditorWindow.GetWindow<SoxSpectrographConverter> ();
		}


		void OnGUI() {
			EditorGUILayout.BeginHorizontal();
			EditorGUILayout.LabelField("Sox Spectorgram: ");
			soxImage = (Texture2D)EditorGUILayout.ObjectField(soxImage, typeof(Texture2D), false);
			EditorGUILayout.EndHorizontal();
			if(GUILayout.Button("truut")) {
				Convert();
			}
		}

		void Convert() {
			var pixels = soxImage.GetPixels();
			int width = soxImage.width;
			int height = soxImage.height;

			//byte [][] data = new byte[width][height/2];
			byte [] data = new byte[width*height/2];

			for(int x = 0; x < width; x++) {
				int f = 0;
				for(int y = 0; y < height/2; y++) {
					float val = pixels[y*width+x].r;
					data[f*width+x] = (byte)(val * 255);
					f++;
				}
			}

			FileStream file = new FileStream("Assets/Audio/fft.bytes", File.Exists("Assets/Audio/fft.bytes") ? FileMode.Truncate : FileMode.CreateNew);
			BinaryWriter bw = new BinaryWriter (file);
			bw.Write(width);
			bw.Write(height/2);
			bw.Write(data);
			file.Flush();
			file.Close();
		}

	}
}
