
var p_animdata = [
	{ name: "asentoi", count: 190 },	// 0
	{ name: "aivot", count: 52 },		// 1
	{ name: "autoi", count: 22 },		// 2
	{ name: "bussi", count: 20 },
	{ name: "cliff", count: 10 },
	{ name: "fiilis", count: 19 },
	{ name: "hahmo", count: 11 },
	{ name: "hakkaa", count: 24 },
	{ name: "hebepr", count: 12 },
	{ name: "huutaa", count: 39 },
	{ name: "hyppy", count: 10 },
	{ name: "ihmisii", count: 21 },
	{ name: "istuu", count: 22 },
	{ name: "jotai", count: 16 },
	{ name: "kallo", count: 14 },
	{ name: "kopykat", count: 133 },
	{ name: "kytat", count: 48 },
	{ name: "lasit", count: 12 },
	{ name: "nainen", count: 18 },
	{ name: "nauru", count: 45 },
	{ name: "paeae", count: 11 },
	{ name: "party", count: 28 },
	{ name: "peili", count: 21 },
	{ name: "pillereit", count: 27 },
	{ name: "poeoe", count: 11 },
	{ name: "raa", count: 24 },
	{ name: "raaa", count: 79 },
	{ name: "ritual", count: 48 },
	{ name: "ritual2", count: 54 },
	{ name: "ruisku", count: 73 },
	{ name: "ruumishuone", count: 51 },
	{ name: "selitta", count: 22 },
	{ name: "selitys", count: 24 },
	{ name: "soosoo", count: 18 },
];


var anims_shader;
var vb_quad;

var p_anims_loadingcounter = 0;
var p_anims_totalload = 0;

var prev_anim_id = 0;
var prev_frame_id = 0;

var anim_id = 0;
var frame_id = 0;

function anims_init() { 

	for ( var anim_i in p_animdata ) {
		initAnimData( anim_i );
	}
	//anims_shader = gl_createShader( "vs_anim", "fs_anim" );
	gl_createShader( "data/anim.vs", "data/anim.fs", function(program) {
		anims_shader = program;
		anims_shader.vertexPositionAttribute = GL.getAttribLocation(anims_shader, "in_Pos");
	} );
	vb_quad = gl_createQuad();


	if ( DEBUG ) {
		onPressed( KEY_W, function() {
			anim_id = mod(anim_id + 1, p_animdata.length);
			frame_id = 0;
		} );
		onPressed( KEY_Q, function() {
			anim_id = mod(anim_id - 1, p_animdata.length);
			frame_id = 0;
		} );
	}

}

function initAnimData( anim_i ) {
	var animdata = p_animdata[anim_i];
	var url_base = "data/anims/" + animdata.name;
	var imgs = [];
	var textures = [];
	for ( var i = 1; i <= animdata.count; i++ ) {
		textures.push( null );
	}
	for ( var i = 1; i <= animdata.count; i++ ) {
		initAnimImage( i, imgs, textures, url_base );
	}
	animdata.imgs = imgs;
	animdata.textures = textures;
}

function initAnimImage( i, imgs, textures, url_base ) {
	var num_str = "" + i;
	if ( num_str.length == 1 ) num_str = "00" + num_str;
	if ( num_str.length == 2 ) num_str = "0" + num_str;
	var url = url_base + "-" + num_str + ".jpeg";
	var img = new Image();
	imgs.push( img );
	p_anims_loadingcounter++;
	p_anims_totalload++;
	img.onload = function() {
		textures[i-1] = GL.createTexture();
		GL.bindTexture( GL.TEXTURE_2D, textures[i-1] );
		GL.texImage2D( GL.TEXTURE_2D, 0, GL.RGBA, GL.RGBA, GL.UNSIGNED_BYTE, imgs[i-1] );
		GL.texParameteri( GL.TEXTURE_2D, GL.TEXTURE_MAG_FILTER, GL.LINEAR );
		GL.texParameteri( GL.TEXTURE_2D, GL.TEXTURE_MIN_FILTER, GL.LINEAR );
		GL.bindTexture( GL.TEXTURE_2D, null );
		imgs[i-1] = null;
		p_anims_loadingcounter--;
	};
	img.crossOrigin = "";
	img.src = url;
}

function anims_getLoadingProgress() {
	return 1.0 - (p_anims_loadingcounter / p_anims_totalload);
}

function anims_isready() {
	return p_anims_loadingcounter == 0 && anims_shader;
}

function anims_render_frame( anim_id, frame_id, alpha, additive, mul ) {
	GL.useProgram( anims_shader );
	//GL.uniform2f( GL.getUniformLocation( anims_shader, "g_Dir" ), dir.x, dir.y );
	GL.activeTexture( GL.TEXTURE0 );
	var texlist = p_animdata[anim_id].textures;
	GL.bindTexture( GL.TEXTURE_2D, texlist[Math.floor(mod(frame_id,texlist.length))] );
	GL.uniform1i( GL.getUniformLocation( anims_shader, "tex0" ), 0 );
	GL.uniform1f( GL.getUniformLocation( anims_shader, "ip" ), alpha );
	GL.uniform1f( GL.getUniformLocation( anims_shader, "mul" ), mul );

	GL.bindBuffer( GL.ARRAY_BUFFER, vb_quad );
	GL.enableVertexAttribArray(anims_shader.vertexPositionAttribute);
	GL.vertexAttribPointer( anims_shader.vertexPositionAttribute, vb_quad.itemSize, GL.FLOAT, false, 0, 0 );
	GL.enable( GL.BLEND );
	GL.disable( GL.DEPTH_TEST );
	if ( additive ) {
		GL.blendFunc( GL.SRC_ALPHA, GL.ONE );
	} else {
		GL.blendFunc( GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA );
	}

	GL.drawArrays( GL.TRIANGLE_STRIP, 0, vb_quad.numItems );
	
	GL.disableVertexAttribArray(anims_shader.vertexPositionAttribute);
	GL.bindBuffer( GL.ARRAY_BUFFER, null );
}


function anims_render( anim_id, speed, blur, _alpha, additive, mul ) {
	frame_id += dt * speed;

	var texlength = p_animdata[anim_id].textures.length;
	for( var fid = -blur-1; fid <= blur+1; fid += 1.0 ) {
		var f = Math.floor( frame_id + fid );
		var alpha = lerp( _alpha, 0, Math.abs(frame_id - f), blur );
		alpha = min(_alpha, max(0, alpha));
		anims_render_frame( anim_id, f, alpha, additive, mul );
	}

}


