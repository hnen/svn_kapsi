using System;
using System.Linq;
using System.IO;
using System.Collections.Generic;

namespace f.DemoExporter {
	
	public class ScriptExporter {
	
		public static void Export(string path, AssetManager assetManager) {
			string output = "";
			foreach(var method in assetManager.Scripts) {
				string paramString = "";
				foreach(var param in method.Value.Parameters) {
					if (!string.IsNullOrEmpty(paramString)) {
						paramString += ", ";
					}
					paramString += param.Name;
				}
				output += string.Format("var {0} = function({1}) {2};\n\n", method.Key, paramString, ProcessCode(method.Value));
			}

			File.WriteAllText(path, output);
		}


		private static string ProcessCode(CsCodeExtractor.CsMethod method) {
			string code = method.Code;
			code = AddThisReferences(code, method);
			code = FixVariableDeclarations(code);
			code = ConvertListObjectsToNative(code);
			code = RemoveTypeConversions(code);
			code = RemoveNumberSuffixes(code);
			code = FixGenericsCalls(code);
			code = FixArrayInitializations(code);
			CheckForLoops(code);
			return code;
		}

		private static string FixGenericsCalls(string inputCode) {
			string ret = "";
			int N = 0;
			string prev_w = "";
			while (N < inputCode.Length) {
				var w = CsCodeParser.ReadWord(inputCode, ref N);
				if (prev_w.Length > 0 && CsCodeParser.IsValidSymbolChar(prev_w[prev_w.Length-1]) && CsCodeParser.IsValidSymbolChar(w[0])) {
					ret += " ";
				}
				
				var N0 = N;
				var next_w0 = CsCodeParser.ReadWord(inputCode, ref N0);
				var next_w1 = CsCodeParser.ReadWord(inputCode, ref N0);
				var next_w2 = CsCodeParser.ReadWord(inputCode, ref N0);
				var N1 = N0;
				var next_w3 = CsCodeParser.ReadWord(inputCode, ref N1);

				if (CsCodeParser.IsValidKeyword(prev_w) && w == "<" && CsCodeParser.IsValidKeyword(next_w0) && next_w1 == ">") {
					ret += string.Format("(\"{0}\"", next_w0);
					if (next_w3 != ")") {
						ret += ", ";
					}
					N = N0;
				} else {
					ret += w;
				}
				
				prev_w = w;
			}
			
			return ret;
		}

		private static string RemoveNumberSuffixes(string inputCode) {
			string ret = "";
			int N = 0;
			string prev_w = "";
			while (N < inputCode.Length) {
				var w = CsCodeParser.ReadWord(inputCode, ref N);
				if (prev_w.Length > 0 && CsCodeParser.IsValidSymbolChar(prev_w[prev_w.Length-1]) && CsCodeParser.IsValidSymbolChar(w[0])) {
					ret += " ";
				}
				
				var N0 = N;
				var next_w0 = CsCodeParser.ReadWord(inputCode, ref N0);
				var next_w1 = CsCodeParser.ReadWord(inputCode, ref N0);
				
				if (w.Length > 0 && char.IsNumber(w[0])) {
					if (w[w.Length-1] == 'f') {
						w = w.Substring(0, w.Length-1);
					}
					ret += w;
				} else {
					ret += w;
				}
				
				prev_w = w;
			}
			
			return ret;
		}

		private static string CheckForLoops(string inputCode) {
			string ret = "";
			int N = 0;
			string prev_w = "";
			while (N < inputCode.Length) {
				var w = CsCodeParser.ReadWord(inputCode, ref N);

				if (w == "foreach") {
					throw new Exception("foreach loops not supported yet");
				}

				prev_w = w;
			}
			
			return ret;
		}

		private static string RemoveTypeConversions(string inputCode) {
			string ret = "";
			int N = 0;
			string prev_w = "";
			while (N < inputCode.Length) {
				var w = CsCodeParser.ReadWord(inputCode, ref N);
				if (prev_w.Length > 0 && CsCodeParser.IsValidSymbolChar(prev_w[prev_w.Length-1]) && CsCodeParser.IsValidSymbolChar(w[0])) {
					ret += " ";
				}
				
				var N0 = N;
				var next_w0 = CsCodeParser.ReadWord(inputCode, ref N0);
				var next_w1 = CsCodeParser.ReadWord(inputCode, ref N0);

				if (!CsCodeParser.IsValidKeyword(prev_w) && w == "(" && CsCodeParser.IsValidKeyword(next_w0) && next_w1 == ")") {
					if (next_w0 != "var") {
						ret += string.Format("/*({0})*/", next_w0);
					}
					N = N0;
				} else {
					ret += w;
				}
				
				prev_w = w;
			}
			
			return ret;
		}

		private static string ConvertListObjectsToNative(string inputCode) {
			string ret = "";
			int N = 0;
			string prev_w = "";
			while (N < inputCode.Length) {
				var w = CsCodeParser.ReadWord(inputCode, ref N);
				if (prev_w.Length > 0 && CsCodeParser.IsValidSymbolChar(prev_w[prev_w.Length-1]) && CsCodeParser.IsValidSymbolChar(w[0])) {
					ret += " ";
				}
				
				var N0 = N;
				var next_w = CsCodeParser.ReadWord(inputCode, ref N0);

				if (w == "new" && next_w == "List") {
					CsCodeParser.ReadWord(inputCode, ref N); // new
					CsCodeParser.ReadWord(inputCode, ref N); // List
					CsCodeParser.ReadWord(inputCode, ref N); // <
					while (CsCodeParser.ReadWord(inputCode, ref N) != ">"); // typename + >
					CsCodeParser.ReadWord(inputCode, ref N); // (
					while (CsCodeParser.ReadWord(inputCode, ref N) != ")"); // constructing parameters + )
					ret += "[]";
				} else {
					ret += w;
				}
				
				prev_w = w;
			}
			
			return ret;
		}

		private static string FixVariableDeclarations(string inputCode) {
			string ret = "";
			int N = 0;
			string prev_w = "";
			while (N < inputCode.Length) {
				var w = CsCodeParser.ReadWord(inputCode, ref N);
				
				if (prev_w.Length > 0 && CsCodeParser.IsValidSymbolChar(prev_w[prev_w.Length-1]) && CsCodeParser.IsValidSymbolChar(w[0])) {
					ret += " ";
				}
				
				var N0 = N;
				var next_w = CsCodeParser.ReadWord(inputCode, ref N0);
				
				if ((prev_w == ";" || prev_w == "{" || prev_w == "}" || prev_w == "(")) {
					CsCodeParser.ReadWord(inputCode, ref N0);
					var next_w2 = CsCodeParser.ReadWord(inputCode, ref N0);
					if (CsCodeParser.IsValidKeyword(next_w) && CsCodeParser.IsValidKeyword(w) && !CsCodeParser.IsReservedKeyword(w)) {
						ret += string.Format("var /*{0}*/", w);
					} else if (CsCodeParser.IsValidKeyword(w) && next_w == "<" && next_w2 == ">" && !CsCodeParser.IsReservedKeyword(w)) {
						ret += string.Format("var /*{0}*/", w);
						CsCodeParser.ReadWord(inputCode, ref N);
						CsCodeParser.ReadWord(inputCode, ref N);
						CsCodeParser.ReadWord(inputCode, ref N);
					} else {
						ret += w;
					}
				} else {
					ret += w;
				}
				
				prev_w = w;
			}
			
			return ret;
			
		}

		private static string FixArrayInitializations(string inputCode) {
			string ret = "";
			int N = 0;
			string prev_w = "";

			bool insideArray = false;
			while (N < inputCode.Length) {
				var w = CsCodeParser.ReadWord(inputCode, ref N);

				if (prev_w.Length > 0 && CsCodeParser.IsValidSymbolChar(prev_w[prev_w.Length-1]) && CsCodeParser.IsValidSymbolChar(w[0])) {
					ret += " ";
				}

				var N0 = N;
				var next_w = CsCodeParser.ReadWord(inputCode, ref N0);
				var next_w2 = CsCodeParser.ReadWord(inputCode, ref N0);
				var next_w3 = CsCodeParser.ReadWord(inputCode, ref N0);
				var next_w4 = CsCodeParser.ReadWord(inputCode, ref N0);

				if (!insideArray && w == "new" && CsCodeParser.IsValidKeyword(next_w) && next_w2 == "[" && next_w3 == "]" && next_w4 == "{") {
					ret += "[";
					insideArray = true;
					N = N0;
				} else if (insideArray && w == "}") {
					insideArray = false;
					ret += "]";
				} else {
					ret += w;
				}

				prev_w = w;
			}
			
			return ret;
			
		}

		private static string AddThisReferences(string inputCode, CsCodeExtractor.CsMethod method) {
			string ret = "";
			int N = 0;
			string prev_w = "";
			while (N < inputCode.Length) {
				var w = CsCodeParser.ReadWord(inputCode, ref N);

				if (prev_w.Length > 0 && CsCodeParser.IsValidSymbolChar(prev_w[prev_w.Length-1]) && CsCodeParser.IsValidSymbolChar(w[0])) {
					ret += " ";
				}

				if (prev_w != "." && IsClassMember(w, method)) {
					ret += "this.";
				}

				if (prev_w != "." && IsClassMethod(w, method)) {
					//ret += string.Format("this.{0}_", method.ParentClass.Name);
					ret += "this.";
				}
				

				ret += w;

				prev_w = w;
			}

			return ret;

		}

		private static bool IsClassMember(string word, CsCodeExtractor.CsMethod method) {
			return word == "transform" ||
				method.ParentClass.Properties.FirstOrDefault(a => a.Name == word) != null;
		}

		private static bool IsClassMethod(string word, CsCodeExtractor.CsMethod method) {
			return method.ParentClass.Methods.FirstOrDefault(a => a.Name == word) != null;
		}

	}



}