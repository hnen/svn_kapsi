using UnityEngine;
using System.Collections;

[RequireComponent(typeof(Camera))]
public class RenderTexture : MonoBehaviour {

	public Material mat;
	public RenderTextureContainer rtContainer;

	public void OnPostRender () {
		mat.SetTexture("_MainTex", rtContainer.RenderTexture);
		GL.PushMatrix();
		GL.LoadOrtho();
		for (var i = 0; i < mat.passCount; ++i) {
			mat.SetPass(i);
			GL.Begin(GL.QUADS);
			GL.TexCoord2(0,1);
			GL.Vertex3(0.0f, 1.0f, 0.1f);
			GL.TexCoord2(1,1);
			GL.Vertex3(1.0f, 1.0f, 0.1f);
			GL.TexCoord2(1,0);
			GL.Vertex3(1.0f, 0.0f, 0.1f);
			GL.TexCoord2(0,0);
			GL.Vertex3(0.0f, 0.0f, 0.1f);
			GL.End();
		}
		GL.PopMatrix();
	}
}
