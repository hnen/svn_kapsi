using UnityEngine;
using UnityEditor;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Linq;

namespace f.DemoExporter {
	public class AssetExporter {

		const string fileTemplate = "var assets = {{\n{0}\n}};\n";

		Dictionary <Shader, int> usedShaders = new Dictionary<Shader, int>();

		public void Export(string fileName, string resourceRoot, string dataRoot, AssetManager assetManager) {
			bool first = true;
			usedShaders.Clear();
			string objects = "";
			int I = 0;
			HashSet<int> addedAssets = new HashSet<int>();
			bool newAssets;
			do {
				newAssets = false;
				foreach(var asset in assetManager.Assets.ToArray()) {
					if (addedAssets.Contains(asset.Key)) {
						continue;
					}
					newAssets = true;
					addedAssets.Add(asset.Key);
					if (!first) {
						objects += ",\n";
					}
					objects += Serialize(asset.Key, asset.Value, dataRoot, assetManager);
					first = false;
					I++;
				}
			} while(newAssets);

			var file = string.Format(fileTemplate, objects);
			System.IO.File.WriteAllText(resourceRoot + fileName, JsFormatter.Format(file));
		}

		string Serialize(int assetId, UnityEngine.Object asset, string dataRoot, AssetManager assetManager) {
			string template = "a{0}: {{ type: \"{1}\", {2}\t}}";
			if (asset is GameObject) {
				return string.Format("a{0}: {2}", assetId, asset.GetType().Name, SerializeGameObject(asset as GameObject, assetId, dataRoot, assetManager));
			} else {
				return string.Format(template, assetId, asset.GetType().Name, Serialize(asset, assetId, dataRoot, assetManager));
			}
		}

		string Serialize(UnityEngine.Object asset, int assetId, string dataRoot, AssetManager assetManager) {
			if (asset is Mesh) {
				return SerializeMesh (asset as Mesh, assetId, dataRoot);
			} else if (asset is AudioClip) {
				return SerializeAudioClip(asset as AudioClip, assetId, dataRoot);
			} else if (asset is Material) {
				return SerializeMaterial(asset as Material, assetId, dataRoot, assetManager);
			} else if (asset is TextAsset) {
				return SerializeTextAsset(asset as TextAsset, assetId, dataRoot);
			} else if (asset is Shader) {
				return SerializeShader(asset as Shader, assetId);
			} else if (asset is RenderTexture) {
				return SerializeRenderTexture(asset as RenderTexture, assetId);
			} else if (asset is Texture2D) {
				return SerializeTexture2D(asset as Texture2D, assetId, dataRoot);
			} else {
				throw new UnityException("Unknown asset type: " + asset.GetType().Name);
			}
		}

		string SerializeGameObject(GameObject gameObject, int assetId, string dataRoot, AssetManager am) {
			var ego = DemoExporter.PrefabToExportGameObject(gameObject, null);
			var se = new SceneExporter();
			return se.ObjectToString(ego, am, 0, null);
		}

		string SerializeAudioClip(AudioClip audioClip, int assetId, string dataRoot) {
			string fileName = string.Format("a{0:000}.mp3", assetId);
			var srcPath = AssetDatabase.GetAssetPath(audioClip);
			File.Copy(srcPath, dataRoot + "/" + fileName, true);
			return string.Format("fileName: \"{0}\"", fileName);
		}

		string SerializeTextAsset(UnityEngine.TextAsset textAsset, int assetId, string dataRoot) {
			string fileName = string.Format("b{0:000}.bin", assetId);
			//MeshWriter.WriteMesh(mesh, dataRoot + "/" + fileName);
			File.WriteAllBytes(dataRoot + "/" + fileName, textAsset.bytes);
			return string.Format("fileName: \"{0}\"", fileName);
		}

		string SerializeMesh(UnityEngine.Mesh mesh, int assetId, string dataRoot) {
			string fileName = string.Format("m{0:000}.f", assetId);
			MeshWriter.WriteMesh(mesh, dataRoot + "/" + fileName);
			return string.Format("fileName: \"{0}\"", fileName);
		}
		
		string SerializeTexture2D(UnityEngine.Texture2D txt2d, int assetId, string dataRoot) {
			string fileName = string.Format("t{0:000}.png", assetId);
			//MeshWriter.WriteMesh(mesh, dataRoot + "/" + fileName);
			TextureWriter.WriteTexture(txt2d, dataRoot + "/" + fileName);
			return string.Format("fileName: \"{0}\"", fileName);
		}

		string SerializeMaterial(UnityEngine.Material material, int assetId, string dataRoot, AssetManager assetManager) {
			var shader = material.shader;
			return string.Format("name: \"{0}\", properties: {2}, shader: {1}", material.name,
			                     assetManager.RegisterAsset(shader), SerializeMaterialProperties(material, assetManager), 1);
		}

		string SerializeRenderTexture(UnityEngine.RenderTexture rt, int assetId) {
			return string.Format("width: {0}, height: {1}", rt.width, rt.height);
		}

		string SerializeMaterialProperties(UnityEngine.Material material, AssetManager am) {
			var props = MaterialEditor.GetMaterialProperties(new [] { material });
			string propString = "";
			bool first = true;
			foreach(var prop in props) {
				if (!first) {
					propString = propString + ", ";
				}	
				propString += prop.name + ": ";
				switch(prop.type) {
				case MaterialProperty.PropType.Texture:
					var i = am.RegisterAsset(prop.textureValue);
					propString += i;
					break;
				case MaterialProperty.PropType.Color:
					var c = prop.colorValue;
					propString += string.Format("[{0}, {1}, {2}, {3}]", c.r, c.g, c.b, c.a);
					break;
				case MaterialProperty.PropType.Float:
				case MaterialProperty.PropType.Range:
					var c1 = prop.floatValue;
					propString += string.Format("[{0}]", c1);
					break;
				case MaterialProperty.PropType.Vector:
					var c2 = prop.vectorValue;
					propString += string.Format("[{0}, {1}, {2}, {3}]", c2.x, c2.y, c2.z, c2.w);
					break;
					break;
				default:
					throw new System.NotSupportedException("Material property type " + prop.type + " not implemented.");
				}
				first = false;
			}
			return string.Format("{{{0}}}", propString);
		}

		string SerializeShader(Shader shader, int assetId) {
			var so = (new SerializedObject (shader));
			var source = so.FindProperty("m_Script").stringValue;

			var openShader = typeof(ShaderUtil).GetMethod("OpenCompiledShader", BindingFlags.Static | BindingFlags.NonPublic);
			openShader.Invoke(null,new object[]{shader, 2, 0x7fffffff, true});
			//Debug.Log (AssetDatabase.GetAssetPath (shader));
			var path = AssetDatabase.GetAssetPath(shader);
			var filename = path.Substring(path.LastIndexOf ('/')+1);
			try {
				filename = filename.Substring (0, filename.IndexOf ('.'));
			} catch(System.Exception e) {
				Debug.Log("problem with " + filename);
				throw e;
			}
			var compiled_filename = string.Format ("Temp/Compiled-{0}.shader", filename);
			source = File.ReadAllText(compiled_filename);

			string [] vss, fss;
			GetGlesShaders(source, out vss, out fss);

			//vs.Replace("\n", "\\\n"), fs.Replace("\n", "\\\n")

			string passesString = "";
			for(int i = 0; i < vss.Length; i++) {
				var vs = vss[i];
				var fs = fss[i];
				if (i > 0) {
					passesString += ", ";
				}
				passesString += string.Format("{{ vs: \"{0}\", fs: \"{1}\" }}", vs.Replace("\n", "\\\n"), fs.Replace("\n", "\\\n"));
			}

			return string.Format("name: \"{1}\", passes: [{2}]", assetId, shader.name, passesString);
		}

		void GetGlesShaders(string source, out string [] vss, out string [] fss) {
			List<string> vslist = new List<string>();
			List<string> fslist = new List<string>();
			int P = 0;

			string vs = "";
			string fs = "";

			string findstr = "SubProgram \"gles \"";
			while (P < source.Length - findstr.Length) {
				if (findstr == source.Substring(P, findstr.Length)) {
					P++;
					string ignoreStr = "SubProgram";
					string str0 = "#ifdef VERTEX";
					bool skip = false;
					while(source.Substring(P, str0.Length) != str0) {
						if (source.Substring(P, ignoreStr.Length) == ignoreStr) {
							skip = true;
							P++;
							break;
						}
						P++;
					}
					if (skip) {
						continue;
					}
					P += str0.Length;
					string str1 = "#endif";
					while(source.Substring(P, str1.Length) != str1) {
						vs += source[P];
						P++;
					}

					string str2 = "#ifdef FRAGMENT";
					while(source.Substring(P, str2.Length) != str2) {
						P++;
					}
					P += str2.Length;
					while(source.Substring(P, str1.Length) != str1) {
						fs += source[P];
						P++;
					}
					vslist.Add(vs);
					fslist.Add(fs);
					vs = "";
					fs = "";
				}
				P++;
			}

			vss = vslist.ToArray();
			fss = fslist.ToArray();

		}

	}
}
