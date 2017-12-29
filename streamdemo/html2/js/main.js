
	var DEBUG = 0;
	var debug;

	var shaders = {};
	var shader_blend;
	var shader_particle;
	var shader_gradient;
	var shader_moveprts;
	var shader_mesh;
	var shader_blur;
	var shader_blur_vfield;
	var mesh = {};
//	var mesh_kuutio;
//	var mesh_f;
//	var mesh_sphere;
//	var mesh_plane;
	var vb_quad

	var beat = 0;

	var cnt_frame = 0;

	function start() {
		GL = gl_createGL( "GLCanvas" );
		GL.getExtension("OES_element_index_uint");
		debug = document.getElementById("debug");

		musa_init();
		timeline_init();
		vfield_init();
		teksti_init();
		okkult_init();

		vb_quad = gl_createQuad();


		gl_createShader("data/tex2d.vs", "data/copy.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.uniform_tex = GL.getUniformLocation(shader, "tex");
					shader.uniform_size = GL.getUniformLocation(shader, "size");
					shader_blend = shader;
				}
			);
		gl_createShader("data/tex2d.vs", "data/manu.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.uniform_tex = GL.getUniformLocation(shader, "tex");
					shader.uniform_size = GL.getUniformLocation(shader, "size");
					shader_tex = shader;
				}
			);
		gl_createShader("data/tex2d.vs", "data/mul2.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.uniform_tex = GL.getUniformLocation(shader, "tex");
					shader.uniform_tex2 = GL.getUniformLocation(shader, "tex2");
					shader.uniform_t = GL.getUniformLocation(shader, "t");
					shader.uniform_size = GL.getUniformLocation(shader, "size");
					shader_blend_perlin = shader;
				}
			);
		gl_createShader("data/tex2d.vs", "data/blur.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.uniform_tex = GL.getUniformLocation(shader, "tex");
					shader.uniform_size = GL.getUniformLocation(shader, "size");
					shader.uniform_radius = GL.getUniformLocation(shader, "radius");
					shader.uniform_dir = GL.getUniformLocation(shader, "dir");
					shader_blur = shader;
				}
			);

		gl_createShader("data/tex2d.vs", "data/blur_vfield.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.uniform_tex = GL.getUniformLocation(shader, "tex");
					shader.uniform_size = GL.getUniformLocation(shader, "size");
					shader.uniform_radius = GL.getUniformLocation(shader, "radius");
					shader.uniform_dir = GL.getUniformLocation(shader, "dir");
					shader_blur_vfield = shader;
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

		gl_createShader("data/tekst.vs", "data/tekst.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.uniform_tex = GL.getUniformLocation(shader, "tex");
					shader.uniform_size = GL.getUniformLocation(shader, "size");
					shader.uniform_t = GL.getUniformLocation(shader, "t");
					shader.uniform_alpha = GL.getUniformLocation(shader, "alpha");
					shader.uniform_local_to_screen = GL.getUniformLocation(shader, "local_to_screen");
					shader_tekst = shader;
				}
			);
		gl_createShader("data/okkult.vs", "data/okkult.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.uniform_tex = GL.getUniformLocation(shader, "tex");
					shader.uniform_aspect = GL.getUniformLocation(shader, "aspect");
					shader.uniform_alpha = GL.getUniformLocation(shader, "alpha");
					shader.uniform_local_to_screen = GL.getUniformLocation(shader, "local_to_screen");
					shader_okkult = shader;
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
					shader.uniform_coltex = GL.getUniformLocation(shader, "tex_color");
					shader.uniform_vfieldtex = GL.getUniformLocation(shader, "vfield");
					shader.uniform_frame = GL.getUniformLocation(shader, "current_frame");
					shader.uniform_prtlifetime = GL.getUniformLocation(shader, "particlelifetime");
					shader.uniform_chainlen = GL.getUniformLocation(shader, "chain_length");
					shader.uniform_texsize_x = GL.getUniformLocation(shader, "tex_size_x");
					shader.uniform_texsize_y = GL.getUniformLocation(shader, "tex_size_y");
					shader.uniform_paska = GL.getUniformLocation(shader, "paska");
					shader.uniform_b_spawn_only_visible = GL.getUniformLocation(shader, "spawn_only_visible");
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
					shader.uniform_particle_vfield = GL.getUniformLocation(shader, "tex_field");
					shader.uniform_aspect = GL.getUniformLocation(shader, "aspect");
					shader.uniform_tex_size_x = GL.getUniformLocation(shader, "tex_size_x");
					shader.uniform_tex_size_y = GL.getUniformLocation(shader, "tex_size_y");
					shader.uniform_tex_bg_color = GL.getUniformLocation(shader, "tex_bg_color");
					shader.uniform_mul_len = GL.getUniformLocation(shader, "mul_len");
					shader.uniform_mul_brig = GL.getUniformLocation(shader, "mul_brig");
					shader.uniform_paska = GL.getUniformLocation(shader, "paska");
					shader.uniform_b_colorize_particles = GL.getUniformLocation(shader, "colorize");
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
		gl_createShader("data/mesh.vs", "data/mesh_black.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.attrib_norm = GL.getAttribLocation(shader, "in_norm");
					shader.uniform_localtoscreen = GL.getUniformLocation(shader, "local_to_screen");
					shader.uniform_localtoview = GL.getUniformLocation(shader, "local_to_view");
					shaders.mesh_black = shader;
				}
			);
		gl_createShader("data/mesh.vs", "data/mesh_white.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.attrib_norm = GL.getAttribLocation(shader, "in_norm");
					shader.uniform_localtoscreen = GL.getUniformLocation(shader, "local_to_screen");
					shader.uniform_localtoview = GL.getUniformLocation(shader, "local_to_view");
					shaders.white = shader;
				}
			);
		gl_createShader("data/mesh.vs", "data/mesh_moon.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.attrib_norm = GL.getAttribLocation(shader, "in_norm");
					shader.uniform_localtoscreen = GL.getUniformLocation(shader, "local_to_screen");
					shader.uniform_localtoview = GL.getUniformLocation(shader, "local_to_view");
					shaders.mesh_moon = shader;
				}
			);
		gl_createShader("data/mesh_tex.vs", "data/mesh_tex.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.attrib_norm = GL.getAttribLocation(shader, "in_norm");
					//shader.attrib_uv = GL.getAttribLocation(shader, "in_uv");
					shader.uniform_localtoscreen = GL.getUniformLocation(shader, "local_to_screen");
					shader.uniform_localtoview = GL.getUniformLocation(shader, "local_to_view");
					//shader.uniform_tex = GL.getUniformLocation(shader, "tex");
					shaders.mesh_tex = shader;
				}
			);
		gl_createShader("data/seppo.vs", "data/seppo.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.attrib_norm = GL.getAttribLocation(shader, "in_norm");
					//shader.attrib_uv = GL.getAttribLocation(shader, "in_uv");
					shader.uniform_localtoscreen = GL.getUniformLocation(shader, "local_to_screen");
					shader.uniform_localtoview = GL.getUniformLocation(shader, "local_to_view");
					//shader.uniform_tex = GL.getUniformLocation(shader, "tex");
					shader.uniform_t = GL.getUniformLocation(shader, "t");
					shaders.seppo = shader;
				}
			);
		gl_createShader("data/mesh_tex.vs", "data/koti.fs",
				function(shader) {
					shader.attrib_pos = GL.getAttribLocation(shader, "in_pos");
					shader.attrib_norm = GL.getAttribLocation(shader, "in_norm");
					//shader.attrib_uv = GL.getAttribLocation(shader, "in_uv");
					shader.uniform_localtoscreen = GL.getUniformLocation(shader, "local_to_screen");
					shader.uniform_localtoview = GL.getUniformLocation(shader, "local_to_view");
					//shader.uniform_tex = GL.getUniformLocation(shader, "tex");
					shaders.koti = shader;
				}
			);

		meshimport_import("data/scissors_1.f", function(mdata) {
			console.log("Loaded scissors_1.f, vertcount: " + mdata.vertex_count + " facecount: " + mdata.face_count);

			mdata.vb_position = gl_createVB(mdata.positions, 3);
			mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.ib = gl_createIB(mdata.triangles);

			mesh.scissors_1 = mdata;
		} );
		meshimport_import("data/scissors_2.f", function(mdata) {
			console.log("Loaded scissors_2.f, vertcount: " + mdata.vertex_count + " facecount: " + mdata.face_count);

			mdata.vb_position = gl_createVB(mdata.positions, 3);
			mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.ib = gl_createIB(mdata.triangles);

			mesh.scissors_2 = mdata;
		} );

		meshimport_import("data/kuutio.f", function(mdata) {
			console.log("Loaded kuutio.f, vertcount: " + mdata.vertex_count + " facecount: " + mdata.face_count);

			mdata.vb_position = gl_createVB(mdata.positions, 3);
			mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.ib = gl_createIB(mdata.triangles);

			mesh.kuutio = mdata;
		} );
		meshimport_import("data/f.f", function(mdata) {
			console.log("Loaded f.f, vertcount: " + mdata.vertex_count + " facecount: " + mdata.face_count);

			mdata.vb_position = gl_createVB(mdata.positions, 3);
			mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.ib = gl_createIB(mdata.triangles);

			mesh.f = mdata;
		} );
		meshimport_import("data/seppo.f", function(mdata) {
			console.log("Loaded seppo.f, vertcount: " + mdata.vertex_count + " facecount: " + mdata.face_count);

			mdata.vb_position = gl_createVB(mdata.positions, 3);
			mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.ib = gl_createIB(mdata.triangles);

			mesh.seppo = mdata;
		} );
		meshimport_import("data/pallo.f", function(mdata) {
			console.log("Loaded f.f, vertcount: " + mdata.vertex_count + " facecount: " + mdata.face_count);

			var uvs = [];
			// gen lame uvs
			for (var n = 0; n < mdata.positions.length; n += 3) {
				var p = [mdata.positions[n+0],mdata.positions[n+1],mdata.positions[n+2]];
				var uv = [p[0]+p[2],p[1]+p[2]];
				uvs.push(uv[0]);
				uvs.push(uv[1]);
			}

			mdata.vb_position = gl_createVB(mdata.positions, 3);
			mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(new Float32Array(uvs), 2);
			mdata.ib = gl_createIB(mdata.triangles);

			mesh.sphere = mdata;
		} );

		meshgen_circle2(10,14,32,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);			mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);						mdata.ib = gl_createIB(mdata.triangles);

			mesh.circle = mdata;
		} );
		meshgen_circle2(33, 36, 3,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);			mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);						mdata.ib = gl_createIB(mdata.triangles);

			mesh.tri1 = mdata;
		} );
		meshgen_circle2(33, 36, 4,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);			mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);						mdata.ib = gl_createIB(mdata.triangles);
			mesh.square1 = mdata;
		} );
		meshgen_circle2(36, 38, 32,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);			mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);						mdata.ib = gl_createIB(mdata.triangles);

			mesh.circle2 = mdata;
		} );
		meshgen_circle2(0.9, 1, 32,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);			mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);						mdata.ib = gl_createIB(mdata.triangles);
			mesh.circleOne = mdata;
		} );

		meshgen_plane(40,6,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);
			mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);
			mdata.ib = gl_createIB(mdata.triangles);

			mesh.plane = mdata;
		} );
		meshgen_plane(5,5,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);
			mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);
			mdata.ib = gl_createIB(mdata.triangles);

			mesh.plane2 = mdata;
		} );

		gl_createTextureFromImage("credits00", "data/hth.png", function(tex) {
		});
		gl_createTextureFromImage("credits01", "data/wdm.png", function(tex) {
		});
		gl_createTextureFromImage("credits02", "data/nosfe.png", function(tex) {
		});
		gl_createTextureFromImage("credits03", "data/astu.png", function(tex) {
		});
		gl_createTextureFromImage("credits04", "data/osa2.png", function(tex) {
		});

		meshgen_star(30,33,5,2,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);	mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);				mdata.ib = gl_createIB(mdata.triangles);
			mesh.star1 = mdata;
		} );
		meshgen_star(30,33,6,2,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);	mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);				mdata.ib = gl_createIB(mdata.triangles);
			mesh.star2 = mdata;
		} );
		meshgen_star(30,33,7,2,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);	mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);				mdata.ib = gl_createIB(mdata.triangles);
			mesh.star3 = mdata;
		} );
		meshgen_star(30,33,8,3,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);	mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);				mdata.ib = gl_createIB(mdata.triangles);
			mesh.star4 = mdata;
		} );
		meshgen_star(30,33,9,2,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);	mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);				mdata.ib = gl_createIB(mdata.triangles);
			mesh.star92 = mdata;
		} );
		meshgen_star(30,33,9,3,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);	mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);				mdata.ib = gl_createIB(mdata.triangles);
			mesh.star93 = mdata;
		} );
		meshgen_star(30,33,9,4,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);	mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);				mdata.ib = gl_createIB(mdata.triangles);
			mesh.star94 = mdata;
		} );
		meshgen_star(30,33,11,4,function(mdata) {
			mdata.vb_position = gl_createVB(mdata.positions, 3);	mdata.vb_normal = gl_createVB(mdata.normals, 3);
			mdata.vb_uvs = gl_createVB(mdata.uvs, 2);				mdata.ib = gl_createIB(mdata.triangles);
			mesh.star5 = mdata;
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


		GL.bindFramebuffer( GL.FRAMEBUFFER, null );

		GL.viewport( 0, 0, GL.drawingBufferWidth, GL.drawingBufferHeight );
		GL.clear(GL.COLOR_BUFFER_BIT | GL.DEPTH_BUFFER_BIT);

		//vfield_render();
		//flow_render();
		timeline_update();

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
		onPressed( KEY_P, function() {
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

