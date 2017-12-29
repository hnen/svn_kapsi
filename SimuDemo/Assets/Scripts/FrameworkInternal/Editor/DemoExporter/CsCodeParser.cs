using System;

public static class CsCodeParser {
	public static bool IsReservedKeyword(string word) {
		return word == "new" || word == "class" || word == "namespace" || word == "return" || word =="else" || word == "if";
	}
	
	public static bool IsValidSymbolChar(char c) {
		return char.IsLetterOrDigit(c) || c == '_';
	}
	
	public static bool IsValidKeyword(string word) {
		if (word.Length == 0) return false;
		if (!char.IsLetter(word[0])) {
			return false;
		}
		foreach(char c in word) {
			if (!char.IsLetterOrDigit(c) && c != '_') {
				return false;
			}
		}
		return true;
	}
	
	public static string ReadWord(string text, ref int N) {
		if (N >= text.Length) {
			return "";
		}
		string ret = "";
		bool isAlphaNumeric = char.IsLetter(text[N]);
		bool isNumber = char.IsNumber(text[N]);
		if (isAlphaNumeric) {
			while(!char.IsWhiteSpace(text[N]) && (char.IsLetterOrDigit(text[N]) || text[N] == '_')) {
				ret += text[N];
				N++;
			}
		} else if (isNumber) {
			while(!char.IsWhiteSpace(text[N]) && (char.IsNumber(text[N]) || text[N] == '.' || text[N] == 'f')) {
				ret += text[N];
				N++;
			}
		} else {
			char f = text[N];
			if (f == '"') {
				do {
					ret += text[N];
					N++;
				} while(N==1 || text[N-1] != '"');
			} else if (f == '\'') {
				do {
					ret += text[N];
					N++;
				} while(N==1 || text[N-1] != '\'');
			} else
			if (f == '/' && text[N+1] == '/') {
				while(text[N] != '\n') {
					ret += text[N];
					N++;
				}
			} else if (f == '/' && text[N+1] == '*') {
				while(text[N-2] != '*' || text[N-1] != '/') {
					ret += text[N];
					N++;
				}
			} else {
				ret += text[N];
				N++;
				//while(N < text.Length && text[N] == f) {
				//	ret += text[N];
				//	N++;
				//}
			}
		}
		while(N < text.Length && char.IsWhiteSpace(text[N])) N++;
		return ret;
	}
	
	public static void SkipToNextWord(string text, ref int N) {
		ReadWord(text, ref N);
	}

}

