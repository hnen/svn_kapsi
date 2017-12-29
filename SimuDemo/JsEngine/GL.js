

var g_Canvas;
var wgl;

var gl_textures = {};

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

function gl_resizeRenderTexture(fb, w, h) {
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
        return gl_createRenderTexture(fb.name,w,h);
    } else {
        fb.width = w;
        fb.height = h;
        return fb;
    }
}


function gl_createRenderTexture(name,w,h) {

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

    var fb = wgl.createFramebuffer();
    wgl.bindFramebuffer( wgl.FRAMEBUFFER, fb );
    fb.width = w;
    fb.height = h;

    var tex = wgl.createTexture();
    wgl.bindTexture( wgl.TEXTURE_2D, tex );
    wgl.texParameteri( wgl.TEXTURE_2D, wgl.TEXTURE_MAG_FILTER, wgl.LINEAR );
    wgl.texParameteri( wgl.TEXTURE_2D, wgl.TEXTURE_MIN_FILTER, wgl.LINEAR );
    wgl.texParameteri( wgl.TEXTURE_2D, wgl.TEXTURE_WRAP_S, wgl.CLAMP_TO_EDGE );
    wgl.texParameteri( wgl.TEXTURE_2D, wgl.TEXTURE_WRAP_T, wgl.CLAMP_TO_EDGE );
    wgl.texImage2D( wgl.TEXTURE_2D, 0, wgl.RGBA, w, h, 0, wgl.RGBA, wgl.UNSIGNED_BYTE, null );
    wgl.framebufferTexture2D( wgl.FRAMEBUFFER, wgl.COLOR_ATTACHMENT0, wgl.TEXTURE_2D, tex, 0 );

    var depth = wgl.createRenderbuffer();
    wgl.bindRenderbuffer( wgl.RENDERBUFFER, depth );
    wgl.renderbufferStorage( wgl.RENDERBUFFER, wgl.DEPTH_COMPONENT16, w, h );

    wgl.framebufferRenderbuffer( wgl.FRAMEBUFFER, wgl.DEPTH_ATTACHMENT, wgl.RENDERBUFFER, depth );

    wgl.bindTexture( wgl.TEXTURE_2D, null );
    wgl.bindRenderbuffer( wgl.RENDERBUFFER, null );
    wgl.bindFramebuffer( wgl.FRAMEBUFFER, null );

    var tx = { name: name, framebuffer: fb, texture: tex, depth: depth, width: ow, height: oh };
    p_gl_registerTexture(tx);
    return tx;
}

function gl_deleteRenderTexture(rt) {
    wgl.deleteTexture( rt.texture );
    wgl.deleteFramebuffer( rt.framebuffer );
    wgl.deleteRenderbuffer( rt.depth );
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

function gl_createTextureFromImage(name, filename, onready) {
    var img = new Image();
    img.onload = function() {

        var w = 1;
        var h = 1;
        while( w < img.width ) w *= 2;
        while( h < img.height ) h *= 2;

        var data = p_gl_createCroppedImageData( img, w, h );

        var texture = wgl.createTexture();
        wgl.bindTexture( wgl.TEXTURE_2D, texture );
        wgl.texImage2D( wgl.TEXTURE_2D, 0, wgl.RGBA, wgl.RGBA, wgl.UNSIGNED_BYTE, data );
        wgl.texParameteri( wgl.TEXTURE_2D, wgl.TEXTURE_MAG_FILTER, wgl.LINEAR );
        wgl.texParameteri( wgl.TEXTURE_2D, wgl.TEXTURE_MIN_FILTER, wgl.LINEAR );
        wgl.bindTexture( wgl.TEXTURE_2D, null );
        var tx = { name: name, texture: texture, width: img.width, height: img.height, texture_width: w, texture_height: h, size_x: img.width / w, size_y: img.height / h };

        gl_textures[name] = tx;
        onready(tx);
        file_loadedResources += 1;

        // remove img from loaded_images
//		var i = loaded_images.indexOf( img );
//		if ( i > -1 ) {
//			loaded_images.splice( i, 1 );
//		}
    }
    img.crossOrigin = "";
    img.src = filename;
    loaded_images.push( img );
    file_totalResources += 1;
}

function gl_createTextureFromArrayRGBA(name, data, width, height, onready) {
    assert(width*height*4 === data.length, "Wrong sized array for RGBA texture!");
    assert(data.toString() === "[object Uint8Array]", "Wrong typed array for texture! Must be Uint8Array");
    p_gl_createTextureFromArray(name, data, width, height, onready, wgl.RGBA);
}
function gl_createTextureFromArrayA(name, data, width, height, onready) {
    assert(width*height === data.length, "Wrong sized array for grayscale texture!");
    assert(data.toString() === "[object Uint8Array]", "Wrong typed array for texture! Must be Uint8Array");
    p_gl_createTextureFromArray(name, data, width, height, onready, wgl.ALPHA);
}

function p_gl_createTextureFromArray(name, data, width, height, onready, format) {
    var w = 1;
    var h = 1;
    while( w < width ) w *= 2;
    while( h < height ) h *= 2;

    var texture = wgl.createTexture();
    wgl.bindTexture( wgl.TEXTURE_2D, texture );
    wgl.texImage2D( wgl.TEXTURE_2D, 0, format, w, h, 0, format, wgl.UNSIGNED_BYTE, data );
    wgl.texParameteri( wgl.TEXTURE_2D, wgl.TEXTURE_MAG_FILTER, wgl.LINEAR );
    wgl.texParameteri( wgl.TEXTURE_2D, wgl.TEXTURE_MIN_FILTER, wgl.LINEAR );
    wgl.bindTexture( wgl.TEXTURE_2D, null );
    var tx = { name: name, texture: texture, width: width, height: height, texture_width: w, texture_height: h, size_x: width / w, size_y: height / h };
    p_gl_registerTexture(tx);
    onready(tx);
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

function p_gl_createShaderFromScript(program, type, script) {
    var shader = wgl.createShader( type );

    wgl.shaderSource( shader, script );
    wgl.compileShader( shader );

    if ( !wgl.getShaderParameter( shader, wgl.COMPILE_STATUS ) ) {
        alert( "Compile error for " + program + ":\n" + wgl.getShaderInfoLog( shader ) );
        return;
    }

    wgl.attachShader( program, shader );
    return shader;
}

function gl_loadShaderFromFile(vs_id, fs_id, onshaderlinked) {
    loaded_shaders.push( { vs: vs_id, fs: fs_id, func: onshaderlinked } );
    function LoadShader( shader_id, program, type, onready ) {
        file_loadText( shader_id, function(script) {
            var shader = p_gl_createShaderFromScript(program, type, script);
            onready( shader );
        } );
    }

    var vs = null, fs = null;

    var program = wgl.createProgram();

    function finalize() {
        wgl.linkProgram( program );
        if ( !wgl.getProgramParameter( program, wgl.LINK_STATUS ) ) {
            alert( "Error while linking shaders " + vs_id + " and " + fs_id );
            return null;
        }
        onshaderlinked( program );
    }

    LoadShader( vs_id, program, wgl.VERTEX_SHADER, function(_vs) { vs = _vs; if ( fs ) finalize(); } );
    LoadShader( fs_id, program, wgl.FRAGMENT_SHADER, function(_fs) { fs = _fs; if ( vs ) finalize(); } );

}

function gl_loadShaderFromScripts(vs_src, fs_src, onshaderlinked) {
    loaded_shaders.push( { vs: vs_src, fs: fs_src, func: onshaderlinked } );
    function LoadShader(script, program, type, onready) {
        onready(p_gl_createShaderFromScript(program, type, script));
    }

    var vs = null, fs = null;

    var program = wgl.createProgram();

    function finalize() {
        wgl.linkProgram( program );
        if ( !wgl.getProgramParameter( program, wgl.LINK_STATUS ) ) {
            alert( "Error while linking shaders " + vs_src + " and " + fs_src );
            return null;
        }
        onshaderlinked( program );
    }

    LoadShader(vs_src, program, wgl.VERTEX_SHADER, function(_vs) { vs = _vs; if ( fs ) finalize(); } );
    LoadShader(fs_src, program, wgl.FRAGMENT_SHADER, function(_fs) { fs = _fs; if ( vs ) finalize(); } );
}


function gl_destroyVB( vb ) {
    wgl.deleteBuffer( vb );
}

function gl_createVB(vertices_float32, itemSize) {
    var ret = wgl.createBuffer();
    wgl.bindBuffer( wgl.ARRAY_BUFFER, ret );
    wgl.bufferData( wgl.ARRAY_BUFFER, vertices_float32, wgl.STATIC_DRAW );
    ret.itemSize = itemSize;
    ret.numItems = vertices_float32.length / ret.itemSize;
    return ret;
}
function gl_createIB(vertices_uint32) {
    var ret = wgl.createBuffer();
    wgl.bindBuffer( wgl.ELEMENT_ARRAY_BUFFER, ret );
    wgl.bufferData( wgl.ELEMENT_ARRAY_BUFFER, vertices_uint32, wgl.STATIC_DRAW );
    ret.itemSize = 1;
    ret.numItems = vertices_uint32.length;
    return ret;
}

function gl_createQuad() {
    return gl_createVB( new Float32Array([
         1.0, 1.0, 0.0,
        -1.0, 1.0, 0.0,
         1.0,-1.0, 0.0,
        -1.0,-1.0, 0.0,
    ]), 3 );
}

function gl_createQuadUv() {
    return gl_createVB( new Float32Array([
        1.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        0.0, 0.0,
        ]), 2 );
    }
function gl_setRenderTarget(rt) {
    wgl.bindFramebuffer( wgl.FRAMEBUFFER, rt.framebuffer );
    wgl.viewport( 0, 0, rt.width, rt.height );
}
function gl_resetRenderTarget() {
    wgl.bindFramebuffer( wgl.FRAMEBUFFER, null );
    wgl.viewport( 0, 0, wgl.drawingBufferWidth, wgl.drawingBufferHeight );
}

function gl_clear(col) {
    if (col) {
        wgl.clearColor(col[0], col[1], col[2], col[3]);
    }
    wgl.clear(wgl.COLOR_BUFFER_BIT | wgl.DEPTH_BUFFER_BIT);
}

function p_gl_registerTexture(tx) {
    gl_textures[tx.name] = tx;
}
