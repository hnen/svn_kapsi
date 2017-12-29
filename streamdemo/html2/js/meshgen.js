
function meshgen_plane(size_x, size_z, onready) {
	var mesh = {};
	mesh.positions = new Float32Array([
		-size_x,0,-size_z,
		 size_x,0,-size_z,
		 size_x,0, size_z,
		-size_x,0, size_z
	]);
	mesh.uvs = new Float32Array([
		0,0,
		1,0,
		1,1,
		0,1
	]);
	mesh.normals = new Float32Array([
		0,1,0,
		0,1,0,
		0,1,0,
		0,1,0
	]);
	mesh.triangles = new Uint32Array([
		0,1,2,
		2,3,0
	]);
	onready(mesh);
}


function meshgen_circle( radius, segments, onready ) {
	var pos = [];
	var norms = [];
	var uvs = [];
	for ( var i=0; i<segments; i++ )
	{
		var a = Math.PI * 2 * i / segments;
		pos.push( radius * Math.sin( a ) );
		pos.push( 0 );
		pos.push( radius * Math.cos( a ) );
		uvs.push( (1.0+Math.sin(a))/2.0 );
		uvs.push( (1.0+Math.cos(a))/2.0 );
		norms.push( 0 );
		norms.push( 1 );
		norms.push( 0 );
	}
	pos.push( 0 );		pos.push( 0 );		pos.push( 0 );
	uvs.push( 0.5 );	uvs.push( 0.5 );
	norms.push( 0 );	norms.push( 1 );	norms.push( 0 );
	var tris = [];
	for ( var i=0; i<segments; i++ )
	{
		tris.push( segments );
		tris.push( (i)%segments );
		tris.push( (i+1)%segments );
	}
	var mesh = {};
	mesh.positions = new Float32Array( pos );
	mesh.uvs = new Float32Array( uvs );
	mesh.normals = new Float32Array( norms );	
	mesh.triangles = new Uint32Array( tris );
	onready( mesh );
}

function meshgen_circle2( innerradius, outerradius, segments, onready ) {
	var pos = [];
	var norms = [];
	var uvs = [];
	for ( var i=0; i<segments+1; i++ )
	{
		var a = Math.PI * 2 * i / segments;
		var mcs1 = Math.sin(a);
		var mcs2 = Math.cos(a);
		pos.push( outerradius * mcs1 );
		pos.push( 0 );
		pos.push( outerradius * mcs2 );
		pos.push( innerradius * mcs1 );
		pos.push( 0 );
		pos.push( innerradius * mcs2 );
		uvs.push( 1 );
		uvs.push( i/segments );
		uvs.push( 0 );
		uvs.push( i/segments );
		norms.push( 0 );	norms.push( 1 );	norms.push( 0 );
		norms.push( 0 );	norms.push( 1 );	norms.push( 0 );
	}
	var tris = [];
	for ( var i=0; i<segments; i++ )
	{
		tris.push( i*2 );
		tris.push( i*2+2 );
		tris.push( i*2+1 );

		tris.push( i*2+2 );
		tris.push( i*2+3 );
		tris.push( i*2+1 );
	}
	var mesh = {};
	mesh.positions = new Float32Array( pos );
	mesh.uvs = new Float32Array( uvs );
	mesh.normals = new Float32Array( norms );	
	mesh.triangles = new Uint32Array( tris );
	onready( mesh );
}

function meshgen_star( innerradius, outerradius, segments, connection, onready ) {
	var pos = [];
	var norms = [];
	var uvs = [];
	for ( var i=0; i<segments+1; i++ )
	{
		var a = Math.PI * 2 * i / segments;
		var mcs1 = Math.sin(a);
		var mcs2 = Math.cos(a);
		pos.push( outerradius * mcs1 );
		pos.push( 0 );
		pos.push( outerradius * mcs2 );
		pos.push( innerradius * mcs1 );
		pos.push( 0 );
		pos.push( innerradius * mcs2 );
		uvs.push( 1 );
		uvs.push( i/segments );
		uvs.push( 0 );
		uvs.push( i/segments );
		norms.push( 0 );	norms.push( 1 );	norms.push( 0 );
		norms.push( 0 );	norms.push( 1 );	norms.push( 0 );
	}
	var tris = [];
	for ( var i=0; i<segments; i++ )
	{
		var i2 = (i+connection)%(segments)
		tris.push( i*2 );
		tris.push( i2*2 );
		tris.push( i*2+1 );

		tris.push( i2*2 );
		tris.push( i2*2+1 );
		tris.push( i*2+1 );
	}
	var mesh = {};
	mesh.positions = new Float32Array( pos );
	mesh.uvs = new Float32Array( uvs );
	mesh.normals = new Float32Array( norms );	
	mesh.triangles = new Uint32Array( tris );
	onready( mesh );
}


