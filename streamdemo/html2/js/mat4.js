
function mat_identity() {
	return [1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1];
}

function mat_scale(v) {
	return [v[0],	0,	 0,   0,
			   0,v[1],	 0,   0,
			   0,	0,v[2],   0,
			   0,	0,	 0,   1];
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
	var dz = z_near - z_far;
	return [f/aspect,0,0,0,
			0,f, 0,0,
			0,0,(z_far+z_near)/dz,-2*(z_far*z_near)/dz,
			0,0,-1,0];
}


