using UnityEngine;
using UnityEditor;
using System.Collections.Generic;
using System.IO;

public class TextureWriter {
	public static void WriteTexture(Texture2D texture, string fileName) {
		var png = texture.EncodeToPNG();
		File.WriteAllBytes(fileName, png);
	}

}
