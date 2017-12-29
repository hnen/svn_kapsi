var p_assets_textureTotal = 0;
var p_assets_audioTotal = 0;
var p_assets_meshTotal = 0;
var p_assets_shaderTotal = 0;
var p_assets_binsTotal = 0;
var p_assets_rtTotal = 0;


var p_assets_textureLoaded = 0;
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
        if (asset.type == "Material") {
            cpyTo(unitymaterial_prototype, asset);
            for(var prop in unitymaterial_properties) {
                Object.defineProperty(asset, prop, unitymaterial_properties[prop]);
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

function assets_loadRenderTextures(assets) {
    for (var i in assets) {
        var asset = assets[i];
        if (asset.type === "RenderTexture") {
            p_assets_loadRenderTexture(asset);
        }
    }
}

function assets_loadTextures(assets) {
    for (var i in assets) {
        var asset = assets[i];
        if (asset.type === "Texture2D") {
            p_assets_loadTexture(asset);
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

function p_assets_loadRenderTexture(asset) {
    p_assets_rtTotal++;

    var w = document.body.clientWidth;
    var h = document.body.clientHeight;
    
    if (w/h > 16/9) {
        w = h / 9 * 16;
    } else {
        h = w / 16 * 9;
    }
    
    asset.rt = gl_createRenderTexture("rt_" + p_assets_rtTotal, w, h);
}

function p_assets_loadTexture(asset) {
    p_assets_textureTotal++;
    gl_createTextureFromImage("texture_" +p_assets_textureTotal,  "data/" + asset.fileName, function(texture) {
        cpyTo(texture, asset);
        p_assets_textureLoaded++;
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
    asset.shaders = [];
    for (var i = 0; i < asset.passes.length; i++) {
        p_assets_shaderTotal++;
        var vs = asset.passes[i].vs;
        var fs = asset.passes[i].fs;
        var _i = i;
        asset.shaders.push(null);
        gl_loadShaderFromScripts(vs, fs,
            function(shader) {
                p_assets_shaderLoaded++;
                shader.attrib_pos = wgl.getAttribLocation(shader,
                    "_glesVertex");
                shader.attrib_nrm = wgl.getAttribLocation(shader,
                    "_glesNormal");
                shader.attrib_uv = wgl.getAttribLocation(shader,
                    "_glesMultiTexCoord0");
                shader.uniform_localtoscreen = wgl.getUniformLocation(shader,
                    "glstate_matrix_mvp");
                shader.uniform_localtoview = wgl.getUniformLocation(shader,
                    "glstate_matrix_modelview0");
                shader.uniform_worldtoview = wgl.getUniformLocation(shader,
                    "unity_MatrixV");
                shader.uniform_mainTex = wgl.getUniformLocation(shader,
                    "_MainTex");
                shader.uniform_mainTexST = wgl.getUniformLocation(shader,
                    "_MainTex_ST");
                shader.src_fs = fs;
                shader.src_vs = vs;
                asset.shaders[_i] = shader;
            }
        );
    }

}
