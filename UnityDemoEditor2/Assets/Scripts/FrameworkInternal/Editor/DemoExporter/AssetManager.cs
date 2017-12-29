using UnityEngine;
using UnityEditor;
using System.Collections.Generic;
using System.Linq;

namespace f.DemoExporter {

	public class AssetManager {

		int idCnt = 0;
		Dictionary<UnityEngine.Object, int> ids = new Dictionary<Object, int>();
		//Dictionary<UnityEngine.MonoBehaviour, CsCodeExtractor.CsScript> scripts = new Dictionary<MonoBehaviour, CsCodeExtractor.CsScript>();

		Dictionary<string, CsCodeExtractor.CsMethod> scriptMethods = new Dictionary<string, CsCodeExtractor.CsMethod>();

		public IEnumerable<KeyValuePair<int, UnityEngine.Object>> Assets {
			get {
				return ids.Select(a => new KeyValuePair<int, UnityEngine.Object>(a.Value, a.Key));
			}
		}

		public IEnumerable<KeyValuePair<string, CsCodeExtractor.CsMethod>> Scripts {
			get {
				return scriptMethods.Select(a => new KeyValuePair<string, CsCodeExtractor.CsMethod>(a.Key, a.Value));
			}
		}

		public int NumAssets {
			get {
				return ids.Count;
			}
		}

		public string RegisterScript(MonoBehaviour script, IObjectContainer objs) {
			CsCodeExtractor.CsScript parsedscript;
			//if (!scripts.ContainsKey(script)) {
				parsedscript = CsCodeExtractor.GetScript(script);
			//	scripts.Add(script, parsedscript);
			//} else {
			//	parsedscript = scripts[script];
			//}

			string ret = "";

			foreach(var scriptclass in parsedscript.Classes) {
				if (scriptclass.Implements.Contains("MonoBehaviour")) {
					foreach(var property in scriptclass.Properties) {
						UnityEngine.Debug.Log( string.Format("exporting property {0}.{1}", scriptclass.Name, property.Name) );
						bool isStatic = false;
						var field = script.GetType().GetField(property.Name);
						if (field == null) {
							field = script.GetType().GetField(property.Name, System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance);
						} 
						if (field == null) {
							field = script.GetType().GetField(property.Name, System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static);
							isStatic = true;
						} 
						if (field != null) {
							var value = field.GetValue(script);
							if (!string.IsNullOrEmpty(ret)) ret += ", ";
							ret += string.Format("{0}: {1}", property.Name, ExportValue(value, objs));
						} else {
							UnityEngine.Debug.Log("couldn't find property :(");
						}
					}
					foreach(var method in scriptclass.Methods) {
						var jsmethod = JsMethodName(method, scriptclass);
						if (!scriptMethods.ContainsKey(jsmethod)) {
							scriptMethods.Add(jsmethod, method);
						} else {
							if (method.Code != scriptMethods[jsmethod].Code) {
								Debug.LogError("Code mismatch w/ " + jsmethod);
							}
						}
						if (!string.IsNullOrEmpty(ret)) ret += ", ";
						ret += string.Format("{0}: {1}", method.Name, jsmethod);
					}
				}
			}

			return ret;
		}

		private string ExportValue(object value, IObjectContainer objs) {
			if (value == null) {
				return "null";
			} else
			if (value.GetType().IsSubclassOf(typeof(UnityEngine.Object))) {
				var val = objs.GetObjectRefId(value as UnityEngine.Object);
				if (val >= 0) {
					return string.Format("{{__sceneRef: {0}}}", val);
				} else {
					return string.Format("{0}", this.RegisterAsset(value as UnityEngine.Object));
				}
			} else {
				if (value.GetType().IsArray) {
					string ret = "[";
					bool first = true;
					if (value is object[]) {
						foreach(var el in (object[])value) {
							if (!first) {
								ret += ", ";
							}
							ret += ExportValue(el, objs);
							first =false;
						}
					} else if (value is int[]) {
						foreach(var el in (int[])value) {
							if (!first) {
								ret += ", ";
							}
							ret += ExportValue(el, objs);
							first =false;
						}
					} else {
						throw new System.Exception("Havin problems serializing array type " + value.GetType().Name);
					}
					return ret + "]";
				} else
				if (value.GetType() == typeof(string) || value.GetType() == typeof(char)) {
					return string.Format("\"{0}\"", value.ToString().Replace("\n", " "));
				} else if (!value.GetType().IsPrimitive) {
					string ret = "{";
					bool first = true;
					foreach(var field in value.GetType().GetFields()) {
						if (!first) {
							ret += ", ";
						}
						ret += string.Format("{0}: {1}", field.Name, ExportValue(field.GetValue(value), objs));
						first =false;
					}
					return ret + "}";
				} else {
					return string.Format("{0}", value.ToString());
				}
			}
		}

		private string JsMethodName(CsCodeExtractor.CsMethod method, CsCodeExtractor.CsClass parentClass) {  
			return parentClass.Name + "_" + method.Name;
		}

		public string RegisterAsset(UnityEngine.Object obj) {
			if (obj == null) {
				return "null";
			} else {
				if (!ids.ContainsKey(obj)) {
					ids.Add(obj, idCnt++);
				}
				return string.Format("{{__assetRef: \"{0}\"}}", ids[obj]);
			}
		}

	}


}
