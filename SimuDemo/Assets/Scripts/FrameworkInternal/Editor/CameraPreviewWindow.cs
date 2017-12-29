using UnityEngine;
using System.Collections.Generic;
using UnityEditor;
using System.Linq;

public class CameraPreviewWindow : EditorWindow {

	Camera camera = Camera.main;
	Camera [] prevCamera = null;
	RenderTexture renderTexture = null;
	bool lockCamera = false;

	[MenuItem("Window/Preview Selected Camera #c")]
	static void Init () {
		EditorWindow editorWindow = GetWindow(typeof(CameraPreviewWindow));
		editorWindow.autoRepaintOnSceneChange = true;
		editorWindow.Show();
		editorWindow.title = "Camera Preview";
	}
	
	void Awake () {
		CreateRenderTexture();
	}
	
	void Update () {
		SelectCamera();
		if(renderTexture.width != position.width && renderTexture.height != position.height) {
			if (renderTexture != null) {
				Graphics.SetRenderTarget(null);
				GameObject.DestroyImmediate(renderTexture);
			}
			CreateRenderTexture();
		}
	}
	
	void OnSelectionChange () {
		SelectCamera();
		CreateRenderTexture();
	}
	
	void CreateRenderTexture () {
		int width = (int)position.width;
		int height = (int)position.height;
		if (9*width > 16*height) {
			width = 16 * height / 9;
		} else {
			height = 9 * width / 16;
		}
		renderTexture = new RenderTexture(width, height, 32, RenderTextureFormat.ARGB32);
		renderTexture.name = "windowtexture";
	}

	void SelectCamera() {
		if (!lockCamera) {
			var obj = Selection.activeGameObject;
			if (obj != null && obj.GetComponent<Camera>() != null) {
				camera = obj.GetComponent<Camera>();
			}

			Transform root = camera.transform;
			while(root.parent != null) {
				root = root.parent;
			}
			var cameras = new List<Camera>(root.GetComponentsInChildren<Camera>().Where(a => a.depth < camera.depth));
			cameras.Sort((a,b) => a.depth.CompareTo(b.depth));
			this.prevCamera = cameras.ToArray();
		}
	}

	void RenderCamera(bool wholeChain) {
		if (wholeChain) {
			foreach(var cam in this.prevCamera) {
				if (cam.targetTexture == null) {
					continue;
				}
				cam.Render();
				if (cam.GetComponent<RenderTextureToScreen>() != null) {
					PostRenderCamera(cam, cam.targetTexture);
				}
			}
		}
		if(camera != null) {
			var prevTex = camera.targetTexture;
			camera.targetTexture = renderTexture;
			camera.Render();
			if (camera.GetComponent<RenderTextureToScreen>() != null) {
				//Graphics.SetRenderTarget(renderTexture);
				//camera.GetComponent<RenderTextureToScreen>().OnPostRender();
				PostRenderCamera(camera, renderTexture);
			}
			camera.targetTexture = prevTex; 
		}
	}

	void PostRenderCamera(Camera camera, RenderTexture renderTexture) {
		var mat = camera.GetComponent<RenderTextureToScreen>().mat;
		//Debug.Log(string.Format("Postrendering {0} to {1}", camera.name, renderTexture.name));
		if (mat.passCount != 1 && mat.passCount != 3) {
			throw new System.Exception("Only 1 or 3 pass postprocess supported here. sorppa!");
		}
		Graphics.SetRenderTarget(renderTexture);
		GL.PushMatrix();
		GL.LoadOrtho();
		RenderTexture tmptexture = null;
		for (var i = 0; i < mat.passCount; ++i) {
			if (mat.passCount == 3 && i == 0) {
				tmptexture = new RenderTexture(renderTexture.width, renderTexture.height, 32, RenderTextureFormat.ARGB32);
				Graphics.SetRenderTarget(tmptexture);
				GL.Clear(true, true, camera.backgroundColor);
			} else
			if (mat.passCount == 3 && i == 1) {
				mat.SetTexture("_GrabTexture", tmptexture);
				continue;
			} else
			if (mat.passCount == 3 && i == 2) {
				Graphics.SetRenderTarget(renderTexture);
			} else {
				Graphics.SetRenderTarget(renderTexture);
			}
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
		if (tmptexture != null) {
			GameObject.DestroyImmediate(tmptexture);
		}
	}
	
	void OnGUI () {
		SelectCamera();
		RenderCamera(true);
		Graphics.SetRenderTarget(null);
		GUI.DrawTexture(new Rect(0.0f, 0.0f, renderTexture.width, renderTexture.height), renderTexture);
		lockCamera = EditorGUILayout.Toggle(lockCamera);
	}
}
