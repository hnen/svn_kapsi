function sceneprocessor_preprocess(scene) {
    p_sceneprocessor_createParentPointers(scene, true);
    p_sceneprocessor_makeAssetReferences(scene);
}

function sceneprocessor_process(scene) {
    p_sceneprocessor_createParentPointers(scene, false);
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

function p_sceneprocessor_cacheGlobalTransform(obj, mat) {
    //for (var i in obj.children) {
    var xtf = obj.transform;
    var mat_obj_tf = mat_mul(sceneprocessor_getMatrixFromTransform(xtf), mat);
    xtf.cached_localToWorldMatrix = mat_obj_tf;
    xtf.cached_worldToLocalMatrix = mat_inverse(mat_obj_tf);
    
    for (var i = 0; i < obj.children.length; i++) {
        var cobj = obj.children[i];
        p_sceneprocessor_cacheGlobalTransform(cobj, mat_obj_tf);
    }
}


function p_sceneprocessor_createParentPointers(obj, force) {
    if (!force && !obj.enabled) {
        return;
    }
    cpyTo(unitygameobject_prototype, obj);
    for(var prop in unitygameobject_properties) {
        if (obj.hasOwnProperty(prop)) {
            break;
        }
        Object.defineProperty(obj, prop, unitygameobject_properties[prop]);
    }
    for (var i in obj) {
        var component = obj[i];
        if (i != 'children') {
            if (i == 'transform') {
                cpyTo(unitytransform_prototype, component);
                for(var prop in unitytransform_properties) {
                    Object.defineProperty(component, prop, unitytransform_properties[prop]);
                }
            }
            if (i == 'AudioSource') {
                if (!component.hasOwnProperty("time")) {
                    Object.defineProperty(component, "time", {
                            get: function() {
                                return t();
                            },
                            set: function(v) {
                                return set_t(v);
                            }
                        } );
                }
            }
            component.gameObject = obj;
            component.transform = obj.transform;
            component.GetComponent = unitywrapper_getComponentFunc;
        }
    }
    //for (var i in obj.children) {
    for(var i = 0; i < obj.children.length; i++) {
        var cobj = obj.children[i];
        cobj.transform._parent = obj.transform;
        p_sceneprocessor_createParentPointers(cobj, force);
    }
}

function p_sceneprocessor_makeAssetReferences(obj) {
    for (var i in obj) {
        var component = obj[i];
        if (i != 'children') {
            for (var j in component) {
                var property = component[j];
                if (Array.isArray(property)) {
                    for (var k = 0; k < property.length; k++) {
                        var _property = property[k];
                        if(_property == null) {}
                        else if (_property.hasOwnProperty('__assetRef')) {
                            property[k] = assets_getAsset(_property.__assetRef);
                        } else if (_property.hasOwnProperty('__sceneRef')) {
                            property[k] = scene_getByRefId(scene, _property.__sceneRef);
                        }
                    }
                } else {
                    if(property == null) {}
                    else if (property.hasOwnProperty('__assetRef')) {
                        component[j] = assets_getAsset(property.__assetRef);
                    } else if (property.hasOwnProperty('__sceneRef')) {
                        component[j] = scene_getByRefId(scene, property.__sceneRef);
                    }
                }
            }
        }
    }
    for (var i in obj.children) {
        var cobj = obj.children[i];

        p_sceneprocessor_makeAssetReferences(cobj);
    }
}

var sceneprocessor_cnt;
var sceneprocessor_upd;
function sceneprocessor_runScripts(obj, method, force) {
    sceneprocessor_cnt++;
    if (!obj.enabled && !force) {
        return;
    }
    for (var i in obj) {
        var component = obj[i];
        if (component.hasOwnProperty(method)) {
            sceneprocessor_upd++;
            component[method](component);
        }
    }
    for (var j in obj.children) {
        sceneprocessor_runScripts(obj.children[j], method, force);
    }
}


function sceneprocessor_foreachGameObject(obj, method, force) {
    if (!obj.enabled && !force) {
        return;
    }
    method(obj);
    for (var j in obj.children) {
        sceneprocessor_foreachGameObject(obj.children[j], method, force);
    }
}

function sceneprocessor_copyGameObject(src) {
    var retObj = {};
    var children = [];
    for (var i in src) {
        var component = src[i];
        if (i == 'children') {
            //for (var j in component) {
            for (var j = 0; j < component.length; j++) {
                children.push(sceneprocessor_copyGameObject(component[j]));
            }
        } else {
            retObj[i] = cpy(component);
        }
    }
    retObj.children = children;
    return retObj;
}
