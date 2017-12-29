var Mathf = function() {};
var Time = function() {};

var System = function() {};
System.Math = Mathf;

var UnityEngine = {};
var Application = {};

Application.isEditor = false;

var False = false;
var True = true;

Array.prototype.Add = Array.prototype.push;
Array.prototype.ToUnityArray = function() { return this; };
Array.prototype.ToArray = function() { return this; };
Object.defineProperty(Array.prototype, "Length", {
    get: function() { return this.length; }
});
Object.defineProperty(Array.prototype, "Count", {
    get: function() { return this.length; }
});
Object.defineProperty(String.prototype, "Length", {
    get: function() { return this.length; }
});

var char = {};
char.IsLower = function(a) {
    return a == a.toLowerCase();
}
char.IsUpper = function(a) {
    return a == a.toUpperCase();
}
char.ToUpper = function(a) {
    return a.toUpperCase();
}
char.ToLower = function(a) {
    return a.toLowerCase();
}

var p_starttime;

function unitywrapper_init() {
    Mathf.Sin = Math.sin;
    Mathf.Cos = Math.cos;
    Mathf.Round = Math.round;
    Mathf.Abs = Math.abs;
    Mathf.Floor = Math.floor;
    Mathf.Ceil = Math.ceil;
    Mathf.Pow = Math.pow;
    Mathf.Log10 = Math.log10;
    Mathf.Log = Math.log;
    Mathf.Sqrt = Math.sqrt;
    Mathf.Atan = Math.atan;
    Mathf.FloorToInt = Math.floor;
    Mathf.Repeat = function(a,b) { return a%b; };

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

    Object.defineProperty(this, "Magnitude", { get: function () { return Math.sqrt(this.x * this.x + this.y * this.y + this.z * this.z); } });
    Object.defineProperty(this, "SqrMagnitude", { get: function () { return this.x * this.x + this.y * this.y + this.z * this.z; } });
    Object.defineProperty(this, "Normalized", { get: function () { return this.Mul(1.0 / Math.sqrt(this.x * this.x + this.y * this.y + this.z * this.z)); } });

    this.Add = function(b) {
        assert(!this.isNan(), "NaN Vector!")
        var result = new Vector3(this.x + b.x, this.y + b.y, this.z + b.z);
        assert(!result.isNan(), "NaN Vector!")
        return result;
    }

    this.Mul = function(a) {
        assert(!this.isNan(), "NaN Vector!")
        var result = new Vector3(this.x * a, this.y * a, this.z * a);
        assert(!result.isNan(), "NaN Vector!")
        return result;
    }

    this.Sub = function(b) {
        assert(!this.isNan(), "NaN Vector!")
        var result = new Vector3(this.x - b.x, this.y - b.y, this.z - b.z);
        assert(!result.isNan(), "NaN Vector!")
        return result;
    }

    this.Cross = function(b) {
        return new Vector3(
    		this.y * b.z - this.z * b.y,
    		this.z * b.x - this.x * b.z,
    		this.x * b.y - this.y * b.x
    	);
    }


    this.isNan = function() {
        return isNaN(this.x) || isNaN(this.y) || isNaN(this.z);
    }

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

Vector3.Sub = function(a, b) {
	return new Vector3(
        a.x - b.x, a.y - b.y, a.z - b.z
	);
}

Vector3.Add = function(a, b) {
	return new Vector3(
        a.x + b.x, a.y + b.y, a.z + b.z
	);
}

Vector3.Dot = function(a, b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3.Normalized = function(v) {
	var m = Vector3.Magnitude(v);
	return new Vector3(v.x / m, v.y / m, v.z / m);
}

var Quaternion = {};

Quaternion.AngleAxis = function(angle, a) {
    var a0 = angle / 360.0 * Math.PI;
    var s = Math.sin(a0);
    return { x: a.x * s, y: a.y * s, z: a.z *s, w: Math.cos(a0) };
};

Quaternion.identity = {x: 0, y: 0, z: 0, w: 1};

Quaternion.Slerp = function(qa, qb, t) {
    // quaternion to return
	var qm = {};
	// Calculate angle between them.
	var cosHalfTheta = qa.w * qb.w + qa.x * qb.x + qa.y * qb.y + qa.z * qb.z;
	// if qa=qb or qa=-qb then theta = 0 and we can return qa
	if (Math.abs(cosHalfTheta) >= 1.0){
		qm.w = qa.w;qm.x = qa.x;qm.y = qa.y;qm.z = qa.z;
		return qm;
	}
	// Calculate temporary values.
	var halfTheta = Math.acos(cosHalfTheta);
	var sinHalfTheta = Math.sqrt(1.0 - cosHalfTheta*cosHalfTheta);
	// if theta = 180 degrees then result is not fully defined
	// we could rotate around any axis normal to qa or qb
	if (Math.abs(sinHalfTheta) < 0.001){ // fabs is floating point absolute
		qm.w = (qa.w * 0.5 + qb.w * 0.5);
		qm.x = (qa.x * 0.5 + qb.x * 0.5);
		qm.y = (qa.y * 0.5 + qb.y * 0.5);
		qm.z = (qa.z * 0.5 + qb.z * 0.5);
		return qm;
	}
	var ratioA = Math.sin((1 - t) * halfTheta) / sinHalfTheta;
	var ratioB = Math.sin(t * halfTheta) / sinHalfTheta;
	//calculate Quaternion.
	qm.w = (qa.w * ratioA + qb.w * ratioB);
	qm.x = (qa.x * ratioA + qb.x * ratioB);
	qm.y = (qa.y * ratioA + qb.y * ratioB);
	qm.z = (qa.z * ratioA + qb.z * ratioB);
	return qm;
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




var Camera = {};
Object.defineProperty(Camera, "main", {
    get: function() {
        var cam = null;
        sceneprocessor_foreachGameObject(scene, function(obj) {
            if (obj.camera && obj.camera.isMainCamera) {
                cam = obj.camera;
            }
        }, false);
        
        return cam;
    }
});
