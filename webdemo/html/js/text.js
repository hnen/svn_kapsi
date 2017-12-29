
texts = {
	f20: { filename: "data/txt_f20.png" },
	f: { filename: "data/f_logo_trans.png" },
	die: { filename: [ "data/txt_00.png", "data/txt_01.png", "data/txt_02.png",
		"data/txt_03.png", "data/txt_04.png", "data/txt_05.png", "data/txt_06.png", "data/txt_07.png", "data/txt_08.png", "data/txt_09.png", "data/txt_10.png" ] },
	g: { filename: [ "data/g_00.png", "data/g_01.png", "data/g_02.png",

		"data/g_03.png", "data/g_04.png", "data/g_05.png", "data/g_06.png", "data/g_07.png", "data/g_08.png" ] },
	credits: { filename: [ "data/credits0.png", "data/credits1.png", "data/credits2.png", "data/credits3.png" ] }
		
};

var text_readycount = 0;
var text_origcount = 0;

var text_shader = null;

function text_init() {
	for( var name in texts ) {
		var text = texts[name];
		p_text_createText( text );
	}

	gl_createShader( "data/text.vs", "data/text.fs", function(shader) {
		text_shader = shader;
		text_shader.v_pos = GL.getAttribLocation( text_shader, "in_Pos" );
	} );
}

function p_text_createText( text ) {
	if ( Array.isArray( text.filename ) ) {
		text.texture = [];
		for( var i in text.filename ) {
			var filename = text.filename[i];
			text_origcount++;
			gl_createTextureFromImage( filename, function(texture) {
				text.texture.push( texture );
				text_readycount++;
			} );
			
		}
	} else {
		text_origcount++;
		var texture = gl_createTextureFromImage( text.filename, function(texture) {
			text.texture = [texture];
			text_readycount++;
		} );
	}
}

function text_render( textName, x, y, size, alpha, n ) {
	var text = texts[textName];

	var texture = text.texture[ Math.floor(mod(n,text.texture.length)) ];
	GL.useProgram( text_shader );
	GL.activeTexture( GL.TEXTURE0 );
	GL.bindTexture( GL.TEXTURE_2D, texture.texture );
	GL.uniform1i( GL.getUniformLocation( text_shader, "tex" ), 0 );
	GL.uniform2f( GL.getUniformLocation( text_shader, "tex_size" ), texture.width / texture.texture_width, texture.height / texture.texture_height );
	GL.uniform1f( GL.getUniformLocation( text_shader, "scr_aspect" ), 16.0/9.0 );
	GL.uniform1f( GL.getUniformLocation( text_shader, "img_aspect" ), texture.width / texture.height );
	GL.uniform1f( GL.getUniformLocation( text_shader, "draw_size" ), size );
	GL.uniform2f( GL.getUniformLocation( text_shader, "draw_pos" ), x, y );
	GL.uniform1f( GL.getUniformLocation( text_shader, "alpha" ), alpha );
	GL.enableVertexAttribArray(text_shader.v_pos);
	GL.bindBuffer( GL.ARRAY_BUFFER, vb_quad );
	GL.vertexAttribPointer( text_shader.v_pos, vb_quad.itemSize, GL.FLOAT, false, 0, 0 );
	GL.enable( GL.BLEND );
	GL.blendFunc( GL.SRC_ALPHA, GL.ONE );
	GL.disable( GL.DEPTH_TEST );
	GL.drawArrays( GL.TRIANGLE_STRIP, 0, vb_quad.numItems );

	GL.disableVertexAttribArray(text_shader.v_pos);
	GL.bindBuffer( GL.ARRAY_BUFFER, null );
}

function text_isready() {
	return text_readycount >= text_origcount && text_shader;
}

