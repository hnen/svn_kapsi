
function Mesh() {

    Object.defineProperty(this, "vertices", { set: function (v) { this.setVertices(v); }, get: function() { return this._vertices_vec; } });
    Object.defineProperty(this, "normals", { set: function (v) { this.setNormals(v); }, get: function() { return this._normals_vec; } });
    Object.defineProperty(this, "uv", { set: function (v) { this.setUv(v); }, get: function() { return this._uv_vec; } });
    Object.defineProperty(this, "triangles", { set: function(i) { this._setTriangles(i); }, get: function() { return this._tris; } });

    this.setVertices = function(v) {
        vf = [];
        for(var i = 0; i < v.length; i++) {
            vf.push(v[i].x);
            vf.push(v[i].y);
            vf.push(v[i].z);
        }
        this._vertices = new Float32Array(vf);
        this._vertices_vec = v;
        this.vb_vertices = gl_createVB(this._vertices, 3);
    }

    this.setNormals = function(v) {
        vf = [];
        for(var i = 0; i < v.length; i++) {
            vf.push(v[i].x);
            vf.push(v[i].y);
            vf.push(v[i].z);
        }
        this._normals = new Float32Array(vf);
        this._normals_vec = v;
        this.vb_normals = gl_createVB(this._normals, 3);
    }

    this.setUv = function(v) {
        vf = [];
        for(var i = 0; i < v.length; i++) {
            vf.push(v[i].x);
            vf.push(v[i].y);
        }
        this._uv = new Float32Array(vf);
        this._uv_vec = v;
        this.vb_uv = gl_createVB(this._uv, 2);
    }

    this.SetTriangles = function(triangles, submesh) {
        if (!this.ibs) {
            this.ibs = [];
        }
        while (submesh >= this.ibs.length) {
            this.ibs.push(null);
        }
        this.ibs[submesh] = gl_createIB(new Uint32Array(triangles));
    }

    this._setTriangles = function(i) {
        //this._tris = new Uint32Array(i);
        //this.ib = gl_createIB(this._tris);
        this.SetTriangles(i, 0);
    }

    this.Dispose = function() {
        if (this.vb_uv) {
            gl_destroyVB(this.vb_uv);
        }
        if (this.vb_normals) {
            gl_destroyVB(this.vb_normals);
        }
        if (this.vb_vertices) {
            gl_destroyVB(this.vb_vertices);
        }
        for (var i = 0; i < this.ibs.length; i++) {
            gl_destroyVB(this.ibs[i]);
        }
    }

    this.RecalculateNormals = function() {
        var verts = this.vertices;
        var tris = this.triangles;
        var nrms = [];
        for(var i = 0; i < verts.length; i++) {
            nrms.push({x: 0, y: 0, z: 0});
        }
        for(var i = 0; i < tris.length; i+=3) {
            var i0 = tris[i+0];
            var i1 = tris[i+1];
            var i2 = tris[i+2];
            var p0 = verts[tris[i+0]];
            var p1 = verts[tris[i+1]];
            var p2 = verts[tris[i+2]];
            var v0 = Vector3.Sub(p1,p0);
            var v1 = Vector3.Sub(p2,p0);
            var n = Vector3.Normalized(Vector3.Cross(v1, v0));
            nrms[i0] = Vector3.Add(nrms[i0], n);
            nrms[i1] = Vector3.Add(nrms[i1], n);
            nrms[i2] = Vector3.Add(nrms[i2], n);
        }
        for(var i = 0; i < verts.length; i++) {
            nrms[i] = Vector3.Normalized(nrms[i]);
        }
        this.normals = nrms;
    }

}
