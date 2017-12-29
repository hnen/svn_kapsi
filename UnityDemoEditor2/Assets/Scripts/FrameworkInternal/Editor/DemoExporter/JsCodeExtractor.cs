using UnityEngine;
using System;
using System.IO;
using System.Collections.Generic;

namespace f.DemoExporter {

	public class JsMethod {
		public string Name;
		public string Parameters;
		public string Code;
	}

	public class JsProperty {
		public string Name;
		public object Value;
	}

	public class JsScript {
		public JsProperty [] Properties;
		public JsMethod [] Methods;
	}
	
	public class JsCodeExtractor {

		public static JsScript GetScript(MonoBehaviour mb) {
			var scriptName = mb.GetType().Name;
			var scriptPath = "Assets/" + scriptName + ".js";
			if (!File.Exists(scriptPath)) {
				scriptPath = "Assets/Scripts/" + scriptName + ".js";
			}
			if (File.Exists(scriptPath)) {
				var code = File.ReadAllText(scriptPath);
				return JsCodeExtractor.GetScript(code);
			} else {
				return null;
			}
		}

		public static JsScript GetScript(string text) {
			List<JsMethod> methods = new List<JsMethod>();
			List<JsProperty> properties = new List<JsProperty>();
			int P = 0;
			while(P < text.Length) {
				string s2 = Substring(text, P, 2);
				string s6 = Substring(text, P, 6);
				if (s2 == "//") {
					while(text[P] != '\n') {
						P++;
					}
				} else if (s2 == "/*") {
					while(Substring(text, P, 2) != "*/") {
						P++;
					}
				} else if(s6 == "public") {
					properties.Add(ReadProperty(text, ref P));
				} else {
					string s8 = Substring(text, P, 8);
					if (s8 == "function") {
						methods.Add(ReadAndProcessMethod(text, ref P));
					} else {
						P++;
					}
				}
			}

			var script = new JsScript {
				Methods = methods.ToArray(),
				Properties = properties.ToArray()
			};

			foreach(var method in methods) {
				//method.Code = InjectSelfReferenceToMethodCalls(method.Code, script);
			}

			return script;
		}

		public static void PostProcessScript(JsScript script, JsScript [] allScripts) {
			HashSet<string> allMethods = new HashSet<string>();
			foreach(var scr in allScripts) {
				foreach(var method in scr.Methods) {
					allMethods.Add(method.Name);
				}
			}
			foreach(var method in script.Methods) {
				PostProcessMethod(method, allMethods);
			}
		}

		private static void PostProcessMethod(JsMethod method, HashSet<string> allMethods) {
			//Debug.Log(method.Code);
			//method.Code = InjectSelfReferenceToMethodCalls(method.Code, allMethods);
			//Debug.Log(method.Code);
		}

		private static JsProperty ReadProperty(string text, ref int P) {
			P += 6;
			string name = "";
			while (char.IsWhiteSpace(text[P])) P++;
			while (char.IsLetterOrDigit(text[P])) P++;
			while (char.IsWhiteSpace(text[P])) P++;
			while (char.IsLetterOrDigit(text[P])) {
				name += text[P];
				P++;
			}
			return new JsProperty {
				Name = name
			};
		}

		private static string FixForLoops(string code) {
			string ret = "";
			int N = 0;
			while (N < code.Length) {
				string s3 = N < code.Length - 3 ? code.Substring(N, 3) : "";
				if (s3 == "for") {
					int M = N + 3;
					while(char.IsWhiteSpace(code[M])) M++;
					M++; // (
					while(char.IsWhiteSpace(code[M])) M++;
					M += 3; // var
					while(char.IsWhiteSpace(code[M])) M++;
					string variableName = "";
					while(char.IsLetterOrDigit(code[M])) {
						variableName += code[M];
						M++;
					}
					while(char.IsWhiteSpace(code[M])) M++;
					string keyword = "";
					while(!char.IsWhiteSpace(code[M])) {
						keyword += code[M];
						M++;
					}
					if (keyword == "in") {
						var counterName = "__c" + N;
						string containerName = "";
						while(char.IsWhiteSpace(code[M])) M++;
						while(char.IsLetterOrDigit(code[M])) {
							containerName += code[M];
							M++;
						}
						ret += string.Format("for (var {0} in {1})", counterName, containerName);
						while(code[M] != '{') {
							M++;
						}
						ret += string.Format("{{\nvar {0} = {1}[{2}];\n", variableName, containerName, counterName);
						N = M + 1;
					}
				}
				ret += code[N];
				N++;
			}

			return ret;
		}

		private static string InjectSelfReferenceToMethodCalls(string code, HashSet<string> methods) {
			string ret = "";
			int N = 0;
			while (N < code.Length) {
				if (code[N] == '.') {
					int M = N+1;
					var funcName = "";
					while(char.IsLetterOrDigit(code[M])) {
						funcName += code[M];
						M++;
					}
					if (methods.Contains(funcName)) {
						int P = N-1;
						var objName = "";
						while(char.IsLetterOrDigit(code[P])) {
							objName = code[P] + objName;
							P--;
						}
						bool hasParameters = false;
						while(code[M] != '(') {
							M++;
						}
						M++;
						int Q = M;
						while(code[M] != ')') {
							if (!char.IsWhiteSpace(code[M])) {
								hasParameters = true;
								break;
							}
							M++;
						}
						string proto = hasParameters ? ".{0}({1}, " : ".{0}({1}";
						string text = string.Format(proto, funcName, objName);
						ret += text;
						N = Q;
					}
				}

				ret += code[N];
				N++;
			}

			return ret;

		}

		private static JsMethod ReadAndProcessMethod(string text, ref int P) {
			var method = ReadMethod(text, ref P); 
			method.Parameters = StripTypeNames(method.Parameters);
			method.Code = StripTypeNames(method.Code);
			method.Code = StripFloatSuffixes(method.Code);
			method.Code = StringifyTypeParameters(method.Code);
			Debug.Log(method.Code);
			method.Code = FixForLoops(method.Code);
			Debug.Log(method.Code);
			return method;
		}

		private static string StringifyTypeParameters(string text) {
			string ret = "";
			int N = 0;
			while (N < text.Length) {
				string s0 = N < text.Length - "GetComponent".Length ? text.Substring(N, "GetComponent".Length) : "";
				if (s0 == "GetComponent") {
					ret += "GetComponent('";
					while(text[N] != '(') N++;
					while(!char.IsLetter(text[N])) N++;
					while(char.IsLetterOrDigit(text[N])) {
						ret += text[N];
						N++;
					}
					ret += "')";
					while(text[N] != ')') N++;
				} else {
					ret += text[N];
				}
				N++;
			}
			return ret;
		}

		private static string StripFloatSuffixes(string text) {
			string ret = "";
			int N = 0;
			while (N < text.Length) {
				char c = text[N];
				if(c == 'f' && char.IsDigit(text[N-1])) {
				} else {
					ret += c;
				}
				N++;
			}
			return ret;
		}

		private static string StripTypeNames(string text) {
			string ret = "";
			int N = 0;
			while (N < text.Length) {
				char c = text[N];
				if (c == ':') {
					N++;
					while(char.IsWhiteSpace(text[N])) N++;
					while(N < text.Length && char.IsLetterOrDigit(text[N])) N++;
					if (N >= text.Length) {
						break;
					}
					c = text[N];
				}
				ret += c;
				N++;
			}

			return ret;
		}

		private static JsMethod ReadMethod(string text, ref int P) {
			while (!char.IsWhiteSpace(text[P])) {
				P++;
			}
			while (char.IsWhiteSpace(text[P])) {
				P++;
			}
			string name = "";
			while (!char.IsWhiteSpace(text[P]) && text[P] != '(') {
				name += text[P];
				P++;
			}

			string paramstr = "";
			while (text[P] != '(') {
				P++;
			}
			P++;
			while (text[P] != ')') {
				paramstr += text[P];
				P++;
			}

			string code = "";
			while (text[P] != '{') {
				P++;
			}
			int depth = 0;
			bool inString = false;
			bool inComment = false;
			char stringDelimiter = '\0';
			do {
				string s4 = P < text.Length - 4 ? Substring(text, P, 4) : "";
				string s2 = P < text.Length - 2 ? Substring(text, P, 2) : "";
				if (!inComment) {

					if (!inString && s2 == "//") {
						while(P < text.Length && text[P] != '\n') {
							P++;
						}
					}

					//if (s4 == "this") {
					//	code += "self";
					//	P += 4;
					//} else 
					if (s2 == "/*") {
						inComment = true;
					} else {
						if (!inString && (text[P] == '"' || text[P] == '\'')) {
							inString = true;
							stringDelimiter = text[P];
						} else if (inString && text[P-1] != '\\' && text[P] == stringDelimiter) {
							inString = false;
						}
						if (!inString) {
							if (text[P] == '{') {
								depth++;
							} else if (text[P] == '}') {
								depth--;
							}
						}
						code += text[P];
						P++;
					}
				} else {
					if (s2 == "*/") {
						inComment = false;
						P += 2;
					} else {
						P++;
					}
				}
			} while (depth > 0);

			code += text[P];
			//Debug.Log(code);

			return new JsMethod {
				Name = name, Parameters = paramstr, Code = code
			};
		}



		private static string Substring(string text, int p, int len) {
			if (p >= text.Length) {
				throw new IndexOutOfRangeException();
			} else
			if (text.Length < p + len) {
				return text.Substring(p);
			} else {
				return text.Substring(p, len);
			}
		}


	}

}


