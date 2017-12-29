
function mat_identity() {
    return [1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1];
}

function mat_scale(v) {
    return [v[0],	0,	 0,   0,
                0,v[1],	  0,   0,
                0,	 0,v[2],   0,
                0,	 0,	  0,   1];
}

function mat_orient(x,y,z) {
    return [x[0], x[1], x[2], 0,
    y[0], y[1], y[2], 0,
    z[0], z[1], z[2], 0,
    0 ,  0  ,  0  , 1];
}

function mat_translate(v) {
    return [1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    v[0],v[1],v[2],1];
}

function mat_posrotscale(vec_pos, quat_rot, vec_scale) {
    return mat_mul(mat_scale(vec_scale), mat_mul(mat_inverse(mat_quat(quat_rot)), mat_translate(vec_pos)));
}

function mat_transpose(m) {
    return [
        m[0], m[4], m[8], m[12],
        m[1], m[5], m[9], m[13],
        m[2], m[6], m[10], m[14],
        m[3], m[7], m[11], m[15],
        ];
}

function mat_quat(quat) {
    var qw = quat[0];
    var qx = quat[1];
    var qy = quat[2];
    var qz = quat[3];

    var m = 1.0/Math.sqrt(qx*qx+qy*qy+qz*qz+qw*qw);
    qw *= m;
    qx *= m;
    qy *= m;
    qz *= m;

    var qw2 = qw*qw;
    var qx2 = qx*qx;
    var qy2 = qy*qy;
    var qz2 = qz*qz;
    return [
        1 - 2*qy2 - 2*qz2, 2*qx*qy - 2*qz*qw, 2*qx*qz + 2*qy*qw, 0,
        2*qx*qy + 2*qz*qw, 1 - 2*qx2 - 2*qz2, 2*qy*qz - 2*qx*qw, 0,
        2*qx*qz - 2*qy*qw, 2*qy*qz + 2*qx*qw, 1 - 2*qx2 - 2*qy2, 0,
        0, 0, 0, 1];
}

function mat_mul(a,b) {
    return [vec4_dot([a[0+0],a[1+0],a[2+0],a[3+0]],[b[0+0],b[4+0],b[8+0],b[12+0]]),
             vec4_dot([a[0+0],a[1+0],a[2+0],a[3+0]],[b[0+1],b[4+1],b[8+1],b[12+1]]),
             vec4_dot([a[0+0],a[1+0],a[2+0],a[3+0]],[b[0+2],b[4+2],b[8+2],b[12+2]]),
             vec4_dot([a[0+0],a[1+0],a[2+0],a[3+0]],[b[0+3],b[4+3],b[8+3],b[12+3]]),

             vec4_dot([a[0+4],a[1+4],a[2+4],a[3+4]],[b[0+0],b[4+0],b[8+0],b[12+0]]),
             vec4_dot([a[0+4],a[1+4],a[2+4],a[3+4]],[b[0+1],b[4+1],b[8+1],b[12+1]]),
             vec4_dot([a[0+4],a[1+4],a[2+4],a[3+4]],[b[0+2],b[4+2],b[8+2],b[12+2]]),
             vec4_dot([a[0+4],a[1+4],a[2+4],a[3+4]],[b[0+3],b[4+3],b[8+3],b[12+3]]),

             vec4_dot([a[0+8],a[1+8],a[2+8],a[3+8]],[b[0+0],b[4+0],b[8+0],b[12+0]]),
             vec4_dot([a[0+8],a[1+8],a[2+8],a[3+8]],[b[0+1],b[4+1],b[8+1],b[12+1]]),
             vec4_dot([a[0+8],a[1+8],a[2+8],a[3+8]],[b[0+2],b[4+2],b[8+2],b[12+2]]),
             vec4_dot([a[0+8],a[1+8],a[2+8],a[3+8]],[b[0+3],b[4+3],b[8+3],b[12+3]]),

             vec4_dot([a[0+12],a[1+12],a[2+12],a[3+12]],[b[0+0],b[4+0],b[8+0],b[12+0]]),
             vec4_dot([a[0+12],a[1+12],a[2+12],a[3+12]],[b[0+1],b[4+1],b[8+1],b[12+1]]),
             vec4_dot([a[0+12],a[1+12],a[2+12],a[3+12]],[b[0+2],b[4+2],b[8+2],b[12+2]]),
             vec4_dot([a[0+12],a[1+12],a[2+12],a[3+12]],[b[0+3],b[4+3],b[8+3],b[12+3]])];
    /*
    return [a[0+0]*b[0+0]+a[1+0]*b[4+0]+a[2+0]*b[8+0]+a[3+0]*b[12+0],
    a[0+0]*b[0+1]+a[1+0]*b[4+1]+a[2+0]*b[8+1]+a[3+0]*b[12+1],
    a[0+0]*b[0+2]+a[1+0]*b[4+2]+a[2+0]*b[8+2]+a[3+0]*b[12+2],
    a[0+0]*b[0+3]+a[1+0]*b[4+3]+a[2+0]*b[8+3]+a[3+0]*b[12+3],

    a[0+4]*b[0+0]+a[1+4]*b[4+0]+a[2+4]*b[8+0]+a[3+4]*b[12+0],
    a[0+4]*b[0+1]+a[1+4]*b[4+1]+a[2+4]*b[8+1]+a[3+4]*b[12+1],
    a[0+4]*b[0+2]+a[1+4]*b[4+2]+a[2+4]*b[8+2]+a[3+4]*b[12+2],
    a[0+4]*b[0+3]+a[1+4]*b[4+3]+a[2+4]*b[8+3]+a[3+4]*b[12+3],

    a[0+8]*b[0+0]+a[1+8]*b[4+0]+a[2+8]*b[8+0]+a[3+8]*b[12+0],
    a[0+8]*b[0+1]+a[1+8]*b[4+1]+a[2+8]*b[8+1]+a[3+8]*b[12+1],
    a[0+8]*b[0+2]+a[1+8]*b[4+2]+a[2+8]*b[8+2]+a[3+8]*b[12+2],
    a[0+8]*b[0+3]+a[1+8]*b[4+3]+a[2+8]*b[8+3]+a[3+8]*b[12+3],

    a[0+12]*b[0+0]+a[1+12]*b[4+0]+a[2+12]*b[8+0]+a[3+12]*b[12+0],
    a[0+12]*b[0+1]+a[1+12]*b[4+1]+a[2+12]*b[8+1]+a[3+12]*b[12+1],
    a[0+12]*b[0+2]+a[1+12]*b[4+2]+a[2+12]*b[8+2]+a[3+12]*b[12+2],
    a[0+12]*b[0+3]+a[1+12]*b[4+3]+a[2+12]*b[8+3]+a[3+12]*b[12+3]];
    */
}

function mat_rotY(angle) {
    var c = Math.cos(angle);
    var s = Math.sin(angle);
    return [ c, 0, s, 0,
    0, 1, 0, 0,
    -s, 0, c, 0,
    0, 0, 0, 1];
}

function mat_rotX(angle) {
    var c = Math.cos(angle);
    var s = Math.sin(angle);
    return [ 1, 0, 0, 0,
    0, c,-s, 0,
    0, s, c, 0,
    0, 0, 0, 1];
}

function mat_rotZ(angle) {
    var c = Math.cos(angle);
    var s = Math.sin(angle);
    return [ c,-s, 0, 0,
    s, c, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1];
}

function mat_rot(axis, angle) {
    var x = axis[0];
    var y = axis[1];
    var z = axis[2];
    var c = Math.cos(angle);
    var s = Math.sin(angle);
    var ac = 1-c;
    return [x*x*ac+ c  , x*y*ac-z*s, x*z*ac+y*s, 0,
    x*y*ac+z*s , y*y*ac+ c , y*z*ac-x*s, 0,
    x*z*ac-y*s , y*z*ac+x*s, z*z*ac+ c , 0,
    0    ,	 0	   ,	 0	   , 1];
}

function mat_decompose(m) {
    var X = [m[0], m[1], m[2]];
    var Y = [m[4], m[5], m[6]];
    var Z = [m[8], m[9], m[10]];
    var translate = [m[12], m[13], m[14]];
    var scale = [vec3_len(X), vec3_len(Y), vec3_len(Z)];
    return [translate, quat_fromMat(m), scale];
}

function mat_inverse(m)
{
    var inv = mat_identity();

    inv[0] = m[5]  * m[10] * m[15] -
        m[5]  * m[11] * m[14] -
        m[9]  * m[6]  * m[15] +
        m[9]  * m[7]  * m[14] +
        m[13] * m[6]  * m[11] -
        m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
        m[4]  * m[11] * m[14] +
        m[8]  * m[6]  * m[15] -
        m[8]  * m[7]  * m[14] -
        m[12] * m[6]  * m[11] +
        m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
        m[4]  * m[11] * m[13] -
        m[8]  * m[5] * m[15] +
        m[8]  * m[7] * m[13] +
        m[12] * m[5] * m[11] -
        m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
        m[4]  * m[10] * m[13] +
        m[8]  * m[5] * m[14] -
        m[8]  * m[6] * m[13] -
        m[12] * m[5] * m[10] +
        m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
        m[1]  * m[11] * m[14] +
        m[9]  * m[2] * m[15] -
        m[9]  * m[3] * m[14] -
        m[13] * m[2] * m[11] +
        m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
        m[0]  * m[11] * m[14] -
        m[8]  * m[2] * m[15] +
        m[8]  * m[3] * m[14] +
        m[12] * m[2] * m[11] -
        m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
        m[0]  * m[11] * m[13] +
        m[8]  * m[1] * m[15] -
        m[8]  * m[3] * m[13] -
        m[12] * m[1] * m[11] +
        m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
        m[0]  * m[10] * m[13] -
        m[8]  * m[1] * m[14] +
        m[8]  * m[2] * m[13] +
        m[12] * m[1] * m[10] -
        m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
        m[1]  * m[7] * m[14] -
        m[5]  * m[2] * m[15] +
        m[5]  * m[3] * m[14] +
        m[13] * m[2] * m[7] -
        m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
        m[0]  * m[7] * m[14] +
        m[4]  * m[2] * m[15] -
        m[4]  * m[3] * m[14] -
        m[12] * m[2] * m[7] +
        m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
        m[0]  * m[7] * m[13] -
        m[4]  * m[1] * m[15] +
        m[4]  * m[3] * m[13] +
        m[12] * m[1] * m[7] -
        m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
        m[0]  * m[6] * m[13] +
        m[4]  * m[1] * m[14] -
        m[4]  * m[2] * m[13] -
        m[12] * m[1] * m[6] +
        m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
        m[1] * m[7] * m[10] +
        m[5] * m[2] * m[11] -
        m[5] * m[3] * m[10] -
        m[9] * m[2] * m[7] +
        m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
        m[0] * m[7] * m[10] -
        m[4] * m[2] * m[11] +
        m[4] * m[3] * m[10] +
        m[8] * m[2] * m[7] -
        m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
        m[0] * m[7] * m[9] +
        m[4] * m[1] * m[11] -
        m[4] * m[3] * m[9] -
        m[8] * m[1] * m[7] +
        m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
        m[0] * m[6] * m[9] -
        m[4] * m[1] * m[10] +
        m[4] * m[2] * m[9] +
        m[8] * m[1] * m[6] -
        m[8] * m[2] * m[5];

    var det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0) {
        return null;
    }

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        inv[i] = inv[i] * det;

    return inv;
}

function mat_view(pos, dst, up) {
    var Z = vec3_normalized([dst[0]-pos[0], dst[1]-pos[1], dst[2]-pos[2]]);
    var X = vec3_normalized(vec3_cross(up, Z));
    var Y = vec3_cross(Z,X);
    var M = [X[0], Y[0], Z[0], 0,
    X[1], Y[1], Z[1], 0,
    X[2], Y[2], Z[2], 0,
    0 ,	0  ,  0  , 1];
    var P = mat_translate([-pos[0], -pos[1], -pos[2]]);
    return mat_mul(P,M);
}

function mat_perspective(fov_y, aspect, z_near, z_far) {
    var f = Math.tan((Math.PI - fov_y)*0.5);
    //var dz = z_near - z_far;
    var dz = z_far - z_near;
    var dz = z_far - z_near;
    return [f/aspect, 0,  0,                   0,
            0,         f,  0,                   0,
            0,         0, (z_far+z_near)/dz,    1,
            0,         0,-(2*z_far*z_near)/dz,  0];}
