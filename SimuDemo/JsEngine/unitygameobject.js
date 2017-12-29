
function GameObject() {}

GameObject.Instantiate = function(prefab) {
	if (prefab.hasOwnProperty("__sceneRef")) {
		prefab = scene_getByRefId(scene, prefab.__sceneRef);
	}
	var ret = sceneprocessor_copyGameObject(prefab);
	sceneprocessor_process(ret);

	sceneprocessor_foreachGameObject(ret, function(obj) {
		obj._startCalled = false;
	}, true);

	sceneprocessor_foreachGameObject(ret, function(obj) {
		if (!obj._startCalled) {
			obj._startCalled = true;
			for(var i in obj) {
				if (obj[i].hasOwnProperty('Start')) {
					obj[i].Start();
				}
			}
		}
	}, false);

	sceneprocessor_runScripts(ret, 'Awake', true);
	return ret;
}

GameObject.Destroy = function(obj) {
    GameObject.DestroyImmediate(obj);
}

GameObject.DestroyImmediate = function(obj) {
    var parent = obj.transform.parent.gameObject;
    var chi = parent.children.indexOf(obj);
    parent.children.splice(chi, 1);
}

var unitygameobject_prototype = {

    SetActive: function(b) {
        if (b != this.enabled) {
            if (!b) {
                sceneprocessor_runScripts(this, 'OnDisable');
            }
            this.enabled = b;
            if (b) {
				sceneprocessor_foreachGameObject(this, function(obj) {
					if (!obj._startCalled) {
						obj._startCalled = true;
						for(var i in obj) {
							if (obj[i].hasOwnProperty('Start')) {
								obj[i].Start();
							}
						}
					}
				}, false);
                sceneprocessor_runScripts(this, 'OnEnable');
            }
        }
    },

	GetComponent: function(comp) {
		return this[comp];
	},

    p_isActiveInHierarchy: function() {
        var tf = this.transform;

        while(tf) {
            if (!tf.gameObject.enabled) {
                return false;
            }
            tf = tf.parent;
        }

        return true;
    }
};

var unitygameobject_properties = {

    activeSelf: { get: function() { return this.enabled; }},
    activeInHierarchy: { get: function() { return this.p_isActiveInHierarchy(); }},

}
