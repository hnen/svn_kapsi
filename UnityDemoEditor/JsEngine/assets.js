var p_assets_audioTotal = 0;
var p_assets_meshTotal = 0;
var p_assets_shaderTotal = 0;
var p_assets_binsTotal = 0;

var p_assets_audioLoaded = 0;
var p_assets_meshLoaded = 0;
var p_assets_shaderLoaded = 0;
var p_assets_binsLoaded = 0;

var assets_biisi;

function assets_getAsset(asset) {
    return assets['a' + asset];
}

function assets_preprocess(assets) {
    for (var k in assets) {
        var asset = assets[k];
        for (var j in asset) {
            var property = asset[j];
            if (property != null && property.hasOwnProperty('__assetRef')) {
                asset[j] = assets_getAsset(property.__assetRef);
            }
        }
		if (typeof asset === 'object') {
			assets_preprocess(asset);
		}
    }
}

function assets_playSong() {
    if (assets_biisi) {
        musa_start(assets_biisi);
    }
}

function assets_loadMeshes(assets) {
    for (var i in assets) {
        var asset = assets[i];
        if (asset.type === "Mesh") {
            p_assets_loadMesh(asset);
        }
    }
}

function assets_loadBinaries(assets) {
    for (var i in assets) {
        var asset = assets[i];
        if (asset.type === "TextAsset") {
            p_assets_loadTextAsset(asset);
        }
    }
}

function assets_loadShaders(assets) {
    for (var i in assets) {
        var asset = assets[i];
        if (asset.type === "Shader") {
            p_assets_loadShader(asset);
        }
    }
}

function assets_loadAudio(assets) {
    for (var i in assets) {
        var asset = assets[i];
        if (asset.type === "AudioClip") {
            p_assets_loadAudio(asset);
        }
    }
}



function p_assets_loadMesh(asset) {
    p_assets_meshTotal++;
    meshimport_import("data/" + asset.fileName, function(mesh) {
        //asset.mesh = mesh;
        cpyTo(mesh, asset);
        p_assets_meshLoaded++;
    });
}

function p_assets_loadTextAsset(asset) {
    p_assets_binsTotal++;
    file_loadBin("data/" + asset.fileName, function(txt) {
        asset.bytes = txt;
        p_assets_binsLoaded++;
    });
}


function p_assets_loadAudio(asset) {
    p_assets_audioTotal++;

    musa_load("data/" + asset.fileName, function(biisi) {
        assets_biisi = biisi;
        p_assets_audioLoaded++;
    });
}

function p_assets_loadShader(asset) {
    p_assets_shaderTotal++;
    gl_loadShaderFromScripts(asset.vs, asset.fs,
        function(shader) {
            p_assets_shaderLoaded++;
            shader.attrib_pos = GL.getAttribLocation(shader,
                "_glesVertex");
            shader.attrib_nrm = GL.getAttribLocation(shader,
                "_glesNormal");
            shader.attrib_uv = GL.getAttribLocation(shader,
                "_glesMultiTexCoord0");
            shader.uniform_localtoscreen = GL.getUniformLocation(shader,
                "glstate_matrix_mvp");
            shader.uniform_mainTex = GL.getUniformLocation(shader,
                "_MainTex");
            shader.uniform_mainTexST = GL.getUniformLocation(shader,
                "_MainTex_ST");
            asset.shader = shader;
        }
    );

}
