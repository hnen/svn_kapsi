
function scenerenderer_render(scene) {
    var cameras = [];
    p_scenerenderer_gatherCameras(scene, cameras);

    cameras.sort(function(a,b) { return a.camera.renderOrder - b.camera.renderOrder; });

    for(var i in cameras) {
        var camera = cameras[i];
        p_scenerenderer_renderCamera(scene, camera)
    }

}

function p_scenerenderer_gatherCameras(scene, out_cameras) {
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

    if (!cameraObj.camera.renderFullScreenTextureMat) {
        p_scenerenderer_gatherRenderersForCamera(scene, cameraObj, objects);
        for(var i in objects) {
            var renderer = objects[i];
            p_scenerenderer_renderRenderer(scene, cameraObj, renderer);
        }
    } else {
        var mat = cameraObj.camera.renderFullScreenTextureMat;
        var source = scene_getByRefId(scene, cameraObj.camera.renderCameraToTextureRenderer);
        p_scenerenderer_renderFullScreenTexture(mat, source.gameObject.camera.renderTexture);
    }
}

function p_scenerenderer_setupCamera(cameraObj) {
    var w, h;
    if (cameraObj.camera.renderToTexture) {
        if (!cameraObj.camera.renderTexture) {
            cameraObj.camera.renderTexture = gl_createRenderTexture("rt_" + cameraObj.name, 512, 512);
        }
        GL.bindFramebuffer(GL.FRAMEBUFFER, cameraObj.camera.renderTexture.framebuffer);
        var vprect = cameraObj.camera.viewport;
        var x = vprect.x_min * 512;
        var y = vprect.y_min * 512;
        var w = vprect.x_max * 512 - x;
        var h = vprect.y_max * 512 - y;
        GL.viewport(x,y,w,h);
    } else {
        GL.bindFramebuffer(GL.FRAMEBUFFER, null);
        var vprect = cameraObj.camera.viewport;
        var x = vprect.x_min * GL.drawingBufferWidth;
        var y = vprect.y_min * GL.drawingBufferHeight;
        var w = vprect.x_max * GL.drawingBufferWidth - x;
        var h = vprect.y_max * GL.drawingBufferHeight - y;
        GL.viewport(x,y,w,h);
    }

    p_scenerenderer_clearCamera(cameraObj);
    p_scenerenderer_computeCameraMatrices(cameraObj);
}

function p_scenerenderer_clearCamera(cameraObj) {
    var clearMode = cameraObj.camera.clear;
    if (!(clearMode === 'Nothing')) {
        if (clearMode === 'Depth') {
            GL.clear(GL.DEPTH_BUFFER_BIT);
        } else {
            var clearColor = cameraObj.camera.clearColor;
            GL.clearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
            GL.clear(GL.COLOR_BUFFER_BIT | GL.DEPTH_BUFFER_BIT);
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
        alert("ortho camera not supported yet");
    }

    cam.mat_view = cameraObj.transform.worldToLocalMatrix;
}

function p_scenerenderer_gatherRenderersForCamera(scene, cameraObj, out_objects) {
    for(var i in scene.children) {
        var obj = scene.children[i];
        p_scenerenderer_gatherRenderersForCamera(obj, cameraObj, out_objects);
        if (obj.hasOwnProperty('MeshRenderer')) {
            out_objects.push(obj);
        }
    }
}

function p_scenerenderer_renderRenderer(scene, camera, renderer) {
    meshrenderer_render(renderer, camera)
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

    GL.activeTexture(GL.TEXTURE0);
    //GL.bindTexture(GL.TEXTURE_2D, fb.texture);
    GL.bindTexture(GL.TEXTURE_2D, texture.texture);
    GL.uniform1i(shader.shader.uniform_mainTex, 0);
    GL.uniform4f(shader.shader.uniform_mainTexST, 1, 1, 0, 0);
    //GL.uniform2f(shader.uniform_size, fb.size_x, fb.size_y);
    //if (!disableblend) {
    //    GL.enable(GL.BLEND);
    //    GL.blendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA);
    //} else {
        GL.disable(GL.BLEND);
    //}

    GL.drawArrays(GL.TRIANGLE_STRIP, 0, p_scenerenderer_vb_quad.numItems);
    //GL.disableVertexAttribArray(shader.attrib_pos);
    GL.bindBuffer(GL.ARRAY_BUFFER, null);
}
