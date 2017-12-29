var user_confirmed = false;

var shader_default;


function start() {
	wgl = gl_createGL("GLCanvas");
	wgl.getExtension("OES_element_index_uint");
	debug = document.getElementById("debug");

	assets_preprocess(assets);

	assets_loadMeshes(assets);
	assets_loadShaders(assets);
	assets_loadAudio(assets);
	assets_loadBinaries(assets);
	assets_loadRenderTextures(assets);

	sceneprocessor_preprocess(scene);

	setTimeout(function() {
		wait();
	}, 10);

	loadDefaultShader();
	unitywrapper_init();




	onPressed(KEY_SPACE, function() {
		user_confirmed = true;
	});
}

function render() {
	wgl.useProgram(null);
	wgl.bindFramebuffer(wgl.FRAMEBUFFER, null);

	time_frameChanged();
	unitywrapper_update();
	sceneprocessor_process(scene);

	sceneprocessor_cnt = 0;
	sceneprocessor_upd = 0;

	sceneprocessor_foreachGameObject(scene, function(obj) {
		if (!obj._startCalled) {
			obj._startCalled = true;
			for(var i in obj) {
				if (obj[i].hasOwnProperty('Start')) {
					obj[i].Start();
				}
			}
		}
	}, false);

	sceneprocessor_runScripts(scene, 'Update');
	sceneprocessor_runScripts(scene, 'LateUpdate');

	scenerenderer_render(scene);

	var target = 60.0;
	setTimeout(render, Math.floor(1000 / target));

}

function getLoadingProgress() {

	var audioMul = 25;
	var binMul = 25;
	var meshMul = 5;
	var shaderMul = 1;

	var totalAssets = audioMul * p_assets_audioTotal + meshMul * p_assets_meshTotal +
		shaderMul * p_assets_shaderTotal + binMul * p_assets_binsTotal;
	var loadedAssets = audioMul * p_assets_audioLoaded + meshMul * p_assets_meshLoaded +
		shaderMul * p_assets_shaderLoaded + binMul * p_assets_binsLoaded;

	return loadedAssets / totalAssets;

}

function wait() {
	if (getLoadingProgress() >= 1) {
		// remove loading text
		var loading = document.getElementById('loading');
		if (loading) {
			sceneprocessor_runScripts(scene, 'Awake', true);
			loading.parentElement.removeChild(loading);
		}
		if (user_confirmed) {
			var progress = document.getElementById('progress');
			progress.parentElement.removeChild(progress);

			// show canvas
			document.body.appendChild(g_Canvas);


			assets_playSong();

			unitywrapper_init();

			setTimeout(render, 32);
		} else {
			var progress = document.getElementById('progress');
			progress.innerText = "Press SPACE to start.";
			setTimeout(wait, 10);
		}
	} else {
		document.getElementById("progress").innerText = "" + Math.round(getLoadingProgress() *
			100);
		setTimeout(wait, 10);
	}
}

function loadDefaultShader() {
	gl_loadShaderFromScripts(
		"\
        precision mediump float;\
        attribute vec4 in_pos;\
        uniform mat4 local_to_screen;\
        void main() {\
            vec4 p = (local_to_screen * vec4(in_pos.xyz, 1.0));\
            gl_Position = p;\
        }\
        ",
		"\
        precision mediump float;\
        void main() {\
            gl_FragColor = vec4(1,1,1,1);\
        }\
        ",
		function(shader) {
			shader.attrib_pos = wgl.getAttribLocation(shader, "in_pos");
			shader.uniform_localtoscreen = wgl.getUniformLocation(shader, "local_to_screen");
			shader_default = shader;
		}
	);
}

start();
