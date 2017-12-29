function sceneprocessor_preprocess(scene) {
    p_sceneprocessor_createParentPointers(scene);
    p_sceneprocessor_makeAssetReferences(scene);
}

function sceneprocessor_process(scene) {
    p_sceneprocessor_createParentPointers(scene);
    p_sceneprocessor_computeGlobalTransform(scene, mat_identity());
}

function scene_getByRefId(obj, refId) {
    if (obj.ref == refId) {
        return obj;
    }
    for (var i in obj) {
        var cmp = obj[i];
        if (cmp.ref == refId) {
            return cmp;
        }
    }
    for (var i in obj.children) {
        var cobj = obj.children[i];
        var result = scene_getByRefId(cobj, refId);
        if (result) {
            return result;
        }
    }
    return null;
}

function sceneprocessor_getMatrixFromTransform(tf) {
    var pos = tf.localPosition;
    var rot = tf.localRotation;
    var sca = tf.localScale;
    var mat_obj_tf = mat_posrotscale(
        [pos.x, pos.y, pos.z], [rot.w, rot.x, rot.y, rot.z], [
        sca.x, sca.y, sca.z
        ]
    );
    return mat_obj_tf;
}

function p_sceneprocessor_computeGlobalTransform(obj, mat) {
    for (var i in obj.children) {
        var cobj = obj.children[i];
        var tf = cobj.transform;
        var mat_obj_tf = mat_mul(sceneprocessor_getMatrixFromTransform(tf), mat);

        tf.localToWorldMatrix = mat_obj_tf;
        tf.worldToLocalMatrix = mat_inverse(mat_obj_tf);
        cpyTo(unitymatrix_prototype, tf.localToWorldMatrix);
        cpyTo(unitymatrix_prototype, tf.worldToLocalMatrix);

        cobj.position = vec3_tf4([0, 0, 0], tf.localToWorldMatrix)
        p_sceneprocessor_computeGlobalTransform(cobj, mat_obj_tf);
    }
}

function p_sceneprocessor_createParentPointers(obj) {
    for (var i in obj) {
        var component = obj[i];
        if (i != 'children') {
            if (i == 'transform') {
                cpyTo(unitytransform_prototype, component);
            }
            component.gameObject = obj;
            component.transform = obj.transform;
            component.GetComponent = unitywrapper_getComponentFunc;
        }
    }
    for (var i in obj.children) {
        var cobj = obj.children[i];
        cobj.transform.parent = obj.transform;
        p_sceneprocessor_createParentPointers(cobj);
    }
}

function p_sceneprocessor_makeAssetReferences(obj) {
    for (var i in obj) {
        var component = obj[i];
        if (i != 'children') {
            for (var j in component) {
                var property = component[j];
                if (property.hasOwnProperty('__assetRef')) {
                    component[j] = assets_getAsset(property.__assetRef);
                } else if (property.hasOwnProperty('__sceneRef')) {
                    component[j] = scene_getByRefId(scene, property.__sceneRef);
                }
            }
        }
    }
    for (var i in obj.children) {
        var cobj = obj.children[i];

        p_sceneprocessor_makeAssetReferences(cobj);
    }
}

function sceneprocessor_runScripts(obj, method) {
    for (var i in obj.children) {
        var cobj = obj.children[i];
        for (var j in cobj) {
            var component = cobj[j];
            if (component.hasOwnProperty(method)) {
                component[method](component);
            }
        }
        sceneprocessor_runScripts(cobj, method);
    }
}

function sceneprocessor_copyGameObject(src) {
    var retObj = {};
    var children = [];
    for (var i in src) {
        var component = src[i];
        if (i == 'children') {
            for (var j in component) {
                children.push(sceneprocessor_copyGameObject(component[j]));
            }
        } else {
            retObj[i] = cpy(component);
        }
    }
    retObj.children = children;
    return retObj;
}
