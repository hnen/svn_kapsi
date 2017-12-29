var Mathf = function() {};
var Time = function() {};

var False = false;
var True = true;

var p_starttime;

function unitywrapper_init() {
    Mathf.Sin = Math.sin;
    Mathf.Round = Math.round;
    Mathf.Abs = Math.abs;
    Mathf.Floor = Math.floor;
    p_starttime = timer() / 1000;
}

function unitywrapper_update() {
    Time.time = timer() / 1000 - p_starttime;
}

function unitywrapper_getComponentFunc(typename) {
    return this.gameObject[typename];
}

function Vector3(x, y, z) {
    this.x = x;
    this.y = y;
    this.z = z;
}

Vector3.Magnitude = function(v) {
	return Math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3.Cross = function(a, b) {
	return new Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

Vector3.Dot = function(a, b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3.Normalize = function(v) {
	var m = Vector3.Magnitude(v);
	return new Vector3(v.x / m, v.y / m, v.z / m);
}

function GameObject() {}

GameObject.Instantiate = function(prefab) {
	var ret = sceneprocessor_copyGameObject(prefab);
	sceneprocessor_process(ret);
	return ret;
}

function Random() {}

Random.Range = function(a, b) {
	return a + Math.random() * (b - a);
}

var unitymatrix_prototype = {
	MultiplyVector: function(v) {
		return vec3_tf3([v.x, v.y, v.z], this);
	}
};

var unitytransform_prototype = {
	SetParent: function(tf) {
		var old_local_to_world = mat_identity();
		if (this.gameObject.parent) {
			var i = this.parent.gameObject.children.indexOf(this);
			if (i >= 0) {
				this.parent.gameObject.children.splice(i, 1);
			}
			old_local_to_world = mat_inverse(this.p_ComputeLocalToWorld());
		}
		if (tf.gameObject.children.indexOf(this.gameObject) < 0) {
			tf.gameObject.children.push(this.gameObject);
			this.parent = tf;
			var ntf = tf.p_ComputeLocalToWorld();
			var ttf = mat_mul(ntf, old_local_to_world);
			var trs = mat_decompose(ttf);
			this.localPosition = {x: trs[0][0], y: trs[0][1], z: trs[0][2]};
			this.localRotation = {w: trs[1][0], x: trs[1][1], y: trs[1][2], z: trs[1][3]};
			this.localScale = {x:trs[2][0], y: trs[2][1], z: trs[2][2]};
		}
	},

	p_ComputeLocalToWorld: function() {
		var tf = this;
		var mat = mat_identity();
		while (tf) {
			mat = mat_mul(mat, sceneprocessor_getMatrixFromTransform(tf));
			tf = tf.parent;
		}
		return mat;
	}

};
