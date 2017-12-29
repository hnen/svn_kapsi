
function vec3_zero() {
	return [0,0,0];
}

function vec3_one() {
	return [1,1,1];
}

function vec3_add(a,b) {
	return [a[0]+b[0], a[1]+b[1], a[2]+b[2]];
}
function vec3_sub(a,b) {
	return [a[0]-b[0], a[1]-b[1], a[2]-b[2]];
}

function vec3_mul(a,m) {
	return [a[0]*m, b[1]*m, c[1]*m];
}

function vec3_dot(a,b) {
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

function vec3_len(a) {
	return Math.sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
}

function vec3_normalized(a) {
	var len = vec3_len(a);
	return [a[0]/len,a[1]/len,a[2]/len];
}

function vec3_cross(a,b) {
	return [a[1]*b[2]-a[2]*b[1],-(a[0]*b[2]-a[2]*b[0]),a[0]*b[1]-a[1]*b[0]];
}

function vec4_dot(a,b) {
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
}

function vec4(v3,w) {
	return [v3[0], v3[1], v3[2], w];
}

function vec4_tf(v,m) {
	return [vec4_dot(v,[m[0+0],m[0+4],m[0+8],m[0+12]]),
			vec4_dot(v,[m[1+0],m[1+4],m[1+8],m[1+12]]),
			vec4_dot(v,[m[2+0],m[2+4],m[2+8],m[2+12]]),
			vec4_dot(v,[m[3+0],m[3+4],m[3+8],m[3+12]])];
}




