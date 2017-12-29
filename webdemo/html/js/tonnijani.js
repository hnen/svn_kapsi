
var tonnijani_mesh = null;
var tonnijani_shader = null;

function tonnijani_init()
{
	tonnijani_mesh = gen.fromData( meshFlyVertex, meshFlyFace, meshFlyNormal );
	
		tonnijani_mesh.matrix = mat4.identity();
		tonnijani_mesh.child = [];
		tonnijani_mesh.child.push( gen.fromData( meshLegVertex, meshLegFace, meshLegNormal ) );
		tonnijani_mesh.child.push( gen.fromData( meshLegVertex, meshLegFace, meshLegNormal ) );
		tonnijani_mesh.child.push( gen.fromData( meshLegVertex, meshLegFace, meshLegNormal ) );
		
		tonnijani_mesh.child[0].matrix = mat4.rotation( new vec3(0,1,0), 0);
//		tonnijani_mesh.child[1].matrix = mat4.rotation( new vec3(0,1,0), 20.0 );
//		tonnijani_mesh.child[2].matrix = mat4.rotation( new vec3(0,1,0), 100.0 );
		tonnijani_mesh.child[1].matrix = mat4.rotation( new vec3(0,1,0), -20.0 );
		tonnijani_mesh.child[2].matrix = mat4.rotation( new vec3(0,1,0), -60.0 );
	
		tonnijani_mesh.child.push( gen.fromData( meshLegVertex, meshLegFace, meshLegNormal ) );
		tonnijani_mesh.child.push( gen.fromData( meshLegVertex, meshLegFace, meshLegNormal ) );
		tonnijani_mesh.child.push( gen.fromData( meshLegVertex, meshLegFace, meshLegNormal ) );
	
//		tonnijani_mesh.child[3].matrix = mat4.rotation( new vec3(0,1,0), 180.0 );
//		tonnijani_mesh.child[4].matrix = mat4.rotation( new vec3(0,1,0), 180.0-20.0 );
//		tonnijani_mesh.child[5].matrix = mat4.rotation( new vec3(0,1,0), 180.0-100.0);
		tonnijani_mesh.child[3].matrix = mat4.rotation( new vec3(0,1,0), 180.0-65 );
		tonnijani_mesh.child[4].matrix = mat4.rotation( new vec3(0,1,0), 180.0-45 );
		tonnijani_mesh.child[5].matrix = mat4.rotation( new vec3(0,1,0), 180.0-5 );
/*	
		tonnijani_mesh.child.push( gen.fromData( meshWingVertex, meshWingFace, meshWingNormal ) );
		tonnijani_mesh.child.push( gen.fromData( meshWingVertex, meshWingFace, meshWingNormal ) );

		tonnijani_mesh.child[6].matrix = mat4.rotation( new vec3(0,1,0), 0);
		tonnijani_mesh.child[7].matrix = mat4.rotation( new vec3(0,1,0), 0.0 );

		tonnijani_mesh.child.push( gen.fromData( meshWingVertex, meshWingFace, meshWingNormal ) );
		tonnijani_mesh.child.push( gen.fromData( meshWingVertex, meshWingFace, meshWingNormal ) );
		tonnijani_mesh.child[8].matrix = mat4.rotation( new vec3(0,1,0), 0 );
		tonnijani_mesh.child[9].matrix = mat4.rotation( new vec3(0,1,0), 0 );
	*/
	
	for ( var i=0; i<tonnijani_mesh.child.length; i++ ) 
		tonnijani_mesh.child[i].matrix_orig = tonnijani_mesh.child[i].matrix;
//	tonnijani_mesh = gen.tubeIndexed( 7, 1, 0.5, 0, 1, 10 );
	gl_createShader( "data/light.vs", "data/light.fs", function(program) {
		tonnijani_shader = program;
		tonnijani_shader.vertexPositionAttribute = GL.getAttribLocation(tonnijani_shader, "in_Pos");
		tonnijani_shader.normalPositionAttribute = GL.getAttribLocation(tonnijani_shader, "in_Normal");
		tonnijani_shader.viewMatrix = GL.getUniformLocation(tonnijani_shader, "view");
		tonnijani_shader.worldMatrix = GL.getUniformLocation(tonnijani_shader, "world");
		tonnijani_shader.projectionMatrix = GL.getUniformLocation(tonnijani_shader, "projection");
	} );
}


function tonnijani_render( xpos, ypos, size, speed )
{

	if ( tonnijani_shader == null )
		console.log(" EIEIEIEIEIEIEI" );

//	tonnijani_mesh.vertex

	var df = p_musa_audio_el.currentTime;
	var view = mat4.lookAt( 
//			new vec3( zoom*Math.sin(df), zoom*Math.cos(df), 0 ),
			new vec3( 0, size, 0 ),
			new vec3( 0,0.0001,0 ),
			new vec3( 0,0,1 ) );
	
		var tr = mat4.rotation( new vec3(0,1,0), 0 );
		
		tr = mat4.translation( xpos,0, ypos );

//	tonnijani_mesh.draw( mat4.perspectiveProjection( 90, 1.33, 0.1, 1000 ), view, tr );

	var perspective = mat4.perspectiveProjection( 90, 1.33, 0.1, 1000 );
	{
	
		tonnijani_mesh.matrix = mat4.translation( 0, 0, 0 );
		for ( var a=0; a<6; a++ ) 
		{
			var yyyy = Math.sin( df * speed*(a>2?1:-1) + (a-2.5)*(a-2.5)*0.4 );
			yyyy = yyyy * yyyy * (yyyy>0?1:-1);
			var zorro = mat4.rotation( new vec3( 0,1,0), yyyy * 10 );
			tonnijani_mesh.child[a].matrix = mat4.mul( zorro, tonnijani_mesh.child[a].matrix_orig );
		}

/*		tonnijani_mesh.child[6].matrix = mat4.rotation( new vec3(0,1,0), 0);
		tonnijani_mesh.child[7].matrix = mat4.rotation( new vec3(0,1,0), -20 );
		tonnijani_mesh.child[8].matrix = mat4.rotation( new vec3(0,1,0), 40 );
		tonnijani_mesh.child[9].matrix = mat4.rotation( new vec3(0,1,0), 40+20 );

		
		for ( var a=6; a<=9; a++ ) 
		{
			var yyyy = Math.sin( df * 10  ) * 0.3 * (a-7.5);
			var zorro = mat4.rotation( new vec3( 0,1,0), yyyy * 10 );
			tonnijani_mesh.child[a].matrix = mat4.mul( zorro, tonnijani_mesh.child[a].matrix );
		}
*/
				tonnijani_mesh.draw( perspective, view, tr );
/*		for ( var a = 0; a<5; a++ ){
			for ( var b=0; b<5; b++ ) {
//				mat4.scale( tonnijani_mesh.matrix, [10.0,10.0,10.0]  );
				tonnijani_mesh.matrix = mat4.translation( (a-2.5)*0.1,(b-2.5)*0.1,0 );
				tonnijani_mesh.draw( perspective, view, mat4.identity() );
			}
		}*/
		tonnijani_mesh.matrix = mat4.identity();
	}

}


