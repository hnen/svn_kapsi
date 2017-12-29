using UnityEngine;
using System.Collections;

 
[ExecuteInEditMode, RequireComponent(typeof(Camera))]
public class SetCameraReplacementShader : MonoBehaviour {

	public Shader replacementShader;

	void Awake () {	
		this.GetComponent<Camera>().SetReplacementShader(replacementShader, null);
	}
	
	void OnPreRender() {
		if (!Application.isPlaying) {
			this.GetComponent<Camera>().SetReplacementShader(replacementShader, null);
		}
	}

}
