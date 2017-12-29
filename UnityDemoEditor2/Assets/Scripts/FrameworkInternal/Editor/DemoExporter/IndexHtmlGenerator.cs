using UnityEngine;
using System.Collections;

namespace f.DemoExporter {
	public class IndexHtmlGenerator {

		public void Export(string path) {
			System.IO.File.WriteAllText(path, htmlTemplate);
		}

		const string htmlTemplate = @"
<!DOCTYPE html>
<html>
	<head>
		<script src=""js/unitywrapper.js"" type=""text/javascript""></script>
		<script src=""js/unitytransform.js"" type=""text/javascript""></script>
		<script src=""js/unitymaterial.js"" type=""text/javascript""></script>
		<script src=""js/unitygameobject.js"" type=""text/javascript""></script>
		<script src=""js/unitymesh.js"" type=""text/javascript""></script>
		<script src=""js/unitygl.js"" type=""text/javascript""></script>
		<script src=""js/unitytime.js"" type=""text/javascript""></script>
		<script src=""js/scripts.js"" type=""text/javascript""></script>
		<script src=""js/scene.js"" type=""text/javascript""></script>
		<script src=""js/assets.js"" type=""text/javascript""></script>
		<script src=""js/assets_data.js"" type=""text/javascript""></script>
		<script src=""js/GL.js"" type=""text/javascript""></script>
		<script src=""js/Input.js"" type=""text/javascript""></script>
		<script src=""js/file.js"" type=""text/javascript""></script>
		<script src=""js/scenerenderer.js"" type=""text/javascript""></script>
		<script src=""js/meshrenderer.js"" type=""text/javascript""></script>
		<script src=""js/sceneprocessor.js"" type=""text/javascript""></script>
		<script src=""js/meshimport.js"" type=""text/javascript""></script>
		<script src=""js/Time.js"" type=""text/javascript""></script>
		<script src=""js/mat4.js"" type=""text/javascript""></script>
		<script src=""js/vec3.js"" type=""text/javascript""></script>
		<script src=""js/quat.js"" type=""text/javascript""></script>
		<script src=""js/musa.js"" type=""text/javascript""></script>
		<script src=""js/utils.js"" type=""text/javascript""></script>
		<title>{0}</title>
		<style>
			canvas {
			  padding-left: 0;
			  padding-right: 0;
			  margin-left: auto;
			  margin-right: auto;
			  display: block;
			  vertical-align: middle;
			  margin: auto;
			  position: absolute;
			  top: 0; left: 0; bottom: 0; right: 0;
			}
			div.loading {
			  color: white;
			  font-size: 200px;
			  text-align:center;
			  position: absolute;
			  top: 0; left: 0; bottom: 0; right: 0;
			}
			p.progress {
			  color: white;
			  font-size: 40px;
			}
			div.debug {
			  color: white;
			}
			html,body
			{
			  height: 100%;
			  margin: 0;
			  overflow: hidden;
			  background: black;
			}
		</style>
	</head>

	<body>

	<div class=""loading"">
	<p id = ""loading"">loading</p>
	<p id = ""progress"" class = ""progress"">0</p>
	</div>
	<div id=""debug"" class=""debug"">
	<p></p>
	</div>

	<script src=""js/main.js""></script>

	</body>
</html>
			";
	}

}