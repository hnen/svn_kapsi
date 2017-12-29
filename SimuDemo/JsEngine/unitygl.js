
var GL = {};

GL.QUADS = 0;

GL.PushMatrix = function() {}
GL.PopMatrix = function() {}
GL.LoadOrtho = function() {}

GL._curr = -1;
GL.I = 0;
GL._uv = null;
GL._uvs = [];
GL._verts = [];
GL._inds = [];
GL._mat = null;

GL.Begin = function(topology) {
    GL._curr = topology;
    GL._uv = null;
    GL._uvs = [];
    GL._verts = [];
    GL._inds = [];
}

GL.End = function() {
    var m = new Mesh();
    m.vertices = GL._verts;
    m.uv = GL._uvs;
    m.triangles = GL._inds;

    meshrenderer_prepareGlState();
    wgl.disable(wgl.DEPTH_TEST);
    meshrenderer_renderMeshBuffer(m, [GL._mat], 0, GL._matpass, mat_identity(), mat_identity(), mat_identity());

    m.Dispose();
}

GL.TexCoord2 = function(u, v) {
    GL._uv = { x: u, y: v };
}

GL.Vertex3 = function(x, y, z) {
    if (GL._uv) {
        GL._uvs.push(GL._uv);
    } else {
        GL._uvs.push({ x: 0, y: 0 });
    }
    GL._verts.push({x: (x-0.5)*2.0, y: (y-0.5)*2.0, z: (z-0.5)*2.0});
    GL.I++;
    if (GL._curr == GL.QUADS) {
        if (GL.I == 4) {
            var L = GL._verts.length;
            GL._inds.push(L - 4);
            GL._inds.push(L - 3);
            GL._inds.push(L - 2);

            GL._inds.push(L - 2);
            GL._inds.push(L - 1);
            GL._inds.push(L - 4);
            GL.I = 0;
        }
    } else {
        throw "Not implemented"
    }
}
