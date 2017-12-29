using UnityEngine;
using UnityEditor;
using System.Collections;
using System.IO;
using System.Linq;

namespace f.DemoExporter {
	public class ComponentExporter {

		public string SerializeComponent(UnityEngine.Component c, int componentRefId, AssetManager assetManager, IObjectContainer objc) {
			string componentTemplate = "{0}: {{ ref: {2}{1} }}";
			string componentName = c.GetType().Name;
			string componentContent = "";
			if (c is Transform) {
				componentName = "transform";
				componentContent = Serialize((Transform)c);
			} else if (c is MeshFilter) {
				componentName = "MeshFilter";
				componentContent = Serialize(c as MeshFilter, assetManager);
			} else if (c is MeshRenderer) {
				componentName = "MeshRenderer";
				componentContent = Serialize(c as MeshRenderer, assetManager);
			} else if (c is Camera) {
				componentName = "camera";
				componentContent = Serialize(c as Camera, assetManager, objc);
			} else if (c is MonoBehaviour) {
				componentContent = Serialize(c as MonoBehaviour, objc, assetManager);
			} else if (c is AudioSource) {
				componentName = "AudioSource";
				componentContent = Serialize(c as AudioSource, assetManager);
			}
			return string.Format(componentTemplate, componentName, componentContent, componentRefId);
		}

		private string Serialize(MeshFilter mf, AssetManager assetManager) {
			string transformTemplate = ", mesh: {0}";
			return string.Format(transformTemplate, Serialize(mf.sharedMesh, assetManager));
		}

		private string Serialize(MeshRenderer mr, AssetManager assetManager) {
			string transformTemplate = ", sharedMaterial: {0}, sharedMaterials: {1}";
			return string.Format(transformTemplate, Serialize(mr.sharedMaterial, assetManager), Serialize(mr.sharedMaterials, assetManager));
		}

		private string Serialize(AudioSource asource, AssetManager assetManager) {
			string transformTemplate = ", clip: {0}";
			return string.Format(transformTemplate, Serialize(asource.clip, assetManager));
		}

		private string Serialize(MonoBehaviour mb, IObjectContainer objs, AssetManager assetManager) {
			string monoBehaviourTemplate = ", {0}";
			return string.Format(monoBehaviourTemplate, assetManager.RegisterScript(mb, objs));
			/*
			if (script != null) {
				JsCodeExtractor.PostProcessScript(script, objs.GetAllScripts());
				foreach(var prop in script.Properties) {
					var field = mb.GetType().GetFields().First(a => a.Name == prop.Name); 
					prop.Value = field.GetValue(mb);
				}

				var ret = string.Format(monoBehaviourTemplate, Serialize(script, objs, assetManager));
				//Debug.Log(ret);
				return ret;
			} else {
				return string.Format(monoBehaviourTemplate, "");
			}
			*/
		}

		private string Serialize(JsScript script, IObjectContainer objs, AssetManager am) {
			string methodTemplate = "{0}: function({1}) {2}";
			string propertyTemplate = "{0}: {1}";
			string result = "";
			bool first = true;
			foreach(var property in script.Properties) {
				result += (!first ? ", " : "") + string.Format(propertyTemplate, property.Name, SerializePropertyValue(property.Value, objs, am));
				first = false;
			}
			foreach(var method in script.Methods) {
				result += (!first ? ", " : "") + string.Format(methodTemplate, method.Name, method.Parameters, method.Code);
				first = false;
			}
			return result.Substring(0, result.Length);
		}

		private string SerializePropertyValue(object value, IObjectContainer objs, AssetManager am) {
			if (value is UnityEngine.Object) {
				if (objs.GetObjectRefId(value as UnityEngine.Object) >= 0) {
					return string.Format("{{ __sceneRef: {0} }}", objs.GetObjectRefId(value as UnityEngine.Object));
				} else {
					return string.Format("{0}", am.RegisterAsset(value as UnityEngine.Object));
				}
			} else {
				return value.ToString();
			}
		}

		private string Serialize(Transform tf) {
			string transformTemplate = ", localPosition: {0}, localRotation: {1}, localScale: {2}";
			return string.Format(transformTemplate, Serialize(tf.localPosition), Serialize(tf.localRotation),
			                     Serialize (tf.localScale));
		}

		private string Serialize(Camera camera, AssetManager am, IObjectContainer objc) {

			bool isMainCamera = camera.gameObject.tag == "MainCamera";

			string cameraTemplate = ", projection: \"{1}\", fov: {2}, orthographicSize: {10}, renderOrder: {3}, clear: \"{4}\", clearColor: {5}, near: {6}, far: {7}, viewport: {8}, targetTexture: {9}, cullingMask: {11}, isMainCamera: {12}";
			return string.Format(cameraTemplate, "camera", camera.orthographic ? "ortho" : "perspective", 
			                     camera.fieldOfView, camera.depth, camera.clearFlags.ToString(), Serialize(camera.backgroundColor),
			                     camera.nearClipPlane, camera.farClipPlane, Serialize (camera.rect), am.RegisterAsset(camera.targetTexture),
			                     camera.orthographicSize, camera.cullingMask, isMainCamera);
		}

		private string Serialize(Color color) {
			return string.Format ("{{r: {0}, g: {1}, b: {2}, a: {3}}}", color.r, color.g, color.b, color.a);
		}

		private string Serialize(Rect rect) {
			return string.Format ("{{x_min: {0}, y_min: {1}, x_max: {2}, y_max: {3}}}", rect.min.x, rect.min.y, rect.max.x, rect.max.y);
		}

		private string Serialize(Vector3 vec) {
			return string.Format("{{x: {0}, y: {1}, z: {2}}}", vec.x, vec.y, vec.z);
		}
		
		private string Serialize(Quaternion q) {
			return string.Format("{{x: {0}, y: {1}, z: {2}, w: {3}}}", q.x, q.y, q.z, q.w);
		}

		private string Serialize(UnityEngine.Object obj, AssetManager assetManager) {
			return assetManager.RegisterAsset(obj).ToString();
		}
		
		private string Serialize(UnityEngine.Material [] mats, AssetManager assetManager) {
			bool first = true;
			string list = "";
			foreach(var mat in mats) {
				if (!first) {
					list += ", ";
				}
				list += Serialize(mat, assetManager);
				first = false;
			}
			return string.Format("[{0}]", list);
			//return assetManager.RegisterAsset(obj).ToString();
		}

		private string Serialize(UnityEngine.Material obj, AssetManager assetManager) {
			return assetManager.RegisterAsset(obj).ToString();
		}

	}
}
