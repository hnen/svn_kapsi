
	var DEBUG = 0;
	var debug;

	var blit_shader;
	var blur_shader;
	var blend_shader;
	var blur_rt;
	var tex_pal;
	var vign = { filenames: ["data/vign0.png", "data/vign1.png", "data/vign2.png", "data/vign3.png", "data/vign4.png", "data/vign5.png", "data/vign6.png" ] };

	function start() {
		GL = gl_createGL( "GLCanvas" );
		debug = document.getElementById( "debug" );

		musa_init();
		anims_init();
		molecule_init();
		timeline_init();
		text_init();
		tonnijani_init();

		if (DEBUG) {
			setInterval( timeline_init, 500 );
			onPressed( KEY_R, function() {
				gl_reloadShaders();
			} );
		}

		gl_createShader( "data/blit.vs", "data/blit.fs", function(program) {
			blit_shader = program;
			blit_shader.vertexPositionAttribute = GL.getAttribLocation(blit_shader, "in_Pos");
		} );
		gl_createShader( "data/blit.vs", "data/blur.fs", function(program) {
			blur_shader = program;
			blur_shader.vertexPositionAttribute = GL.getAttribLocation(blur_shader, "in_Pos");
		} );
		gl_createShader( "data/blit.vs", "data/blend.fs", function(program) {
			blend_shader = program;
			blend_shader.vertexPositionAttribute = GL.getAttribLocation(blend_shader, "in_Pos");
		} );

		gl_createTextureFromImage( "data/palette2.png", function(texture) {
			tex_pal = texture;
		} );

		
		vign.texture = [];
		for ( var v in vign.filenames ) {
			gl_createTextureFromImage( vign.filenames[v], function(tex) {
				vign.texture.push( tex );
			} );
		}


		var rtsize = gl_getCanvasSize(16,9);
		blur_rt = gl_createRenderTexture( rtsize[0], rtsize[1]); 
		o = window.onresize;
		window.onresize = function() { o(); var nsize = gl_getCanvasSize(16,9); blur_rt = gl_resizeRenderTexture( blur_rt, nsize[0], nsize[1]); };

		var self = this;
		setTimeout( function() { wait(); }, 10 );

		if (DEBUG) {
			// debug stuff
			onPressed( KEY_SPACE, function() {
				if ( p_musa_audio_el.paused ) {
					p_musa_audio_el.play();
				} else {
					p_musa_audio_el.pause();
				}
			} );
			onPressed( KEY_LEFT, function() {
				p_musa_audio_el.currentTime -= 2.5;
			} );
			onPressed( KEY_RIGHT, function() {
				p_musa_audio_el.currentTime += 2.5;
			} );
			onPressed( KEY_DOWN, function() {
				p_musa_audio_el.currentTime -= 10.0;
			} );
			onPressed( KEY_UP, function() {
				p_musa_audio_el.currentTime += 10.0;
			} );
		}

		onPressed( KEY_SPACE, function() {
			if ( musa_isloaded() && anims_isready() && timeline_ready && blit_shader && text_isready() ) {
				user_confirmed = true;
			}
		} );

	}

	var user_confirmed = false;

	function getLoadingProgress() {
		var prog1 = anims_getLoadingProgress();
		var prog2 = musa_isloaded() ? 1.0 : 0.0;
		return prog1 * 0.85 + prog2 * 0.15;
	}

	function wait() {
		if ( musa_isloaded() && anims_isready() && timeline_ready && blit_shader && text_isready() )
		{
			// remove loading text
			var loading = document.getElementById( 'loading' );
			if ( loading ) {
				loading.parentElement.removeChild( loading );
			}
			if ( user_confirmed ) {
				var progress = document.getElementById( 'progress' );
				progress.parentElement.removeChild( progress );

				// show canvas
				document.body.appendChild( g_Canvas );

				musa_start();
				//setInterval( render, 32 );
				setTimeout( render, 32 );
			} else {
				var progress = document.getElementById( 'progress' );
				progress.innerText = "Press SPACE to start.";
				setTimeout( wait, 10 );
			}
		} else {
			document.getElementById( "progress" ).innerText = "" + Math.round(getLoadingProgress() * 100);
			setTimeout( wait, 10 );
		}
	}
	var demo_edgeradius = 0.0002;
	var demo_edgestrength = 4.0;
	var demo_bgstrength = 0.5;
	var demo_edgethreshold = 1.0;
	var demo_bgpow = 6.0;
	var demo_colorscale = 20.0;

	function copy( from, to ) {
		GL.bindFramebuffer( GL.FRAMEBUFFER, getlayer(to).framebuffer );
		GL.viewport( 0, 0, getlayer(to).width, getlayer(to).height );
		GL.clear( GL.COLOR_BUFFER_BIT | GL.DEPTH_BUFFER_BIT );
		for( var n in from ) {
			var name = from[n];
			var rndtype = "transparent";
			var layer = null;
			var mul = 1.0;
			if ( Array.isArray(name) ) {
				var layername = name[0];
				layer = getlayer(layername);
				rndtype = name[1];
				mul = name[2];
			} else {
				layer = getlayer(name);
			}
			GL.useProgram( blend_shader );
			GL.activeTexture( GL.TEXTURE0 );
			GL.bindTexture( GL.TEXTURE_2D, layer.texture );
			GL.uniform1i( GL.getUniformLocation( blend_shader, "tex" ), 0 );
			GL.uniform2f( GL.getUniformLocation( blend_shader, "size" ), layer.width / layer.framebuffer.width, layer.height / layer.framebuffer.height );
			GL.uniform1f( GL.getUniformLocation( blend_shader, "mul" ), mul );
			GL.enableVertexAttribArray(blend_shader.vertexPositionAttribute);
			GL.bindBuffer( GL.ARRAY_BUFFER, vb_quad );
			GL.vertexAttribPointer( blend_shader.vertexPositionAttribute, vb_quad.itemSize, GL.FLOAT, false, 0, 0 );
			GL.enable( GL.BLEND );
			if ( rndtype == "transparent" ) {	
				GL.blendFunc( GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA );
			} else {
				GL.blendFunc( GL.SRC_ALPHA, GL.ONE );
			}
			GL.drawArrays( GL.TRIANGLE_STRIP, 0, vb_quad.numItems );

			GL.disableVertexAttribArray(blend_shader.vertexPositionAttribute);
			GL.bindBuffer( GL.ARRAY_BUFFER, null );
		}

		
	}

	function blur( in_rt, out_rt, amount, brightness ) {

		GL.useProgram( blur_shader );
		GL.activeTexture( GL.TEXTURE0 );
		GL.bindTexture( GL.TEXTURE_2D, in_rt.texture );
		GL.uniform1i( GL.getUniformLocation( blur_shader, "tex" ), 0 );
		GL.uniform1f( GL.getUniformLocation( blur_shader, "radius" ), amount );
		GL.uniform1f( GL.getUniformLocation( blur_shader, "brightness" ), brightness );
		GL.uniform2f( GL.getUniformLocation( blur_shader, "dir" ), 9.0/16, 0 );
		GL.uniform2f( GL.getUniformLocation( blur_shader, "size" ), in_rt.width / in_rt.framebuffer.width, in_rt.height / in_rt.framebuffer.height );
		GL.bindBuffer( GL.ARRAY_BUFFER, vb_quad );
		GL.enableVertexAttribArray(blur_shader.vertexPositionAttribute);
		GL.vertexAttribPointer( blur_shader.vertexPositionAttribute, vb_quad.itemSize, GL.FLOAT, false, 0, 0 );
		GL.disable( GL.BLEND );
		GL.disable( GL.DEPTH_TEST );

		GL.bindFramebuffer( GL.FRAMEBUFFER, blur_rt.framebuffer );
		GL.clear( GL.COLOR_BUFFER_BIT | GL.DEPTH_BUFFER_BIT );
		GL.viewport( 0, 0, blur_rt.width, blur_rt.height );
		GL.drawArrays( GL.TRIANGLE_STRIP, 0, vb_quad.numItems );

		GL.bindFramebuffer( GL.FRAMEBUFFER, out_rt.framebuffer );
		GL.clear( GL.COLOR_BUFFER_BIT | GL.DEPTH_BUFFER_BIT );
		GL.viewport( 0, 0, out_rt.width, out_rt.height );
		GL.bindTexture( GL.TEXTURE_2D, blur_rt.texture );
		GL.uniform1i( GL.getUniformLocation( blur_shader, "tex" ), 0 );
		GL.uniform2f( GL.getUniformLocation( blur_shader, "dir" ), 0, 1 );
		GL.drawArrays( GL.TRIANGLE_STRIP, 0, vb_quad.numItems );

		GL.bindFramebuffer( GL.FRAMEBUFFER, null );
		
		GL.disableVertexAttribArray(blur_shader.vertexPositionAttribute);
		GL.bindBuffer( GL.ARRAY_BUFFER, null );
	}

	var lt;
	function render() {
		time_frameChanged();
		var dt = p_musa_audio_el.currentTime - lt;
		lt = p_musa_audio_el.currentTime;

		GL.useProgram( null );
		GL.clearColor( 0.0, 0.0, 0.0, 1.0 );

		timeline_update();

		GL.bindFramebuffer( GL.FRAMEBUFFER, null );

		GL.viewport( 0, 0, GL.drawingBufferWidth, GL.drawingBufferHeight );

		GL.useProgram( blit_shader );
		GL.activeTexture( GL.TEXTURE0 );
		GL.bindTexture( GL.TEXTURE_2D, getlayer("fb").texture );
		GL.uniform1i( GL.getUniformLocation( blit_shader, "tex" ), 0 );
		GL.activeTexture( GL.TEXTURE1 );
		GL.bindTexture( GL.TEXTURE_2D, tex_pal.texture );
		GL.uniform1i( GL.getUniformLocation( blit_shader, "tex_pal" ), 1 );
		GL.activeTexture( GL.TEXTURE2 );
		var vign_tex = vign.texture[Math.floor(mod(p_musa_audio_el.currentTime*20.0,vign.texture.length))];
		GL.bindTexture( GL.TEXTURE_2D, vign_tex.texture );
		GL.uniform1i( GL.getUniformLocation( blit_shader, "tex_vign" ), 2 );
		GL.uniform2f( GL.getUniformLocation( blit_shader, "tex_vign_size" ), vign_tex.width / vign_tex.texture_width, vign_tex.height / vign_tex.texture_height );
		

		GL.enableVertexAttribArray(blit_shader.vertexPositionAttribute);
		GL.bindBuffer( GL.ARRAY_BUFFER, vb_quad );
		GL.vertexAttribPointer( blit_shader.vertexPositionAttribute, vb_quad.itemSize, GL.FLOAT, false, 0, 0 );

		GL.uniform2f( GL.getUniformLocation( blit_shader, "size" ), getlayer("fb").width / getlayer("fb").framebuffer.width, getlayer("fb").height / getlayer("fb").framebuffer.height );
		GL.uniform2f( GL.getUniformLocation( blit_shader, "fb_size" ), getlayer("fb").width / getlayer("fb").framebuffer.width, getlayer("fb").height / getlayer("fb").framebuffer.height );
		GL.uniform1f( GL.getUniformLocation( blit_shader, "edgeradius" ), demo_edgeradius );
		GL.uniform1f( GL.getUniformLocation( blit_shader, "edgestrength" ), demo_edgestrength );
		GL.uniform2f( GL.getUniformLocation( blit_shader, "tex_pal_size" ), tex_pal.width / tex_pal.texture_width, tex_pal.height / tex_pal.texture_height );
		GL.uniform1f( GL.getUniformLocation( blit_shader, "bgstrength" ), demo_bgstrength );
		GL.uniform1f( GL.getUniformLocation( blit_shader, "edgethreshold" ), demo_edgethreshold );
		GL.uniform1f( GL.getUniformLocation( blit_shader, "bgpow" ), demo_bgpow );
		GL.uniform1f( GL.getUniformLocation( blit_shader, "colorscale" ), demo_colorscale );
		GL.disable( GL.BLEND );
		GL.disable( GL.DEPTH_TEST );
		GL.drawArrays( GL.TRIANGLE_STRIP, 0, vb_quad.numItems );


		var target = 30.0;
//		while ( 1000 * (1.0/target - dt) < 0 ) {
//			target /= 2;
//		}
		setTimeout( render, 1000 * (1.0/target - dt) );
		if ( DEBUG ) {
			debug.innerText = "Time: " + p_musa_audio_el.currentTime + "\ntftfps: " + target;
		}

	}

start();

