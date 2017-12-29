

var gen = {}

function Mesh() {

	this.shader = null;
	this.vertexPositionAttribute = null;
	this.timeAttribute = null;
	
	this.polys = 0; // actually indexCount if indexbuffer!=null
	this.vertexNum = 0;	//unused. set anyway when used indexbuffer. not tested
	this.vertexSize = 2;
	this.vertexBuffer = null;
	
	this.indexBuffer = null;
	this.normalBuffer = null;
	
	this.textureBuffer = null;
	this.texture = null;
	this.textureCoordAttribute = null;

	//unused
	this.matrix = null;
	this.child = [];

	this.draw = drawMesh;
}

function drawMesh( perspective, view, world__ ) {
//Mesh.draw = function( gl ) {
this.shader = tonnijani_shader;
if ( this.shader == null )
{
	this.shader = molecule_shader;	
	console.log("-------- no shader");
	}
	if ( this.shader != null )
	{
		GL.useProgram( this.shader );
//		this.shader.start();
	}
	else
		console.log("shader == null");
		
	if ( this.vertexBuffer == null )
	{
		console.log("vertexBuffer == null" );
		return;
	}

	if ( this.indexBuffer != null )
		GL.bindBuffer( GL.ELEMENT_ARRAY_BUFFER, this.indexBuffer );
		
//	GL.enableVertexAttribArray( this.vertexPositionAttribute );
	GL.enableVertexAttribArray( this.shader.vertexPositionAttribute );
	GL.bindBuffer(GL.ARRAY_BUFFER, this.vertexBuffer);
//	GL.vertexAttribPointer(this.vertexPositionAttribute, this.vertexSize, GL.FLOAT, false, 0, 0);
	GL.vertexAttribPointer(this.shader.vertexPositionAttribute, this.vertexSize, GL.FLOAT, false, 0, 0);
		
	this.textureBuffer = null;
	if ( this.textureBuffer != null )
	{
		var tex_ = null;
		if ( this.texture != null ) 
			tex_ = this.texture;
		else 
			tex_ = this.shader.texture;
			
		if ( tex_ != null )
		{
		/*
			if ( !GL.isTexture(tex_) ) {
				console.log("texture is not texture, but " + tex_ );
			}*/
			GL.activeTexture(GL.TEXTURE0);
			GL.bindTexture(GL.TEXTURE_2D, tex_ );
			var texAttr = GL.getUniformLocation( this.shader, "texture" );
			GL.uniform1i( texAttr, 0 );
			checkGL("bindTexture");
//			GL.enableVertexAttribArray( this.textureCoordAttribute );
			GL.enableVertexAttribArray( 1 );
			GL.bindBuffer(GL.ARRAY_BUFFER, this.textureBuffer);
//			GL.vertexAttribPointer( this.textureCoordAttribute, 2, GL.FLOAT, false, 0, 0 );
			GL.vertexAttribPointer( 1, 2, GL.FLOAT, false, 0, 0 );
			checkGL("texCoords");
		}
		else
		{
//			if ( !GL.isTexture(this.shader.texture) )
				console.log("texture failed @ generator . polys: " + this.polys  );
		}
		checkGL( "texture" );
	}
	
	
		
	if ( this.normalBuffer != null )
	{
		GL.enableVertexAttribArray( this.shader.normalPositionAttribute );
		GL.bindBuffer(GL.ARRAY_BUFFER, this.normalBuffer);
		GL.vertexAttribPointer(this.shader.normalPositionAttribute, 3, GL.FLOAT, false, 0, 0);
	}

	
	var world = world__;
	if ( this.matrix != null && world__ != null ) 
		world = mat4.mul( this.matrix, world__	);
	else if ( this.matrix != null )
		world = this.matrix;
	else
	{
		if ( !world__ )
			console.log( "world__ = null ");
		if ( !this.matrix )
			console.log( "this.matrix = null ");
	}

		/*
//	setMatrix( this.shader, Math.random() );
	var pUniform = GL.getUniformLocation(this.shader.program, "Projection");
	GL.uniformMatrix4fv(pUniform, false, new Float32Array(perspective) );
	var mvUniform = GL.getUniformLocation(this.shader.program, "ModelView");
	GL.uniformMatrix4fv(mvUniform, false, new Float32Array(curMat) );
	*/
	var pUniform = GL.getUniformLocation(this.shader, "projection");
	GL.uniformMatrix4fv(pUniform, false, new Float32Array(perspective) );
	
	var vUniform = GL.getUniformLocation(this.shader, "view");
	GL.uniformMatrix4fv(vUniform, false, view );
	
	var mUniform = GL.getUniformLocation(this.shader, "world");
	GL.uniformMatrix4fv(mUniform, false, world );
	
	
	
	GL.enable( GL.BLEND );	
	if ( this.indexBuffer != null )
		GL.drawElements( GL.TRIANGLES, this.polys, GL.UNSIGNED_SHORT, 0 );
	else
		GL.drawArrays(GL.TRIANGLES, 0, this.polys );

	if ( this.child != null ) 
		for ( var a=0; a<this.child.length; a++ )
			{
				this.child[a].draw( perspective, view, world );
			}
}



// filled circle
gen.circleArray = function( seg, rad )
{
	var vertices = []; 
	if ( rad == null ) 
		rad = 0.4;
	
	var x2, y2;
	for ( var a=0; a<seg; a++ )
	{
		var q = Math.PI * 2 * a / (seg-1);
		var x1 = rad * Math.sin( q );
		var y1 = rad * Math.cos( q )
		if ( a > 0 ) 
		{
			vertices.push( 0 );		vertices.push( 0 );		
			vertices.push( x1 );	vertices.push( y1 );	
			vertices.push( x2 );	vertices.push( y2 );	
		}
		x2 = x1;	y2 = y1;
	}
	return vertices;
}

// circle with inner hole
gen.circleArray2 = function( seg, rad1, rad2, z )
{
	var vertices = []; //new Float32Array([ -1., -1.,   1., -1.,    -1.,  1.,     1., -1.,    1.,  1.,    -1.,  1.]);
	if ( z == null ) 
		z = 0;
	
	var x2, y2;
	for ( var a=0; a<seg; a++ )
	{
		var q = Math.PI * 2 * a / (seg-1);
		var x1 = Math.sin( q );
		var y1 = Math.cos( q )
		if ( a > 0 ) 
		{
			vertices.push( x1*rad1 );	vertices.push( y1*rad1 );	vertices.push( z );
			vertices.push( x2*rad1 );	vertices.push( y2*rad1 );	vertices.push( z );
			vertices.push( x1*rad2 );	vertices.push( y1*rad2 );	vertices.push( z );
			vertices.push( x2*rad1 );	vertices.push( y2*rad1 );	vertices.push( z );
			vertices.push( x2*rad2 );	vertices.push( y2*rad2 );	vertices.push( z );
			vertices.push( x1*rad2 );	vertices.push( y1*rad2 );	vertices.push( z );
		}
		x2 = x1;	y2 = y1;
	}
	return vertices;
}

// indexed circle with inner hole. first rad1, then rad2
gen.circleArrayIndexed = function( seg, rad1, rad2, z )
{
	var vertices = []; 
	if ( z == null ) 
		z = 0;
	for ( var a=0; a<seg; a++ ) {
		var q = Math.PI * 2 * a / (seg-1);
		var x1 = Math.sin( q );
		var y1 = Math.cos( q )
		vertices.push( x1*rad1 );	vertices.push( y1*rad1 );	vertices.push( z );
	}
	for ( var a=0; a<seg; a++ ) {
		var q = Math.PI * 2 * a / (seg-1);
		var x1 = Math.sin( q );
		var y1 = Math.cos( q )
		vertices.push( x1*rad2 );	vertices.push( y1*rad2 );	vertices.push( z );
	}
	return vertices;
}

gen.fromData = function( vert, face, normal )
{
	var mesh = new Mesh();
	mesh.polys = face.length;
	mesh.vertexNum = vert.length;
	
	var ind = new Uint16Array( face );
	var iBuffer = GL.createBuffer();
	GL.bindBuffer(GL.ELEMENT_ARRAY_BUFFER, iBuffer);
	GL.bufferData( GL.ELEMENT_ARRAY_BUFFER, ind, GL.STATIC_DRAW );
	mesh.indexBuffer = iBuffer;
	
	var vertex = new Float32Array( vert );
	for ( var ii=0; ii<vertex.length; ii++ )
		vertex[ii] *= 10;
	var vBuffer = GL.createBuffer();
	GL.bindBuffer(GL.ARRAY_BUFFER, vBuffer);
	GL.bufferData( GL.ARRAY_BUFFER, vertex, GL.STATIC_DRAW );
	mesh.vertexBuffer = vBuffer;
	mesh.vertexSize = 3;
	
	if ( normal != null )
	{
		var norm = new Float32Array( normal );
		var nBuffer = GL.createBuffer();
		GL.bindBuffer(GL.ARRAY_BUFFER, nBuffer);
		GL.bufferData( GL.ARRAY_BUFFER, norm, GL.STATIC_DRAW );
		mesh.normalBuffer = nBuffer;
	}
	
	return mesh;
}

gen.tubeIndexed = function( seg, rad1, rad2, z1, z2, sections )
{
	var mesh = new Mesh();
	
	mesh.polys = 0;
	mesh.vertexNum = 0;
	
	var vertices = [];
	
	if ( sections==null  ) sections = 1;
	for ( var a=0; a<sections+1; a++ )
	{
		var i = a/(sections);
		var v2 = gen.circleArrayIndexed( seg, rad1, rad2, z2*(1.0-i) + z1*i );
		vertices = vertices.concat( v2 );
		mesh.vertexNum += (seg-1)*6;
	}
	
	var indices = [];
	
	// end
	var q=0;
	for ( var a=0; a<seg-1; a++ )
	{
		indices.push( a );
		indices.push( (a+1)%seg );
		indices.push( a+seg );
		indices.push( (a+1)%seg );
		indices.push( (a+1)%seg+seg );
		indices.push( a+seg );
	}
	mesh.polys = (seg-1)*6;

	// another end
	for ( var a=0; a<(seg-1); a++ )
	{
		var s2 = sections*seg*2;
		var a2 = (a+1)%seg;
		indices.push( s2 + a );
		indices.push( s2 + a2 );
		indices.push( s2 + a+seg );
		indices.push( s2 + a2 );
		indices.push( s2 + a2+seg );
		indices.push( s2 + a+seg );
	}
	mesh.polys += (seg-1)*6;

	//sides
	for ( var b=0; b<sections; b++ ) 
	{
		for ( var a=0; a<seg-1; a++ )
		{
			var s1 = b*seg*2;
			var s2 = (b+1)*seg*2;
			
			var a2 = (a+1)%seg;
			// inner. TODO: might be better to turn around. but which one?
			indices.push( seg + s1+ a );
			indices.push( seg + s2 + a );
			indices.push( seg + s1+ a2 );
			indices.push( seg + s2 + a );
			indices.push( seg + s2 + a2 );
			indices.push( seg + s1+ a2 );
			// outer
			indices.push( s1+ a );
			indices.push( s2 + a );
			indices.push( s1+ a2 );
			indices.push( s2 + a );
			indices.push( s2 + a2 );
			indices.push( s1+ a2 );
			
			mesh.polys += 4*3;
		}
	}
	var ind = new Uint16Array( indices );
	var iBuffer = GL.createBuffer();
	GL.bindBuffer(GL.ELEMENT_ARRAY_BUFFER, iBuffer);
	GL.bufferData( GL.ELEMENT_ARRAY_BUFFER, ind, GL.STATIC_DRAW );
	mesh.indexBuffer = iBuffer;
	
	var vert = new Float32Array( vertices );
	var vBuffer = GL.createBuffer();
	GL.bindBuffer(GL.ARRAY_BUFFER, vBuffer);
	GL.bufferData( GL.ARRAY_BUFFER, vert, GL.STATIC_DRAW );
	mesh.vertexBuffer = vBuffer;
	mesh.vertexSize = 3;
console.log(" tubeIndexed. polys: " + mesh.polys  );
	return mesh;
}

gen.grid = function( x0,y0, x1,y1, xseg, yseg )
{
	var tex = [];
	
	var vertices = [];
	for ( var a=0; a<yseg; a++ )
	{
		var i1 = a/(yseg-1.0);
		for ( var b=0; b<xseg; b++ ) 
		{
			var i2 = b/(xseg-1.0);
			vertices.push( x0*i2 + x1*(1.0-i2) ); 
			vertices.push( y0*i1 + y1*(1.0-i1) ); 
			vertices.push( 0 ); 
			
			
			if ( b%2 == 0 ) 
				tex.push( 0 );
			else
				tex.push( 1 );
			if ( a&1 )
				tex.push( 0 );
			else
				tex.push( 1 );
		}
	}
	var mesh = new Mesh();
	var vert = new Float32Array( vertices );
	var vBuffer = GL.createBuffer();
	GL.bindBuffer(GL.ARRAY_BUFFER, vBuffer);
	GL.bufferData( GL.ARRAY_BUFFER, vert, GL.STATIC_DRAW );
	mesh.vertexBuffer = vBuffer;
	mesh.vertexSize = 3;
	
	var indices = [];
	for ( var a=0; a<yseg-1; a++ )
		for ( var b=0; b<xseg-1; b++ )
		{
			indices.push( a*xseg+b );
			indices.push( a*xseg+b+1 );
			indices.push( (a+1)*xseg+b );
			indices.push( a*xseg+b+1 );
			indices.push( (a+1)*xseg+b+1 );
			indices.push( (a+1)*xseg+b );
/*			tex.push(0.0);	tex.push(0.0);
			tex.push(1.0);	tex.push(0.0);
			tex.push(0.0);	tex.push(1.0);
			tex.push(1.0);	tex.push(0.0);
			tex.push(1.0);	tex.push(1.0);
			tex.push(0.0);	tex.push(1.0);*/
/*			tex.push( Math.random() );
			tex.push( Math.random() );
			tex.push( Math.random() );
			tex.push( Math.random() );
			tex.push( Math.random() );
			tex.push( Math.random() );*/
			mesh.polys += 2*3;
		}
		
	var ind = new Uint16Array( indices );
	var iBuffer = GL.createBuffer();
	GL.bindBuffer(GL.ELEMENT_ARRAY_BUFFER, iBuffer);
	GL.bufferData( GL.ELEMENT_ARRAY_BUFFER, ind, GL.STATIC_DRAW );
	mesh.indexBuffer = iBuffer;
	
	// texcord
	var textureCoords = new Float32Array( tex );
	var tBuffer = GL.createBuffer();
	GL.bindBuffer( GL.ARRAY_BUFFER, tBuffer );
	GL.bufferData( GL.ARRAY_BUFFER, textureCoords, GL.STATIC_DRAW );
	mesh.textureBuffer = tBuffer;
	
console.log(" gen.grid: polys: " + mesh.polys  );
	return mesh;
}

gen.tube = function( seg, rad1, rad2, z1, z2 )
{
	var vertices = gen.circleArray2( seg, rad1, rad2, z1 );
	var mesh = new Mesh();
	mesh.polys = (seg-1) * 6;
	
	var len = vertices.length;
	var len3 = len/3;
	var q=0;
	for ( var a=0; a<len3; a++ ) 
	{
		vertices.push( vertices[q++] );
		vertices.push( vertices[q++] );
		vertices.push( z2 );
		q++;
	}
	mesh.polys *= 2;
	
/*	q = 0;
	var w = len;
	for ( var a=0; a<len3; a++ ) 
	{
		vertices.push( vertices[q+0] );
		vertices.push( vertices[q+1] );
		vertices.push( vertices[q+2] );
	}*/
	var vert = new Float32Array( vertices );
	var vBuffer = GL.createBuffer();
	GL.bindBuffer(GL.ARRAY_BUFFER, vBuffer);
	GL.bufferData( GL.ARRAY_BUFFER, vert, GL.STATIC_DRAW );
	mesh.vertexBuffer = vBuffer;
	mesh.vertexSize = 3;
console.log(" gen.tube:  polys: " + mesh.polys  );
	return mesh;
//	return vBuffer;
}


gen.circle = function( seg, rad )
{
	var vertices = gen.circleArray( seg, rad );
	var vert = Float32Array( vertices );
	var vBuffer = GL.createBuffer();
	GL.bindBuffer(GL.ARRAY_BUFFER, vBuffer);
	GL.bufferData( GL.ARRAY_BUFFER, vert, GL.STATIC_DRAW );
	return vBuffer;
}

gen.star = function( seg, rad )
{
	var vertices = gen.circleArray( seg, rad );
	if ( rad == null ) rad = 0.4;
	for ( var a=0; a<seg; a++ ) 
	{
		var i1 = a*6;
		var q = Math.PI * 2 * a / (seg-1);
		q += (Math.PI*2 / (seg-1)) / 2.0;
		var x1 = rad * 2 * Math.sin( q );
		var y1 = rad * 2 * Math.cos( q )
		vertices.push( x1 );
		vertices.push( y1 );
		vertices.push( vertices[i1+2] );
		vertices.push( vertices[i1+3] );
		vertices.push( vertices[i1+4] );
		vertices.push( vertices[i1+5] );
//		vertices.push( 0 );
//		vertices.push( 0 );
	}
	var vert = Float32Array( vertices );
	var vBuffer = GL.createBuffer();
	GL.bindBuffer(GL.ARRAY_BUFFER, vBuffer);
	GL.bufferData( GL.ARRAY_BUFFER, vert, GL.STATIC_DRAW );
	return vBuffer;
}

