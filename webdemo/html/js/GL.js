

var g_Canvas;
var GL;

function gl_createGL(canvasId) {
	
	// create canvas and do some diibadaaba
	g_Canvas = document.createElement( 'canvas' );

	p_gl_fitCanvas( 16, 9 );
	window.onresize = function() { p_gl_fitCanvas( 16, 9 ); }

	if ( g_Canvas.getContext( "webgl" ) ) {
		return g_Canvas.getContext( "webgl" );
	} else if ( g_Canvas.getContext( "experimental-webgl" ) ) {
		return g_Canvas.getContext( "experimental-webgl" );
	} else {
		alert( "Error initializing WebGL!" );
		return null;
	}

}

function gl_getCanvasSize(W,H) {
	var rw = document.body.clientWidth;
	var rh = document.body.clientHeight;

	var fw = 0, vh = 0;
	if ( rw/rh > W/H ) {
		fh = rh;
		fw = W*fh/H;
	} else {
		fw = rw;
		fh = fw * H / W;
	}
	return [fw, fh];
}

function p_gl_fitCanvas( W, H ) {
	var rw = document.body.clientWidth;
	var rh = document.body.clientHeight;

	var fw = 0, vh = 0;
	if ( rw/rh > W/H ) {
		fh = rh;
		fw = W*fh/H;
	} else {
		fw = rw;
		fh = fw * H / W;
	}
	g_Canvas.width = fw;
	g_Canvas.height = fh;
}

function gl_resizeRenderTexture( fb, w, h ) {
	var ow = w;
	var oh = h;

	w = 1;
	h = 1;

	while( w < ow ) w *= 2;
	while( h < oh ) h *= 2;

	w = min(1024,w);
	h = min(1024,h);

	while( w < ow ) ow = w;
	while( h < oh ) oh = h;

	if ( w != fb.framebuffer.width || h != fb.framebuffer.height ) {
		gl_deleteRenderTexture( fb );
		return gl_createRenderTexture( w,h );
	} else {
		fb.width = w;
		fb.height = h;
		return fb;
	}
}


function gl_createRenderTexture( w,h ) {

	var ow = w;
	var oh = h;

	w = 1;
	h = 1;

	while( w < ow ) w *= 2;
	while( h < oh ) h *= 2;

	w = min(1024,w);
	h = min(1024,h);

	while( w < ow ) ow = w;
	while( h < oh ) oh = h;

	var fb = GL.createFramebuffer();
	GL.bindFramebuffer( GL.FRAMEBUFFER, fb );
	fb.width = w;
	fb.height = h;

	var tex = GL.createTexture();
	GL.bindTexture( GL.TEXTURE_2D, tex );
	GL.texParameteri( GL.TEXTURE_2D, GL.TEXTURE_MAG_FILTER, GL.LINEAR );
	GL.texParameteri( GL.TEXTURE_2D, GL.TEXTURE_MIN_FILTER, GL.LINEAR );
	GL.texImage2D( GL.TEXTURE_2D, 0, GL.RGBA, w, h, 0, GL.RGBA, GL.UNSIGNED_BYTE, null );
	GL.framebufferTexture2D( GL.FRAMEBUFFER, GL.COLOR_ATTACHMENT0, GL.TEXTURE_2D, tex, 0 );

	var depth = GL.createRenderbuffer();
	GL.bindRenderbuffer( GL.RENDERBUFFER, depth );
	GL.renderbufferStorage( GL.RENDERBUFFER, GL.DEPTH_COMPONENT16, w, h );

	GL.framebufferRenderbuffer( GL.FRAMEBUFFER, GL.DEPTH_ATTACHMENT, GL.RENDERBUFFER, depth );

	GL.bindTexture( GL.TEXTURE_2D, null );
	GL.bindRenderbuffer( GL.RENDERBUFFER, null );
	GL.bindFramebuffer( GL.FRAMEBUFFER, null );

	return { framebuffer: fb, texture: tex, depth: depth, width: ow, height: oh };
}

function gl_deleteRenderTexture(rt) {
	GL.deleteTexture( rt.texture );
	GL.deleteFramebuffer( rt.framebuffer );
	GL.deleteRenderbuffer( rt.depth );
	rt.texture = null;
	rt.framebuffer = null;
	rt.depth = null;
}

var loaded_images = [];
var canvas_tmp = null;

function p_gl_createCroppedImageData( image, w, h ) {
	if ( !canvas_tmp ) {
		canvas_tmp = document.createElement( 'canvas' );
	}
	canvas_tmp.width = w;
	canvas_tmp.height = h;
	var ctx = canvas_tmp.getContext( '2d' ) ;
	ctx.clearRect( 0,0,w,h );
	ctx.drawImage( image, 0, 0 );
	return ctx.getImageData( 0,0,w,h );
}

function gl_createTextureFromImage( filename, onready ) {
	var img = new Image();
	img.onload = function() {

		var w = 1;
		var h = 1;
		while( w < img.width ) w *= 2;
		while( h < img.height ) h *= 2;

		var data = p_gl_createCroppedImageData( img, w, h );
		
		var texture = GL.createTexture();
		GL.bindTexture( GL.TEXTURE_2D, texture );
		GL.texImage2D( GL.TEXTURE_2D, 0, GL.RGBA, GL.RGBA, GL.UNSIGNED_BYTE, data );
		GL.texParameteri( GL.TEXTURE_2D, GL.TEXTURE_MAG_FILTER, GL.LINEAR );
		GL.texParameteri( GL.TEXTURE_2D, GL.TEXTURE_MIN_FILTER, GL.LINEAR );
		GL.bindTexture( GL.TEXTURE_2D, null );
		onready( { texture: texture, width: img.width, height: img.height, texture_width: w, texture_height: h } );
		
		// remove img from loaded_images
//		var i = loaded_images.indexOf( img );
//		if ( i > -1 ) {
//			loaded_images.splice( i, 1 );
//		}
	}
	img.crossOrigin = "";
	img.src = filename;
	loaded_images.push( img );
}

var loaded_shaders = [];

function gl_reloadShaders() {
	var shaderlist = loaded_shaders;
	loaded_shaders = [];
	for ( var i in shaderlist ) {
		var shader = shaderlist[i]; 
		gl_createShader( shader.vs, shader.fs, shader.func );
	}
}

function gl_createShader( vs_id, fs_id, onshaderlinked ) {
	loaded_shaders.push( { vs: vs_id, fs: fs_id, func: onshaderlinked } );
	function LoadShader( shader_id, program, type, onready ) {
		file_loadText( shader_id, function(script) {
			var shader = GL.createShader( type );

			GL.shaderSource( shader, script );
			GL.compileShader( shader );
			
			if ( !GL.getShaderParameter( shader, GL.COMPILE_STATUS ) ) {
				alert( "Compile error for " + shader_id + ":\n" + GL.getShaderInfoLog( shader ) );
				return;
			}

			GL.attachShader( program, shader );
			onready( shader );
		} );
	}

	var vs = null, fs = null;

	var program = GL.createProgram();

	function finalize() {
		GL.linkProgram( program );
		if ( !GL.getProgramParameter( program, GL.LINK_STATUS ) ) {
			alert( "Error while linking shaders " + vs_id + " and " + fs_id );
			return null;
		}
		onshaderlinked( program );
	}

	LoadShader( vs_id, program, GL.VERTEX_SHADER, function(_vs) { vs = _vs; if ( fs ) finalize(); } );
	LoadShader( fs_id, program, GL.FRAGMENT_SHADER, function(_fs) { fs = _fs; if ( vs ) finalize(); } );

}

function gl_destroyVB( vb ) {
	GL.deleteBuffer( vb );
}

function gl_createVB( vertices ) {
	var ret = GL.createBuffer();
	GL.bindBuffer( GL.ARRAY_BUFFER, ret );
	GL.bufferData( GL.ARRAY_BUFFER, new Float32Array( vertices ), GL.STATIC_DRAW );
	ret.itemSize = 3;
	ret.numItems = vertices.length / ret.itemSize;
	return ret;
}

function gl_createQuad() {
	return gl_createVB( [
		 1.0, 1.0, 0.0,
		-1.0, 1.0, 0.0,
		 1.0,-1.0, 0.0,
		-1.0,-1.0, 0.0,
	] );
}

