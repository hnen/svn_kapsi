
function scenerenderer_render(scene) {
    var cameras = [];
    p_scenerenderer_gatherCameras(scene, cameras);

    cameras.sort(function(a,b) { return a.camera.renderOrder - b.camera.renderOrder; });

    p_sceneprocessor_cacheGlobalTransform(scene, mat_identity());

    //for(var i in cameras) {
    for(var i = 0; i < cameras.length; i++) {
        var camera = cameras[i];
        p_scenerenderer_renderCamera(scene, camera)
        for(var c in camera) {
            var component = camera[c];
            for(var el in component) {
                if (el == "OnPostRender") {
                    component[el]();
                }
            }
        }
    }

}

function p_scenerenderer_gatherCameras(scene, out_cameras) {
    if (!scene.enabled) {
        return;
    }
    for(var i in scene.children) {
        var obj = scene.children[i];
        p_scenerenderer_gatherCameras(obj, out_cameras);
        if (obj.hasOwnProperty('camera')) {
            out_cameras.push(obj);
        }
    }
}

function p_scenerenderer_renderCamera(scene, cameraObj) {
    var objects = [];
    p_scenerenderer_setupCamera(cameraObj);

    if (cameraObj.camera.cullingMask != 0) {
        p_scenerenderer_gatherRenderersForCamera(scene, cameraObj, objects);
        for(var i = 0; i < objects.length; i++) {
            var renderer = objects[i];
            p_scenerenderer_renderRenderer(scene, cameraObj, renderer);
        }
    }
}

function p_scenerenderer_setupCamera(cameraObj) {
    var w, h;
    if (cameraObj.camera.targetTexture) {
        wgl.bindFramebuffer(wgl.FRAMEBUFFER, cameraObj.camera.targetTexture.rt.framebuffer);
        var vprect = cameraObj.camera.viewport;
        var rt = cameraObj.camera.targetTexture.rt;
        var x = vprect.x_min * rt.width;
        var y = vprect.y_min * rt.height;
        var w = vprect.x_max * rt.width - x;
        var h = vprect.y_max * rt.height - y;
        wgl.viewport(x,y,w,h);
    } else {
        wgl.bindFramebuffer(wgl.FRAMEBUFFER, null);
        var vprect = cameraObj.camera.viewport;
        var x = vprect.x_min * wgl.drawingBufferWidth;
        var y = vprect.y_min * wgl.drawingBufferHeight;
        var w = vprect.x_max * wgl.drawingBufferWidth - x;
        var h = vprect.y_max * wgl.drawingBufferHeight - y;
        wgl.viewport(x,y,w,h);
    }

    p_scenerenderer_clearCamera(cameraObj);
    p_scenerenderer_computeCameraMatrices(cameraObj);
}

function p_scenerenderer_clearCamera(cameraObj) {
    var clearMode = cameraObj.camera.clear;
    if (!(clearMode === 'Nothing')) {
        if (clearMode === 'Depth') {
            wgl.clear(wgl.DEPTH_BUFFER_BIT);
        } else {
            var clearColor = cameraObj.camera.clearColor;
            wgl.clearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
            wgl.clear(wgl.COLOR_BUFFER_BIT | wgl.DEPTH_BUFFER_BIT);
        }
    }
}

function p_scenerenderer_computeCameraMatrices(cameraObj) {
    var projtype = cameraObj.camera.projection;
    var cam = cameraObj.camera;
    if (projtype === "perspective") {
        cam.mat_proj = mat_perspective(
            cam.fov / 180.0 * Math.PI, 16.0 / 9.0, cam.near, cam.far
        );
    } else {
        var aspect = wgl.drawingBufferWidth / wgl.drawingBufferHeight;
        var X = cam.orthographicSize * aspect;
        var Y = cam.orthographicSize;
        cam.mat_proj = mat_ortho(-X, X, -Y, Y, cam.near, cam.far);
        //alert("ortho camera not supported yet");
    }

    cam.mat_view = cameraObj.transform.worldToLocalMatrix;
}

function p_scenerenderer_gatherRenderersForCamera(scene, cameraObj, out_objects) {
    if (!scene.enabled) {
        return;
    }
    for(var i in scene.children) {
        var obj = scene.children[i];
        p_scenerenderer_gatherRenderersForCamera(obj, cameraObj, out_objects);
        if (obj.hasOwnProperty('MeshRenderer')) {
            out_objects.push(obj);
        }
    }
}

function p_scenerenderer_renderRenderer(scene, camera, renderer) {
    meshrenderer_renderWithCamera(renderer, camera)
}

var p_scenerenderer_vb_quad = null;
var p_scenerenderer_vb_quad_uv = null;

function p_scenerenderer_renderFullScreenTexture(mat, texture) {
    var shader = mat.shader;

    if (!p_scenerenderer_vb_quad) {
        p_scenerenderer_vb_quad = gl_createQuad();
        p_scenerenderer_vb_quad_uv = gl_createQuadUv();
    }

    meshrenderer_prepareShader(shader, mat_identity(), p_scenerenderer_vb_quad, null, p_scenerenderer_vb_quad_uv);

    wgl.activeTexture(wgl.TEXTURE0);
    //wgl.bindTexture(wgl.TEXTURE_2D, fb.texture);
    wgl.bindTexture(wgl.TEXTURE_2D, texture.texture);
    wgl.uniform1i(shader.shader.uniform_mainTex, 0);
    wgl.uniform4f(shader.shader.uniform_mainTexST, 1, 1, 0, 0);
    //wgl.uniform2f(shader.uniform_size, fb.size_x, fb.size_y);
    //if (!disableblend) {
    //    wgl.enable(wgl.BLEND);
    //    wgl.blendFunc(wgl.SRC_ALPHA, wgl.ONE_MINUS_SRC_ALPHA);
    //} else {
        wgl.disable(wgl.BLEND);
    //}

    wgl.drawArrays(wgl.TRIANGLE_STRIP, 0, p_scenerenderer_vb_quad.numItems);
    //wgl.disableVertexAttribArray(shader.attrib_pos);
    wgl.bindBuffer(wgl.ARRAY_BUFFER, null);
}
