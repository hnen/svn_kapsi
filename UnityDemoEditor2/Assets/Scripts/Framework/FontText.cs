using UnityEngine;
using System.Collections;

[ExecuteInEditMode]
public class FontText : MonoBehaviour {

	public FFont Font;
	public string Text;

	[HideInInspector]
	public bool Generated;
	[HideInInspector]
	public string GeneratedText;

	public bool Center;

	public Material OverrideMaterial;

	void Start() {
		if (!Generated) {
			GenerateText();
		}
	}

	void Update() {
		if (Application.isEditor) {
			if (GeneratedText != Text) {
				SetText(Text);
			}
		}
	}

	public void SetText(string text) {
		this.Text = text;
		int cCount = this.transform.childCount;
		for (int i = 0; i < cCount; i++) {
			GameObject.DestroyImmediate(this.transform.GetChild(0).gameObject);
		}
		GenerateText();
	}
	
	void GenerateText() {
		var x = 0.0f;
		var x_offset = 0.0f;

		if (Center) {
			//foreach(char letter in this.Text) {
			for(int i = 0; i < this.Text.Length; i++) {
				char letter = this.Text[i];
				var prefab = Font.GetLetter(letter);
				if (prefab != null) {
					x_offset -= prefab.xOffset0 * 0.5f;
					x_offset -= prefab.xOffset1 * 0.5f;
				} else {
					x += Font.SpaceSpacing * 0.5f;
				}
			}
		}
		for(int i = 0; i < this.Text.Length; i++) {
			char letter = this.Text[i];
			var prefab = Font.GetLetter(letter);
			if (prefab != null) {
				x += prefab.xOffset0;
				var letterObj = GameObject.Instantiate(prefab.Obj);
				letterObj.transform.parent = this.transform;
				letterObj.transform.localRotation = Quaternion.identity;
				letterObj.transform.localPosition = new Vector3(x + x_offset, 0, 0);

				if (OverrideMaterial != null) {
					letterObj.GetComponent<MeshRenderer>().sharedMaterials = new Material[] { OverrideMaterial };
				}

				x += prefab.xOffset1;
			} else {
				x += Font.SpaceSpacing;
			}
		}
		Generated = true;
		GeneratedText = Text;
	}

}
