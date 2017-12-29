using System.Reflection;
using UnityEngine;
using System.Collections;

[RequireComponent(typeof(Camera))]
public class RenderCameraToTexture : MonoBehaviour {
	private static Texture2D tex2D;
	private static Texture tex;

	public RenderTextureContainer Target;	

	void OnPostRender() {
		Capture();
	}

	void Capture() {
		var rect = this.camera.rect;
		if (Target.RenderTexture == null) {
			Target.RenderTexture = new Texture2D((int)(Screen.width * rect.width), (int)(Screen.height * rect.height), TextureFormat.RGB24, false);
		}
		Debug.Log ("" + rect.width + ", " + rect.height);
		Target.RenderTexture.ReadPixels(new Rect( 0f, 0f, Screen.width * rect.width, Screen.height * rect.height ), 0, 0, false);		
		Target.RenderTexture.Apply(false);
	}
}