
var efe0_shader;

function efe0_init() {
	
}

function push(a,v) {
	a.push(v.x);
	a.push(v.y);
	a.push(v.z);
}

function efe0_render() {

	var verts = [];
	for( var i = 0; i < 100; i++ ) {
		var v = new vec3( noise(i), noise(i*313), noise(i*23131) );
		v = v.add( new vec3(-0.5,-0.5,-0.5 ) );
		v.normalize();
		v *= noise(i+2311) * 20;
		verts.push( new vec3(-1.0,0.0,0.0) );
		verts.push( new vec3( 1.0,0.0,0.0) );
		verts.push( v );
	}

	var vb = gl_createVB(verts);

	GL.disable(GL.BLEND);	
	
	GL.useProgram( efe0_shader );
	GL.bindBuffer( GL.ARRAY_BUFFER, vb );
	GL.enableVertexAttribArray(efe0_shader.vertexPositionAttribute);
	GL.vertexAttribPointer( efe0_shader.vertexPositionAttribute, efe0_shader.itemSize, GL.FLOAT, false, 0, 0 );

	var wvp = mat4.perspectiveProjection( 40.0, 16/9, 0.1, 100.0 );
	GL.uniform1f( GL.getUniformLocation( efe0_shader, "wvp" ), mul );
	

	GL.drawArrays( GL.TRIANGLES, 0, vb.numItems ); 
	GL.disableVertexAttribArray(efe0_shader.vertexPositionAttribute);
	GL.bindBuffer( GL.ARRAY_BUFFER, null );

	gl_destroyVB( vb );

}

