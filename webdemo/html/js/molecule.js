

var molecule_shader;

var mol_quad = [];

function mol_createVertical( x, y, z ) {
	var w = 0.05;
	var h = 1.0;
	return [
		 x+w, y+h, z,
		 x-w, y+h, z,
		 x+w, y, z,
		 
		 x-w, y+h, z,
		 x+w, y, z,
		 x-w, y, z
	] ;
}
function mol_createHorizontal( x, y, z, dir ) {
	var w = 0.05;
	var h = 1.0;
	return  [
		 x+dir+w, y+h, z,
		 x+dir-w, y+h, z,
		 x+w, y, z,
		 
		 x+dir-w, y+h, z,
		 x+w, y, z,
		 x-w, y, z

	] ;
}

function kakkis( obu, x, y, z, n )
{
	if ( Math.random() < 0.7 || n < 2 )
	{
		obu = obu.concat( mol_createHorizontal( x, y, z, -1 ) );
		if ( Math.random() < 0.7 )
		{
			obu = obu.concat( mol_createVertical( x-1,y+1, z ) );
			if ( Math.random() < 0.7 )
				obu = kakkis( obu, x-1, y+2, z, n++ );
		}
	}
	if ( Math.random() < 0.7 || n < 2 )
	{
		obu = obu.concat( mol_createHorizontal( x, y, z, +1 ) );
		if ( Math.random() < 0.7 )
		{
			obu = obu.concat( mol_createVertical( x+1,y+1, z ) );
			if ( Math.random() < 0.7 )
				obu = kakkis( obu, x+1, y+2, z, n++ );
		}
	}
	return obu;
}

function kakkisT( obu, x, y, z, n, type )
{
	if ( n>=type.length )
		return obu;
	var c = parseInt(type[n]);
	if ( c > 0 )
	{
		if ( c < 7 )
		{
			obu = obu.concat( mol_createHorizontal( x, y, z, -1 ) );
			if ( c > 3 || c<2 )
			{
				obu = obu.concat( mol_createVertical( x-1,y+1, z ) );
				obu = kakkis( obu, x-1, y+2, z, n+1 );
			}
		}
		if ( c > 2 )
		{
			obu = obu.concat( mol_createHorizontal( x, y, z, +1 ) );
			if ( c < 5 || c>7 )
			{
				obu = obu.concat( mol_createVertical( x+1,y+1, z ) );
				obu = kakkis( obu, x+1, y+2, z, n+1 );
			}
		}
	}
	return obu;
}



function molecule_create(type)
{
	var fii = mol_createVertical(0,0,0);
	if ( type )
	{
		fii = kakkisT( fii, 0,1,0, 0, type );
	}
	else
		fii = kakkis( fii, 0,1,0, 0 );

	if ( type )
		mol_quad[type] = gl_createVB( fii );
	else 
		mol_quad["basic"] = gl_createVB( fii );
		
}


function molecule_init() { 
	
	molecule_create();
	
	gl_createShader( "data/flat.vs", "data/flat.fs", function(program) {
		molecule_shader = program;
		molecule_shader.vertexPositionAttribute = GL.getAttribLocation(molecule_shader, "in_Pos");
		molecule_shader.viewMatrix = GL.getUniformLocation(molecule_shader, "view");
		molecule_shader.worldMatrix = GL.getUniformLocation(molecule_shader, "world");
		molecule_shader.projectionMatrix = GL.getUniformLocation(molecule_shader, "projection");
	} );
}

function molecule_render( count, ydir, ypos, rot, zoom, type ) { 

	var obj;
	if ( type )
	{
		var type_ = (Math.round(type) / 10000).toString();
		if ( !mol_quad.hasOwnProperty(type_) )
			molecule_create(type_);
		obj = mol_quad[type_];
	}
	else
		obj = mol_quad["basic"];
		
	count = Math.round(count);

	GL.useProgram( molecule_shader );
	//GL.uniform1i( GL.getUniformLocation( molecule_shader, "tex0" ), 0 );
	//GL.uniform1f( GL.getUniformLocation( molecule_shader, "ip" ), alpha );
	
	GL.bindBuffer( GL.ARRAY_BUFFER, obj );
	GL.enableVertexAttribArray(molecule_shader.vertexPositionAttribute);
	GL.vertexAttribPointer( molecule_shader.vertexPositionAttribute, obj.itemSize, GL.FLOAT, false, 0, 0 );
	
	GL.disable( GL.BLEND );
	GL.disable( GL.DEPTH_TEST );
	GL.disable( GL.CULL_FACE );
	GL.cullFace( GL.FRONT_AND_BACK );
	
	GL.depthRange( 0.1, 1000 );

	var df = p_musa_audio_el.currentTime * rot;
	df += Math.PI / 2.0;
	var lookAt = mat4.lookAt( 
			new vec3( zoom*Math.sin(df), ydir-ypos, zoom*Math.cos(df) ),
			new vec3( 0,-ydir-ypos,0 ),
			new vec3( 0,1,0 ) );
	
	GL.uniformMatrix4fv( molecule_shader.viewMatrix, false, lookAt );
	
	GL.uniformMatrix4fv( molecule_shader.projectionMatrix, false,
		mat4.perspectiveProjection( 90, 1.33, 0.1, 1000 )
		);

	
	for ( var i=0; i<count; i++ )
	{
		var tr = mat4.rotation( new vec3(0,1,0), (180/count)*i + 90 );
		GL.uniformMatrix4fv( molecule_shader.worldMatrix, false, tr );
		GL.drawArrays( GL.TRIANGLES, 0, obj.numItems );
	}

	GL.disableVertexAttribArray(molecule_shader.vertexPositionAttribute);
	GL.bindBuffer( GL.ARRAY_BUFFER, null );

}


