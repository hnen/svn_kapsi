
var tex_vfield_initialparticledata;

var texrtdb_vfield_particledata;
var tex_vfield_perlin;
var texrt_vfield_particlevectors;
var tex_vfield_particle;

var vb_vfield_pcorners;
var vb_vfield_pindices;
var vb_vfield_puvs;
var ib_vfield_pindices;

var TEX_SIZE_X = 512;
var TEX_SIZE_Y = 512;
var PARTICLE_COUNT = TEX_SIZE_X*TEX_SIZE_Y;

var CHAIN_LENGTH = 8;
var CHAIN_SPACING = 16;

function vfield_init() {
	var noisedata = new Uint8Array(TEX_SIZE_X*TEX_SIZE_Y*4);
	for (var i = 0; i < TEX_SIZE_X*TEX_SIZE_Y*4; i += 1) {
		var c = Math.floor(Math.random() * 256);
		noisedata[i] = c;
	}
	gl_createTextureFromArrayRGBA("vfield_noise", noisedata, TEX_SIZE_X, TEX_SIZE_Y,
			function(tx) {
				tex_vfield_initialparticledata = tx;
			} );
	var perlin = perlin_create_fractal_uint8(1024, 3);
	gl_createTextureFromArrayA("vfield_perlin", perlin, 1024, 1024,
			function(tx) {
				tex_vfield_perlin = tx;
			} );
	vb_vfield_pcorners = vfield_createParticleCornerVB();
	vb_vfield_pindices = vfield_createParticleIndexVB();
	ib_vfield_pindices = vfield_createParticleCornerIB();
	vb_vfield_puvs = vfield_createParticleUvVB();

	gl_createTextureFromImage("blob", "data/blob.png", function(tex) {
		tex_vfield_particle = tex;
	});

	texrt_vfield_particlevectors = gl_createRenderTexture("particlevectors", 1024, 1024);

	texrtdb_vfield_particledata = {
		tex: [ gl_createRenderTexture("particledata0", TEX_SIZE_X, TEX_SIZE_Y),
			   gl_createRenderTexture("particledata1", TEX_SIZE_X, TEX_SIZE_Y) ],
		next_target: 0
	};

}

var vfield_runtime_inited = false;

function vfield_render(texrt_field) {

//	gl_setRenderTarget(texrt_vfield_particlevectors);
//	gl_clear([0,0,0,0]);
//	fb_renderFullscreen(shader_gradient, tex_vfield_perlin, true);
//	gl_resetRenderTarget();
//	gl_clear();
//	texrt_field = texrt_vfield_particlevectors;

	GL.disable(GL.CULL_FACE);
	GL.disable(GL.DEPTH_TEST);

	if (!vfield_runtime_inited) {
		vfield_runtime_init(texrt_field);
		vfield_runtime_inited = true;
	}

	//for (var i = 0; i < CHAIN_LENGTH; i++) {
	//	vfield_particles_move(texrt_field);
	//	cnt_frame+=1;
	//}

	vfield_particles_move(texrt_field);
	//fb_renderFullscreen(shader_blend, texrt_vfield_particlevectors);

	vfield_particles_render();
}


function vfield_runtime_init(texrt_field) {
	p_vfield_initParticleRTs();
	for (var i = 0; i < (CHAIN_LENGTH*CHAIN_SPACING-1+80)*4; i++) {
		vfield_particles_move(texrt_field);
		cnt_frame+=1;
	}
}

function p_vfield_initParticleRTs() {
		gl_setRenderTarget(texrtdb_vfield_particledata.tex[0]);
		gl_clear([0,0,0,0]);
		fb_renderFullscreen(shader_blend, tex_vfield_initialparticledata, true);
		gl_setRenderTarget(texrtdb_vfield_particledata.tex[1]);
		gl_clear([0,0,0,0]);
		fb_renderFullscreen(shader_blend, tex_vfield_initialparticledata, true);
		gl_resetRenderTarget();
}


function vfield_particles_move(texrt_field) {
	var tgt = texrtdb_vfield_particledata.next_target;
	gl_setRenderTarget(texrtdb_vfield_particledata.tex[tgt]);
	//gl_clear([0,0,0,0]);

	GL.useProgram(shader_moveprts);
	GL.activeTexture(GL.TEXTURE1);
	GL.bindTexture(GL.TEXTURE_2D, texrt_field.texture);
	GL.uniform1i(shader_moveprts.uniform_vfieldtex, 1);
	GL.uniform1i(shader_moveprts.uniform_frame, cnt_frame);
	GL.uniform1i(shader_moveprts.uniform_prtlifetime, 0);
	GL.uniform1i(shader_moveprts.uniform_chainlen, CHAIN_LENGTH);
	GL.uniform1f(shader_moveprts.uniform_texsize_x, TEX_SIZE_X);
	GL.uniform1f(shader_moveprts.uniform_texsize_y, TEX_SIZE_Y);

	fb_renderFullscreen(shader_moveprts, texrtdb_vfield_particledata.tex[1-tgt], true);
	gl_resetRenderTarget();
	texrtdb_vfield_particledata.next_target = 1 - tgt;
}

function vfield_particles_render() {
	p_vfield_particles_setupVS(false);

	GL.enable(GL.BLEND);
	GL.blendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA);
	assert(vb_vfield_pcorners.numItems == vb_vfield_pindices.numItems, "differrent number of items in vbs!");
	GL.drawElements(GL.TRIANGLES, ib_vfield_pindices.numItems, GL.UNSIGNED_INT,0);

	p_vfield_particles_setupVS(true);
}

function p_vfield_particles_setupVS(cleanup) {
	if (!cleanup) {
		GL.useProgram(shader_particle);
		var tgt = texrtdb_vfield_particledata.next_target;
		GL.activeTexture(GL.TEXTURE0);
		GL.bindTexture(GL.TEXTURE_2D, texrtdb_vfield_particledata.tex[tgt].texture);
		GL.activeTexture(GL.TEXTURE1);
		GL.bindTexture(GL.TEXTURE_2D, tex_vfield_particle.texture);

		GL.uniform1i(shader_particle.uniform_particledata_tex, 0);
		GL.uniform1i(shader_particle.uniform_particle_tex, 1);
		GL.uniform1f(shader_particle.uniform_aspect, 16.0/9.0);
		GL.uniform1f(shader_particle.uniform_tex_size_x, TEX_SIZE_X);
		GL.uniform1f(shader_particle.uniform_tex_size_y, TEX_SIZE_Y);

		GL.enableVertexAttribArray(shader_particle.attrib_id);
		GL.bindBuffer(GL.ARRAY_BUFFER, vb_vfield_pindices);
		GL.vertexAttribPointer(shader_particle.attrib_id, vb_vfield_pindices.itemSize, GL.FLOAT, false, 0, 0);

		GL.enableVertexAttribArray(shader_particle.attrib_uv);
		GL.bindBuffer(GL.ARRAY_BUFFER, vb_vfield_puvs);
		GL.vertexAttribPointer(shader_particle.attrib_uv, vb_vfield_puvs.itemSize, GL.FLOAT, false, 0, 0);

		GL.enableVertexAttribArray(shader_particle.attrib_corner);
		GL.bindBuffer(GL.ARRAY_BUFFER, vb_vfield_pcorners);
		GL.vertexAttribPointer(shader_particle.attrib_corner, vb_vfield_pcorners.itemSize, GL.FLOAT, false, 0, 0);

		GL.bindBuffer(GL.ELEMENT_ARRAY_BUFFER, ib_vfield_pindices);

	} else {
		GL.disableVertexAttribArray(shader_particle.attrib_corner);
		GL.disableVertexAttribArray(shader_particle.attrib_id);
		GL.bindBuffer(GL.ARRAY_BUFFER, null);
	}
}

function vfield_createParticleIndexVB() {
	var particle_index_array = new Float32Array(PARTICLE_COUNT*2);
	//var particle_index_array = new Float32Array(PARTICLE_COUNT*4);
	var n = 0;
	for (var i = 0; i < PARTICLE_COUNT; i+=1) {
		for (var j = 0; j < 2; j+=1) {
			particle_index_array[n] = i;
			n+=1;
		}
	}
	return gl_createVB(particle_index_array, 1);
}

function vfield_createParticleCornerIB() {
	var chain_count = Math.floor(PARTICLE_COUNT/CHAIN_LENGTH);
	var particle_corner_indices = new Uint32Array(chain_count * (CHAIN_LENGTH-1) * 2 * 3);
	var n = 0;
	var vid = 0;
	for (var i = 0; i < chain_count; i++) {
		for (var j = 0; j  < CHAIN_LENGTH-2; j++) {
			particle_corner_indices[n] = vid+0; n++;
			particle_corner_indices[n] = vid+2; n++;
			particle_corner_indices[n] = vid+1; n++;

			particle_corner_indices[n] = vid+1; n++;
			particle_corner_indices[n] = vid+3; n++;
			particle_corner_indices[n] = vid+2; n++;
			vid += 2;
		}
		vid += 4;
	}
	return gl_createIB(particle_corner_indices);
}

function vfield_createParticleCornerVB() {
	var particle_corner_array = new Float32Array(PARTICLE_COUNT*2*3);
	var j = 0;
	for (var i = 0; i < PARTICLE_COUNT; i+=1) {
		particle_corner_array[j] =-1.0; j+=1;
		particle_corner_array[j] = 0.0; j+=1;
		particle_corner_array[j] = 0.0; j+=1;

		particle_corner_array[j] = 1.0; j+=1;
		particle_corner_array[j] = 0.0; j+=1;
		particle_corner_array[j] = 0.0; j+=1;
	}
	return gl_createVB(particle_corner_array, 3);
}

function vfield_createParticleUvVB() {
	var particle_corner_array = new Float32Array(PARTICLE_COUNT*2*2);
	var j = 0;
	for (var i = 0; i < PARTICLE_COUNT; i+=1) {
		var stripe = i % CHAIN_LENGTH;
		var y = (stripe/(CHAIN_LENGTH-2));
		particle_corner_array[j] = 1.0; j+=1;
		particle_corner_array[j] = y; j+=1;

		particle_corner_array[j] = 0.0; j+=1;
		particle_corner_array[j] = y; j+=1;
	}
	return gl_createVB(particle_corner_array, 2);
}

