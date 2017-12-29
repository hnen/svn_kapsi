
	var DEBUG = 1;
	var debug;

	var shader_blend;
	var shader_particle;
	var shader_gradient;
	var shader_moveprts;
	var shader_mesh;
	var mesh_kuutio;
	var mesh_f;
	var vb_quad

	var cnt_frame = 0;

	function start() {
		GL = gl_createGL( "GLCanvas" );
		GL.getExtension("OES_element_index_uint");
		debug = document.getElementById("debug");

		musa_init();
		timeline_init();
		vfield_init();

		vb_quad = gl_createQuad();

		gl_createShader("data/tex2d.vs", "data/copy.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.uniform_tex = GL.getUniformLocation(shader, "tex");
					shader.uniform_size = GL.getUniformLocation(shader, "size");
					shader_blend = shader;
				}
			);

		gl_createShader("data/tex2d.vs", "data/copy_gs.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.uniform_tex = GL.getUniformLocation(shader, "tex");
					shader.uniform_size = GL.getUniformLocation(shader, "size");
					shader_blend_gs = shader;
				}
			);
		gl_createShader("data/tex2d.vs", "data/gradient.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.uniform_tex = GL.getUniformLocation(shader, "tex");
					shader_gradient = shader;
				}
			);
		gl_createShader("data/tex2d.vs", "data/moveprts.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.uniform_tex = GL.getUniformLocation(shader, "tex");
					shader.uniform_vfieldtex = GL.getUniformLocation(shader, "vfield");
					shader.uniform_frame = GL.getUniformLocation(shader, "current_frame");
					shader.uniform_prtlifetime = GL.getUniformLocation(shader, "particlelifetime");
					shader.uniform_chainlen = GL.getUniformLocation(shader, "chain_length");
					shader.uniform_texsize_x = GL.getUniformLocation(shader, "tex_size_x");
					shader.uniform_texsize_y = GL.getUniformLocation(shader, "tex_size_y");
					shader_moveprts = shader;
				}
			);
		gl_createShader("data/renderparticle.vs", "data/renderparticle.fs",
				function(shader) {
					shader.attrib_corner = GL.getAttribLocation(shader, "in_corner");
					shader.attrib_id = GL.getAttribLocation(shader, "in_id");
					shader.attrib_uv = GL.getAttribLocation(shader, "in_uv");
					shader.uniform_particledata_tex = GL.getUniformLocation(shader, "tex");
					shader.uniform_particle_tex = GL.getUniformLocation(shader, "tex_particle");
					shader.uniform_aspect = GL.getUniformLocation(shader, "aspect");
					shader.uniform_tex_size_x = GL.getUniformLocation(shader, "tex_size_x");
					shader.uniform_tex_size_y = GL.getUniformLocation(shader, "tex_size_y");
					shader_particle = shader;
				}
			);
		gl_createShader("data/mesh.vs", "data/mesh.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.attrib_norm = GL.getAttribLocation(shader, "in_norm");
					shader.uniform_localtoscreen = GL.getUniformLocation(shader, "local_to_screen");
					shader.uniform_localtoview = GL.getUniformLocation(shader, "local_to_view");
					shader_mesh = shader;
				}
			);

		meshimport_import("data/kuutio.f", function(mesh) {
			console.log("Loaded kuutio.f, vertcount: " + mesh.vertex_count + " facecount: " + mesh.face_count);

			mesh.vb_position = gl_createVB(mesh.positions, 3);
			mesh.vb_normal = gl_createVB(mesh.normals, 3);
			mesh.ib = gl_createIB(mesh.triangles);

			mesh_kuutio = mesh;
		} );
		meshimport_import("data/f.f", function(mesh) {
			console.log("Loaded f.f, vertcount: " + mesh.vertex_count + " facecount: " + mesh.face_count);

			mesh.vb_position = gl_createVB(mesh.positions, 3);
			mesh.vb_normal = gl_createVB(mesh.normals, 3);
			mesh.ib = gl_createIB(mesh.triangles);

			mesh_f = mesh;
		} );

		flow_init();

		var self = this;
		setTimeout( function() { wait(); }, 10 );

		if (DEBUG) {
			bindDebugControls();
			setInterval( timeline_init, 500 );
		}

		onPressed( KEY_SPACE, function() {
			user_confirmed = true;
		} );

	}

	var user_confirmed = false;

	function getLoadingProgress() {
		return file_loadedResources / file_totalResources;
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
		GL.clear(GL.COLOR_BUFFER_BIT | GL.DEPTH_BUFFER_BIT);

		//vfield_render();
		flow_render();

		var target = 60.0;
//		while ( 1000 * (1.0/target - dt) < 0 ) {
//			target /= 2;
//		}
		setTimeout( render, Math.floor(1000/target) );
		if ( DEBUG ) {
			debug.innerText = "Time: " + p_musa_audio_el.currentTime + "\ntgtfps: " + target;
		}
		cnt_frame+=1;
	}

var samples = 0;
var T = 0;
	function wait() {
		if (getLoadingProgress() >= 1)
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

	function bindDebugControls() {
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
		onPressed( KEY_R, function() {
			gl_reloadShaders();
		} );
	}

start();

