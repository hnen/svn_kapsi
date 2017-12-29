
var mat4 = {};

function mat4( values )
{
	if ( values )
		m = values;
	else
		m = [ 
			1,0,0,0, 
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
			];
}

mat4.create = function( values )
{
	if ( values )
		return new Float32Array( values );
	return new Float32Array( 
		[ 
		1,0,0,0, 
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
		] );
}

mat4.identity = function() {
	return new Float32Array( 
		[ 
		1,0,0,0, 
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
		] );
}

mat4.mul = function( m1, m2 ) {
	var vals = [0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0];
	for (var i=0; i<4; i++) {
		for (var j=0; j<4; j++) {
			for (var k=0; k<4; k++) 
				vals[i*4+j] += (m1[i*4+k]* m2[k*4+j]);
		}
	}
	return new Float32Array(vals);
}


mat4.rotation = function( axis, degrees ) {
	var ang = degrees / 180.0 * Math.PI;
	var s = Math.sin(ang);
	var c = Math.cos(ang);
	var m = 1.0 - c;
	return new Float32Array( [
			m*axis.x* axis.x + c,		m*axis.x*axis.y + s*axis.z, m*axis.x*axis.z - s*axis.y, 0,
			m*axis.y*axis.x - s*axis.z,	m*axis.y*axis.y + c,		m*axis.y*axis.z + s*axis.x,	0,
			m*axis.z*axis.x + s*axis.y,	m*axis.z*axis.y - s*axis.x,	m*axis.z*axis.z + c,		0,
			0,	0,	0,	1
			] );
}
mat4.translation = function( tx, ty, tz )
{
	return new Float32Array( [ 
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		tx,ty,tz,1 ] );
}

mat4.lookAt = function( eye, target, up )
{
	var dir = target.sub(eye);
	dir.normalize();
	
	var left = dir.cross(up);
	left = vec3.upvector( left, up );
	
	var u = left.cross(dir);
	u = vec3.upvector( u, dir );
	
	values = [
		left.x, u.x, -dir.x, 0,
		left.y, u.y, -dir.y, 0,
		left.z, u.z, -dir.z, 0,
		left.dot(eye), u.dot(eye), dir.dot(eye), 1 
		];
	return new Float32Array(values);
}

mat4.perspectiveProjection = function ( fov, aspect, near, far ) {
	var m = [0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0];
	var f = 1.0 / Math.tan( fov * 0.5 * Math.PI / 180.0 );
	var r = 1.0 / (far-near);
	m[0] = f / aspect;
	m[4+1] = f;
	m[8+2] = (far+near)/(near-far),
	m[8+3] = -1;
	m[12+2] = 2*(far*near)/(near-far);
	
	return new Float32Array(m);
}

