
layers = {};

actions = {
	anim: function( p ) {
		anims_render( p.id, p.speed ? p.speed : 10.0, p.blur ? p.blur : 5.0, p.alpha ? p.alpha : 1.0, p.additive ? p.additive : false, p.mul ? p.mul : 1.0 );
	},
	text: function( p ) {
		text_render( p.text ? p.text : "f20", p.x ? p.x : 0, p.y ? p.y : 0, p.size ? p.size: 0.5, p.alpha ? p.alpha : 1.0, p.n ? p.n : 0 );
	},
	copy: function( p ) {
		copy( p.from, p.to );
	},
	blur: function( p ) {
		blur( layers[p.layer], layers[p.layer], p.amount, p.mul ? p.mul : 1.0 );
	},
	pp: function( p ) {
		if ( p.edge ) {
			demo_edgeradius = p.edge;
		}
		if ( p.s_edge ) {
			demo_edgestrength = p.s_edge;
		}
		if ( p.s_bg ) {
			demo_bgstrength = p.s_bg;
		}
		if ( p.t_edge ) {
			demo_edgethreshold = p.t_edge;
		}
		if ( p.pow_bg ) {
			demo_bgpow = p.pow_bg;
		}
		if ( p.colorscale ) {
			demo_colorscale = p.colorscale;
		}
	},
	molecule: function( p ) {
		if ( p.count && p.ydir && p.rot && p.zoom )
		{
			molecule_render(p.count,p.ydir, p.ypos?p.ypos:0, p.rot, p.zoom, p.type);
		}
	},
	tonnijani: function( p ) {
		tonnijani_render( p.xpos, p.ypos, p.size, p.speed );
	}
};

timeline = [];

var timeline_ready = false;

function timeline_init() {

	var xmlhttp = new XMLHttpRequest();	
	xmlhttp.onreadystatechange = function () {
		//if ( xmlhttp.responseText ) {
		if (xmlhttp.readyState==4) {
			try {
				timeline = json_parse( xmlhttp.responseText );
				timeline_ready = true;
			} catch( err ) {
				alert( "problem parsing disain.json. parsed:\n" + timeline );
				throw err;
			}
		}
	};

	xmlhttp.open( "GET", "data/disain.json", true );
	//xmlhttp.channel.loadFlags |= Components.interfaces.nsIRequest.LOAD_BYPASS_CACHE;
	xmlhttp.send();
}


function p_timeline_evaluateValue( paramValue, current_time, start_time, end_time ) {
	if ( typeof paramValue === 'number' || typeof paramValue == 'string' || Array.isArray( paramValue ) ) {
		return paramValue;
	} else if ( typeof paramValue === 'object' ) {
		if ( paramValue.lerp ) {
			var lerp_param = paramValue.lerp;
			var p0 = p_timeline_evaluateValue(lerp_param[0], current_time,start_time,end_time);
			var p1 = p_timeline_evaluateValue(lerp_param[1], current_time,start_time,end_time);
			return lerp( p0, p1, current_time - start_time, end_time - start_time);
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
			return p0 + (p1-p0) * (Math.random() > p2 ? Math.random()*1.0 : Math.random()*0.2);
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

	if ( Array.isArray( ev.a ) ) {
		for ( var i in ev.a ) {
			p_timeline_doaction( ev.a[i], t, t0, t1, layer );
		}
	} else {
		if ( layer == null ) { 
			GL.bindFramebuffer( GL.FRAMEBUFFER, null );
			GL.viewport( 0, 0, GL.drawingBufferWidth, GL.drawingBufferHeight );
		} else {
			GL.bindFramebuffer( GL.FRAMEBUFFER, layer.framebuffer );
			GL.viewport( 0, 0, layer.width, layer.height );
		}
	
		var func = actions[ev.a];
		var params = timeline_gatherParams(t, t0, t1, ev.p );
		func(params);
	}
}

function createLayer(name) {
	var rtsize = gl_getCanvasSize(16,9);
	layers[name] = gl_createRenderTexture( rtsize[0], rtsize[1]); 
	var o = window.onresize;
	window.onresize = function() { o(); var nsize = gl_getCanvasSize(16,9); layers[name] = gl_resizeRenderTexture( layers[name], nsize[0], nsize[1]); };

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

