using UnityEngine;
using UnityEditor;
using System.Collections;

namespace f.DemoExporter {
	public class SceneExporter {

		const string fileTemplate = "var scene = {0};\n";

		const string objTemplate = "{{name: \"{3}\", enabled: {7}, ref:{6},{5}children: {4}}}";
	

		public void Export(string path, AssetManager assetManager, ExportGameObject rootObject, IObjectContainer objectContainer) {
			System.IO.File.WriteAllText(path, JsFormatter.Format(string.Format(fileTemplate, ObjectToString(rootObject, assetManager, 0, objectContainer))));
		}

		public string ObjectToString(ExportGameObject obj, AssetManager assetManager, int indent, IObjectContainer objectContainer) {
			var childStr = CreateChildString(obj, assetManager, indent, objectContainer);
			var componentStr = CreateComponentString(obj, assetManager, indent, objectContainer);

			string indentStr = "";
			for (int i = indent-1; i >= 0; i--) {
				indentStr += "    ";
			}

			string template = objTemplate;
			template = string.Format(template, indentStr, indentStr + "    ", indentStr + "    " + "    ", 
			                         obj.GameObject == null ? "root" : obj.GameObject.name, childStr, componentStr, obj.ReferenceId, obj.GameObject.activeSelf);
			return template;
		}

		string EscapeBraces(string str) {
			// This is horrible and needs to be rewritten
			string ret = "";
			for(int i = 0; i < str.Length; i++) {
				if(str[i] == '{' && !char.IsNumber(str[i+1]) && i < str.Length-1 && str[i+1] != '{' && (i == 0 || str[i-1] != '{')) {
					ret += "{{";
				} else if(str[i] == '}' && i > 1 && str[i-2] != '{' && (i >= str.Length-1 || str[i+1] != '}') && (str[i-1] != '}' || str[i-3] == '{')) {
					ret += "}}";
				} else {
					ret += str[i];
				}
			}
			return ret;
		}

		bool HasFormatEntries(string template) {
			for(int i = 0; i < template.Length-2; i++) {
				if((template[i] == '{' && char.IsNumber(template[i+1]) && template[i+2] == '}')) {
					return true;
				}
			}
			return false;
		}

		string CreateComponentString(ExportGameObject obj, AssetManager assetManager, int indent, IObjectContainer objectContainer) {
			if (obj.GameObject == null) {
				return "";
			}

			var components = obj.GameObject.GetComponents<UnityEngine.Component>();

			if (components.Length == 0) {
				return "";
			}

			ComponentExporter ce = new ComponentExporter();

			var componentStr = "";
			for(int i = 0; i < components.Length; i++) {
				var c = components[i];
				componentStr += ce.SerializeComponent(c, objectContainer != null ? objectContainer.GetObjectRefId(c) : -1, assetManager, objectContainer);
				if (i < components.Length - 1) {
					componentStr += ",";
				}
				
			}
			componentStr += ",";
			return componentStr;
		}

		string CreateChildString(ExportGameObject obj, AssetManager assetManager, int indent, IObjectContainer container) {
			var childStr = "[";
			if (obj.children.Count > 0) {
				childStr += "\n";
				string [] childObjs = new string[obj.children.Count];
				for (int i = 0; i < obj.children.Count; i++) {
					childObjs[i] = ObjectToString(obj.children[i], assetManager, indent+2, container);
				}
				for (int i = 0; i < obj.children.Count; i++) {
					childStr += childObjs[i];
					if (i < obj.children.Count - 1) {
						childStr += ", ";
					}
					childStr += "\n";
				}
				
			} 
			
			
			childStr += "]";

			return childStr;
		}

	}
}
