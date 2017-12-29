using UnityEngine;
using System;
using System.IO;
using System.Collections.Generic;

public class CsCodeExtractor {
	
	public class CsMethod {
		public string Type;
		public string Name;
		public CsProperty [] Parameters;
		public string Code;
		public CsClass ParentClass;
	}
	
	public class CsProperty {
		public string Type;
		public string Name;
		//public string Value;
	}

	public class CsClass {
		public string Name;
		public string [] Implements;
		public CsProperty [] Properties;
		public CsMethod [] Methods;
	}
	
	public class CsScript {
		public CsClass [] Classes;
	}


	public static CsScript GetScript(MonoBehaviour mb) {
		var scriptName = mb.GetType().Name;
		var scriptPath = "Assets/" + scriptName + ".cs";
		if (!File.Exists(scriptPath)) {
			scriptPath = "Assets/Scripts/" + scriptName + ".cs";
		}
		if (!File.Exists(scriptPath)) {
			scriptPath = "Assets/Scripts/Framework/" + scriptName + ".cs";
		}
		if (!File.Exists(scriptPath)) {
			scriptPath = "Assets/Scripts/SimuDemo/" + scriptName + ".cs";
		}
		if (!File.Exists(scriptPath)) {
			throw new Exception("Add new script path here!! (pitäs vissiin tehä ihan oikee filen etsintä.....)");
		}
		if (File.Exists(scriptPath)) {
			var code = File.ReadAllText(scriptPath);
			return CsCodeExtractor.GetScript(code);
			//return new CsScript();
		} else {
			return null;
		}
	}

	private static CsScript GetScript(string text) {
		List<CsClass> classes = new List<CsClass>();
		int N = 0;
		while (N < text.Length) {
			var s0 = N < text.Length - "class".Length ? text.Substring(N, "class".Length) : "";
			if (s0 == "class") {
				var csClass = GetClass(text, ref N);
				classes.Add (csClass);
				Debug.Log("Parsed class: " + csClass.Name + " w/ " + csClass.Implements.Length + " parent classes.");
			}
			N++;
		}

		return new CsScript {
			Classes = classes.ToArray()
		};
	}

	private static CsClass GetClass(string code, ref int N) {
		CsCodeParser.SkipToNextWord(code, ref N);
		var className = CsCodeParser.ReadWord(code, ref N);

		string [] inherits = new string[0];
		if (code[N] == ':') {
			inherits = ReadInheritances(code, ref N);
		}

		int blockD = 0;

		List<CsProperty> properties = new List<CsProperty>();
		List<CsMethod> methods = new List<CsMethod>();

		CsClass ret = new CsClass();

		do {
			if (code[N] == '{') {
				blockD++;
				CsCodeParser.SkipToNextWord(code, ref N);
			} else 
			if (code[N] == '}') {
				blockD--;
				CsCodeParser.SkipToNextWord(code, ref N);
			} else {
				var w = CsCodeParser.ReadWord(code, ref N);
				if (w == "public" || w == "private" || w == "protected") {
				} else {
					var symboltype = w;
					var oN = N;
					bool validType = CsCodeParser.IsValidKeyword(symboltype) && !CsCodeParser.IsReservedKeyword(symboltype);
					var symbolname = CsCodeParser.ReadWord(code, ref N);
					if (symbolname == "<") {
						symbolname = CsCodeParser.ReadWord(code, ref N);
						symbolname = CsCodeParser.ReadWord(code, ref N);
						symbolname = CsCodeParser.ReadWord(code, ref N);
					}
					bool isArray = false;
					if (symbolname == "[") {
						isArray = true;
						CsCodeParser.ReadWord(code, ref N);
						symbolname = CsCodeParser.ReadWord(code, ref N);
					}
					bool validName = CsCodeParser.IsValidKeyword(symbolname) && !CsCodeParser.IsReservedKeyword(symbolname);
					var df = CsCodeParser.ReadWord(code, ref N);
					bool valid = validName && validType;
					if (valid && (df == ";" || df == "=")) {
						var nprop = ReadProperty(code, ref N, symboltype, df, symbolname);
						properties.Add(nprop);
						Debug.Log(string.Format("Parsed property: {0} {1};", nprop.Type, nprop.Name));
					} else if(valid && (df == "(")) {
						Debug.Log("Method: " + symboltype + " " + symbolname);
						var nmethod = ReadMethod(code, ref N, symboltype, symbolname);
						nmethod.ParentClass = ret;
						methods.Add(nmethod);
						Debug.Log(string.Format("{0} {1} {2}", nmethod.Type, nmethod.Name, nmethod.Code));
					} else {
						//Debug.Log("???: " + symboltype + " " + symbolname + "(" + df + ")");
						N = oN;
					}
				}
			}
		} while (blockD > 0);

		ret.Name = className;
		ret.Implements = inherits;
		ret.Properties = properties.ToArray();
		ret.Methods = methods.ToArray();

		return ret;

	}

	private static CsMethod ReadMethod(string text, ref int N, string typename, string methodname) {
		var prms = ReadMethodParameters(text, ref N);
		int blockD = 0;
		var code = "";
		string prevw = "";
		do {
			if (text[N] == '{') {
				code += '{';
				prevw = "{";
				blockD++;
				CsCodeParser.SkipToNextWord(text, ref N);
			} else 
			if (text[N] == '}') {
				code += '}';
				prevw = "}";
				blockD--;
				CsCodeParser.SkipToNextWord(text, ref N);
			} else {
				var currw = CsCodeParser.ReadWord(text, ref N);
				if (currw.StartsWith("//") || currw.StartsWith("/*")) {
				} else {
					if (prevw.Length > 0 && (CsCodeParser.IsValidSymbolChar(prevw[prevw.Length-1]) && CsCodeParser.IsValidSymbolChar(currw[0]))) {
						code += " ";
					}
					code += currw;
					prevw = currw;
				}
			}
		} while (blockD > 0);

		return new CsMethod {
			Type = typename,
			Name = methodname,
			Parameters = prms,
			Code = code
		};
	}

	private static CsProperty [] ReadMethodParameters(string code, ref int N) {
		List<CsProperty> ret = new List<CsProperty>();
		do {
			var w0 = CsCodeParser.ReadWord(code, ref N);
			if (w0 == ")") {
				break;
			} else {
				var type = w0;
				var name = CsCodeParser.ReadWord(code, ref N);
				if (name == "<") {
					name = CsCodeParser.ReadWord(code, ref N);
					name = CsCodeParser.ReadWord(code, ref N);
					name = CsCodeParser.ReadWord(code, ref N);
				}
				ret.Add(new CsProperty {
					Type = type,
					Name = name
				} );
				var w1 = CsCodeParser.ReadWord(code, ref N);
				if (w1 == ")") {
					break;
				}
			}
		} while (true);
		return ret.ToArray();
	}


	private static CsProperty ReadProperty(string code, ref int N, string typename, string df, string varname) {
		string varvalue = "";
		if (df == "=") {
			char c = code[N];
			do {
				varvalue += c;
				N++;
				c = code[N];
			} while(c != ';');
		}
		return CreateProperty(typename, varname, varvalue);
	}

	private static CsProperty CreateProperty(string typename, string varname, string value) {
		return new CsProperty {
			Type = typename,
			Name = varname
		};
	}

	private static string [] ReadInheritances(string code, ref int N) {
		CsCodeParser.SkipToNextWord(code, ref N);
		List<string> inherits = new List<string>();
		do {
			if (code[N] == ',') {
				CsCodeParser.SkipToNextWord(code, ref N);
			}
			inherits.Add(CsCodeParser.ReadWord(code, ref N));
		} while(code[N] == ',');
		return inherits.ToArray();
	}


}
