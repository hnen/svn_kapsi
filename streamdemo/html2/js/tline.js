
layers = {};

var cam_pos = [0,0,0];
var cam_view = [0,0,1];
var cam_up = [0,1,0];
var cam_fov = 55;

function mat_cam() {
	return mat_view(cam_pos, cam_view, cam_up);
}


actions = {
	cam: function(p) {
		cam_pos = p.pos;
		cam_view = p.dst;
		if (p.fov) {
			cam_fov = p.fov;
		} else {
			cam_fov = 55;
		}
		if (p.up) {
			cam_up = p.up;
		} else {
			cam_up = [0,1,0];
		}
	},
	text: function(p) {
		teksti_render(p.pos, p.normal, p.up, p.N, p.t, p.range, p.scale, p.size, p.seed, p.straight);
	},
	okkult: function(p) {
		okkult_render(p.y, p.rot, p.death);
	},
	mesh: function( p ) {
		var mat = mat_translate(p.pos);
		if (p.scale) {
			mat = mat_mul(mat_scale(p.scale),mat);
		}
		if (p.rotZ) {
			mat = mat_mul(mat_rotZ(p.rotZ / 180.0 * Math.PI), mat);
		}
		if (p.rotY) {
			mat = mat_mul(mat_rotY(p.rotY / 180.0 * Math.PI), mat);
		}
		if (p.rotX) {
			mat = mat_mul(mat_rotX(p.rotX / 180.0 * Math.PI), mat);
		}
		mat = mat_mul(mat, mat_view(cam_pos, cam_view, cam_up));
		var shader = shader_mesh;
		if (p.shader) {
			shader = shaders[p.shader];
		}
		var params = null;
		if (p.params) {
			params = p.params;
		}
		if (p.tex) {
			renderMesh(shader, mesh[p.mesh], mat, gl_textures[p.tex], params);
		} else {
			renderMesh(shader, mesh[p.mesh], mat, null, params);
		}
	},
	seppo: function( p ) {
		var mat = mat_translate(p.pos);
		if (p.scale) {
			mat = mat_mul(mat_scale(p.scale),mat);
		}
		if (p.rotZ) {
			mat = mat_mul(mat_rotZ(p.rotZ / 180.0 * Math.PI), mat);
		}
		if (p.rotY) {
			mat = mat_mul(mat_rotY(p.rotY / 180.0 * Math.PI), mat);
		}
		if (p.rotX) {
			mat = mat_mul(mat_rotX(p.rotX / 180.0 * Math.PI), mat);
		}
		mat = mat_mul(mat, mat_view(cam_pos, cam_view, cam_up));
		shader = shaders.seppo;
		renderMesh(shader, mesh[p.mesh], mat, null, {uniform_t: t()} );
	},
	render_perlinbg: function( p ) {
		GL.useProgram(shader_blend_perlin);
		GL.activeTexture(GL.TEXTURE1);
		GL.bindTexture(GL.TEXTURE_2D, tex_vfield_perlin2.texture);
		GL.uniform1i(shader_blend_perlin.uniform_tex2, 1);
		GL.uniform1f(shader_blend_perlin.uniform_t, t());
		fb_renderFullscreen(shader_blend_perlin, tex_vfield_perlin, true);
	},
	render_layer: function( p ) {
		GL.disable(GL.CULL_FACE);
		GL.disable(GL.DEPTH_TEST);
		fb_renderFullscreen(shader_blend, getlayer(p.src), false);
	},
	render_tex: function( p ) {
		GL.disable(GL.CULL_FACE);
		GL.disable(GL.DEPTH_TEST);
		fb_renderFullscreen(shader_tex, gl_textures[p.tex], false);
	},
	clear: function( p ) {
		gl_clear(p.col);
	},
	gradient: function( p ) {
		GL.disable(GL.CULL_FACE);
		GL.disable(GL.DEPTH_TEST);
		flow_computeGradient(getlayer(p.dst), getlayer(p.src));
	},
	blur_vfield: function( p ) {
		GL.disable(GL.CULL_FACE);
		GL.disable(GL.DEPTH_TEST);
		fb_blur(shader_blur_vfield, getlayer(p.dst), getlayer(p.src), getlayer("intermediate_blur"), p.amount);
	},
	blur: function( p ) {
		GL.disable(GL.CULL_FACE);
		GL.disable(GL.DEPTH_TEST);
		fb_blur(shader_blur, getlayer(p.dst), getlayer(p.src), getlayer("intermediate_blur"), p.amount);
	},
	particles: function( p ) {
		GL.disable(GL.CULL_FACE);
		GL.disable(GL.DEPTH_TEST);
		vfield_render(getlayer(p.dirs), getlayer(p.cols), p.spawn_only_visible, p.colorize_particles, p.mul_len ? p.mul_len : 1.0, p.mul_brig ? p.mul_brig : 1.0, p.paska ? p.paska : 0.0);
	},
	gasket: function( p )
	{
		var mat = mat_translate(p.pos);
		if (p.scale) {
			mat = mat_mul(mat_scale(p.scale),mat);
		}
		if (p.rotZ) {
			mat = mat_mul(mat_rotZ(p.rotZ / 180.0 * Math.PI), mat);
		}
		if (p.rotY) {
			mat = mat_mul(mat_rotY(p.rotY / 180.0 * Math.PI), mat);
		}
		if (p.rotX) {
			mat = mat_mul(mat_rotX(p.rotX / 180.0 * Math.PI), mat);
		}
		var m = mesh.circleOne;
		if ( p.mesh )
			m = mesh[p.mesh];
//		mat = mat_mul(mat, mat_view(cam_pos, cam_view, cam_up));
		shader = shaders.seppo;
		var c1 = [ 0,0,0,10 ];
		var r2 = c1[3]*2;
		var c2 = [ r2*Math.cos(2*Math.PI/6),0,r2*Math.sin(2*Math.PI/6),10 ];
		var c3 = [ -r2*Math.cos(2*Math.PI/6),0,r2*Math.sin(2*Math.PI/6),10 ];

		var c_ = c1;
	/*	var nx = (c1[0]+c2[0]+c3[0])/3;
		var nz = (c1[2]+c2[2]+c3[2])/3;
		var nr = nz+c1[3];
		var c_ = [ nx,0,nz, nr ];	*/

		// center:
		var nx = (c_[0]+c2[0]+c3[0])/3;
		var nz = (c_[2]+c2[2]+c3[2])/3;
		c2[0] -= nx;	c3[0] -= nx;	c_[0] -= nx;
		c2[2] -= nz;	c3[2] -= nz;	c_[2] -= nz;

		/*gasket_drawCircle(c_,mat,m);
		gasket_drawCircle(c2,mat,m);
		gasket_drawCircle(c3,mat,m); */

		var num = 5;
		if ( p.num )
			num = p.num;
		gasket_circles( c_,c2,c3, Math.floor(num), mat,m);
	}
};



timeline = [];

var timeline_ready = false;

function timeline_init() {

	var xmlhttp = new XMLHttpRequest();
	xmlhttp.addEventListener("load", function () {
		try {
			timeline = json_parse( xmlhttp.responseText );
			timeline_ready = true;
			file_loadedResources += 1;
		} catch( err ) {
			alert("problem parsing disain.json: " + err.message);
			throw err;
		}
	} );
	file_totalResources += 1;

	xmlhttp.open( "GET", "data/disain.json", true );
	//xmlhttp.channel.loadFlags |= Components.interfaces.nsIRequest.LOAD_BYPASS_CACHE;
	xmlhttp.send();
}


function p_timeline_evaluateValue( paramValue, current_time, start_time, end_time ) {
	if ( typeof paramValue === 'number' || typeof paramValue == 'string' ) {
		return paramValue;
	} else if ( Array.isArray( paramValue ) ) {
		var ret = [];
		for (var i in paramValue) {
			ret.push(p_timeline_evaluateValue(paramValue[i], current_time, start_time, end_time));
		}
		return ret;
	} else if ( typeof paramValue === 'object' ) {
		if ( paramValue.lerp ) {
			var lerp_param = paramValue.lerp;
			if (lerp_param.length == 2) {
				var p0 = p_timeline_evaluateValue(lerp_param[0], current_time,start_time,end_time);
				var p1 = p_timeline_evaluateValue(lerp_param[1], current_time,start_time,end_time);
				return lerp( p0, p1, current_time - start_time, end_time - start_time);
			} else {
				var p0 = p_timeline_evaluateValue(lerp_param[0], current_time,start_time,end_time);
				var p1 = p_timeline_evaluateValue(lerp_param[1], current_time,start_time,end_time);
				var t0 = p_timeline_evaluateValue(lerp_param[2], current_time,start_time,end_time) + start_time;
				var t1 = p_timeline_evaluateValue(lerp_param[3], current_time,start_time,end_time) + start_time;
				return lerpc( p0, p1, current_time - t0, t1 - t0);
			}
		} else if ( paramValue.mul ) {
			var lerp_param = paramValue.mul;
			var p0 = p_timeline_evaluateValue(lerp_param[0], current_time,start_time,end_time);
			var p1 = p_timeline_evaluateValue(lerp_param[1], current_time,start_time,end_time);
			return p0*p1;
		} else if ( paramValue.cberp ) {
			var lerp_param = paramValue.cberp;
			if (lerp_param.length == 2) {
				var p0 = p_timeline_evaluateValue(lerp_param[0], current_time,start_time,end_time);
				var p1 = p_timeline_evaluateValue(lerp_param[1], current_time,start_time,end_time);
				return cberp( p0, p1, (current_time - start_time)/(end_time - start_time));
			} else {
				var p0 = p_timeline_evaluateValue(lerp_param[0], current_time,start_time,end_time);
				var p1 = p_timeline_evaluateValue(lerp_param[1], current_time,start_time,end_time);
				var t0 = p_timeline_evaluateValue(lerp_param[2], current_time,start_time,end_time) + start_time;
				var t1 = p_timeline_evaluateValue(lerp_param[3], current_time,start_time,end_time) + start_time;
				return cberp( p0, p1, (current_time - t0)/(t1-t0));
			}
		} else if ( paramValue.spectrum )
		{
			// values: start, end, (multiplier)
			var spec_param = paramValue.spectrum;
			var val = 0;
			for ( var i=spec_param[0]; i<spec_param[1]; i++ )
				val = val + getSpectrum(i);
			val /= Math.max(1,spec_param[1]-spec_param[0]);
			if ( spec_param.length==3 )
				val *= spec_param[2];
			return val;
		} else if ( paramValue.saw ) {
			var saw_param = paramValue.saw;
			var p0 = p_timeline_evaluateValue(saw_param[0], current_time,start_time,end_time);
			var p1 = p_timeline_evaluateValue(saw_param[1], current_time,start_time,end_time);
			var p2 = p_timeline_evaluateValue(saw_param[2], current_time,start_time,end_time);
			return lerp(p0, p1, mod(current_time - start_time, p2), p2 );
		} else if ( paramValue.random ) {
			var random_param = paramValue.random;
			var p0 =p_timeline_evaluateValue(random_param[0], current_time,start_time,end_time);
			var p1 =p_timeline_evaluateValue(random_param[1], current_time,start_time,end_time);
			return Math.random() * (p1-p0) + p0;
		} else if ( paramValue.jerk ) {
			var prm = paramValue.jerk;
			var p0 = p_timeline_evaluateValue(prm[0], current_time,start_time,end_time);
			var p1 = p_timeline_evaluateValue(prm[1], current_time,start_time,end_time);
			var p2 = p_timeline_evaluateValue(prm[2], current_time,start_time,end_time);
			return p0 + (p1-p0) * (noise(t()) > p2 ? noise(t()+232)*1.0 : noise(t()+988)*0.2);
		}
	}
}

function timeline_gatherParams( current_time, start_time, end_time, oparams ) {
	var params = {};
	for ( var paramName in oparams ) {
		params[paramName] = p_timeline_evaluateValue( oparams[paramName], current_time, start_time, end_time );
	}
	return params;
}

function getlayer(name) {
	if ( !layers[name] ) {
		createLayer( name );
	}
	return layers[name];
}

function p_timeline_doaction( ev, t, t0, t1, layer ) {
	if ( ev.layer ) {
		layer = getlayer(ev.layer);
	}

	if (ev.t) {
		if (t < ev.t[0] || t > ev.t[1]) {
			return;
		}
	}
	if (ev.t_rel) {
		if (t-t0 < ev.t_rel[0] || t-t0 > ev.t_rel[1]) {
			return;
		}
	}

	if ( Array.isArray( ev.a ) ) {
		for ( var i in ev.a ) {
			p_timeline_doaction( ev.a[i], t, t0, t1, layer );
		}
	} else {
		if ( layer == null ) {
			gl_resetRenderTarget();
		} else {
			gl_setRenderTarget(layer);
		}

		var func = actions[ev.a];
		var params = timeline_gatherParams(t, t0, t1, ev.p );
		func(params);
	}
}

function createLayer(name) {
	var rtsize = gl_getCanvasSize(16,9);
//	layers[name] = gl_createRenderTexture("layer_"+name, rtsize[0], rtsize[1]);
	layers[name] = gl_createRenderTexture("layer_"+name,512, 512);
//	layers[name] = gl_createRenderTexture("layer_"+name,128,128);
	var o = window.onresize;
	window.addEventListener("resize", function() { o();
		var nsize = gl_getCanvasSize(16,9);
//		if ( name == "scene" )
			layers[name] = gl_resizeRenderTexture( layers[name], nsize[0], nsize[1]);
	} );

}

var prev_t = 0;
function timeline_update() {
	var t = p_musa_audio_el.currentTime;

	for ( var ln in layers ) {
		var layer = layers[ln];
		GL.bindFramebuffer( GL.FRAMEBUFFER, layer.framebuffer );
		GL.viewport( 0, 0, layer.width, layer.height );
		GL.clearColor( 0,0,0,0 );
		GL.clear( GL.COLOR_BUFFER_BIT | GL.DEPTH_BUFFER_BIT );
	}

	for ( var i in timeline ) {
		var ev = timeline[i];

		var layer = null;

		if ( ev.t ) {
			if ( t >= ev.t[0] && t < ev.t[1] ) {
				p_timeline_doaction( ev, t, ev.t[0], ev.t[1], layer );
			}
		} else if ( ev.t_repeat ) {
			if ( t >= ev.t_repeat[0] && t < ev.t_repeat[1] ) {
				var tmod = ev.t_repeat[2];
				var ton = ev.t_repeat[3];
				var t0 = mod(t - ev.t_repeat[0],tmod);
				if ( t0 < ton ) {
					p_timeline_doaction( ev, t0, 0, ton, layer );
				}
			}
		}
	}

	prev_t = t;
}

