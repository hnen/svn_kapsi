using UnityEngine;
using System.Collections;

public class FFont : MonoBehaviour {

	[System.Serializable]
	public class LetterObj {
		public GameObject Obj;
		public char Letter;
		public float xOffset0;
		public float xOffset1;
	}

	public LetterObj [] Letters;
	public float SpaceSpacing;

	public LetterObj GetLetter(char c) {
		var l = FindLetter(c);
		if (l == null) {
			if (char.IsLower(c)) {
				return FindLetter(char.ToUpper(c));
			} else if (char.IsUpper(c)) {
				return FindLetter(char.ToLower(c));
			}
			return null;
		} else {
			return l;
		}
	}

	private LetterObj FindLetter(char c) {
		for(int i = 0; i < this.Letters.Length; i++) {
			var letter = this.Letters[i];

			if (letter.Letter == c) {
				return letter;
			}
		}
		return null;
	}

}
