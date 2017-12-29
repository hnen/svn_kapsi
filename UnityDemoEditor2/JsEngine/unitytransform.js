
var unitytransform_prototype = {
    SetParent: function(tf) {
        var old_local_to_world = mat_identity();
        if (this.gameObject._parent) {
            var i = this._parent.gameObject.children.indexOf(this);
            if (i >= 0) {
                this._parent.gameObject.children.splice(i, 1);
            }
            old_local_to_world = mat_inverse(this.p_ComputeLocalToWorld());
        }
        if (tf.gameObject.children.indexOf(this.gameObject) < 0) {
            tf.gameObject.children.push(this.gameObject);
            this._parent = tf;
            var ntf = tf.p_ComputeLocalToWorld();
            var ttf = mat_mul(ntf, old_local_to_world);
            var trs = mat_decompose(ttf);
            this.localPosition = new Vector3(trs[0][0], trs[0][1], trs[0][2]);
            this.localRotation = {w: trs[1][0], x: trs[1][1], y: trs[1][2], z: trs[1][3]};
            this.localScale = {x:trs[2][0], y: trs[2][1], z: trs[2][2]};
        }
    },

    GetChild: function(i) {
        return this.gameObject.children[i].transform;
    },

    p_ComputeLocalToWorld: function() {
        var tf = this;
        var mat = mat_identity();
        while (tf) {
            mat = mat_mul(mat, sceneprocessor_getMatrixFromTransform(tf));
            tf = tf.parent;
        }
        return mat;
    },

    p_getGlobalPosition: function() {
        var localToWorld = this.p_ComputeLocalToWorld();
        var vec = mat_decompose(localToWorld)[0];
        return new Vector3( vec[0], vec[1], vec[2] );
    },

    p_setGlobalPosition: function(v) {
        var worldToLocal = mat_inverse(this.parent.p_ComputeLocalToWorld());
        var vec = vec4_tf([v.x, v.y, v.z, 1], worldToLocal);
        this.localPosition = new Vector3(vec[0], vec[1], vec[2]);
    },

    p_getGlobalRotation: function() {
        var localToWorld = mat_inverse(this.p_ComputeLocalToWorld());
        var rot = mat_decompose(localToWorld)[1];
        return { x: rot[1], y: rot[2], z: rot[3], w: rot[0] };
    },

    p_setGlobalRotation: function(v) {
        var worldToLocal = mat_inverse(this.parent.p_ComputeLocalToWorld());
        var worldToLocalQuat = quat_fromMat(worldToLocal);
        var rot = quat_mul([v.w, v.x, v.y, v.z], worldToLocalQuat);
        this.localRotation = {w: rot[0], x: rot[1], y: rot[2], z: rot[3]};
        //var vec = vec4_tf([v.x, v.y, v.z, 1], worldToLocal);
        //this.localPosition = new Vector3(vec[0], vec[1], vec[2]);
    },

    p_getChildCount: function() {
        return this.gameObject.children.length;
    },

    p_getDirLeft: function() {
        var localToWorld = this.p_ComputeLocalToWorld();
        return new Vector3(localToWorld[0], localToWorld[1], localToWorld[2]);
    },
    p_getDirUp: function() {
        var localToWorld = this.p_ComputeLocalToWorld();
        return new Vector3(localToWorld[4], localToWorld[5], localToWorld[6]);
    },
    p_getDirForward: function() {
        var localToWorld = this.p_ComputeLocalToWorld();
        return new Vector3(localToWorld[8], localToWorld[9], localToWorld[10]);
    },

    p_setDirLeft: function(v) {
        v = Vector3.Normalized(v);

        var X = v;
        //var Y = this.p_getDirUp();
        var Y = new Vector3(0,1,0);
        var Z = new Vector3(0,0,1);
        //var Z = this.p_getDirForward();

        Z = Vector3.Normalized(Vector3.Cross(X,Y));
        Y = Vector3.Normalized(Vector3.Cross(Z,X));

        var mat = mat_inverse( mat_orient([X.x, X.y, X.z], [Y.x, Y.y, Y.z], [Z.x, Z.y, Z.z]) );
        var rot = quat_fromMat(mat);
        this.p_setGlobalRotation({w:rot[0], x:rot[1], y:rot[2], z:rot[3]});
    },
    p_setDirUp: function(v) {
        v = Vector3.Normalized(v);

        var X = new Vector3(1,0,0);
        var Y = v;
        var Z = new Vector3(0,0,1);

        X = Vector3.Normalized(Vector3.Cross(Y,Z));
        Z = Vector3.Normalized(Vector3.Cross(X,Y));

        var mat = mat_inverse(mat_orient([X.x, X.y, X.z], [Y.x, Y.y, Y.z], [Z.x, Z.y, Z.z]));
        var rot = quat_fromMat(mat);
        this.p_setGlobalRotation({w:rot[0], x:rot[1], y:rot[2], z:rot[3]});
    },
    p_setDirForward: function(v) {
        v = Vector3.Normalized({x: v.x, y: v.y, z: v.z});

        var X = new Vector3(1,0,0);
        var Y = new Vector3(0,1,0);
        var Z = v;

        X = Vector3.Normalized(Vector3.Cross(Y,Z));
        Y = Vector3.Normalized(Vector3.Cross(Z,X));

        var mat = mat_inverse(mat_orient([X.x, X.y, X.z], [Y.x, Y.y, Y.z], [Z.x, Z.y, Z.z]));
        var rot = quat_fromMat(mat);
        this.p_setGlobalRotation({w:rot[0], x:rot[1], y:rot[2], z:rot[3]});
    }

};

var unitytransform_properties = {
    localToWorldMatrix: { get: function() { return this.p_ComputeLocalToWorld(); }},
    worldToLocalMatrix: { get: function() { return mat_inverse(this.p_ComputeLocalToWorld()); }},
    position: { get: function() { return this.p_getGlobalPosition(); }, set: function(v) { this.p_setGlobalPosition(v); } },
    rotation: { get: function() { return this.p_getGlobalRotation(); }, set: function(v) { this.p_setGlobalRotation(v); } },
    childCount: { get: function() { return this.p_getChildCount(); } },
    parent: {get: function() { return this._parent; }, set: function(v) { this.SetParent(v); } },

    right: {get: function() { return this.p_getDirLeft(); }, set: function(v) { this.p_setDirLeft(v); } },
    up: {get: function() { return this.p_getDirUp(); }, set: function(v) { this.p_setDirUp(v); } },
    forward: {get: function() { return this.p_getDirForward(); }, set: function(v) { this.p_setDirForward(v); } }



};
