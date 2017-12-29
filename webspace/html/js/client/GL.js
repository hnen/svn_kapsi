/* jshint browser: true */
/* global alert: false */
/* exported gl_create */
/* exported gl_createShader */
/* exported gl_createVBO */
/* exported gl_createQuad */

"use strict";

var g_canvas;
var gl;

function gl_create(canvasId) {
	g_canvas = document.getElementById( canvasId );
	if ( g_canvas.getContext( "webgl" ) ) {
		return g_canvas.getContext( "webgl" );
	} else if ( g_canvas.getContext( "experimental-webgl" ) ) {
		return g_canvas.getContext( "experimental-webgl" );
	} else {
		alert( "Error initializing WebGL!" );
		return null;
	}
}

function gl_createShader( vs_id, fs_id ) {
	function getShader(shader_id) {
		var scriptElement = document.getElementById( shader_id );
		if ( !scriptElement ) {
			alert( "Unknown shader element: " + shader_id );
			return null;
		}

		var script = scriptElement.textContent;

		var shader;
		if ( scriptElement.type === "x-shader/x-fragment" ) {
			shader = gl.createShader( gl.FRAGMENT_SHADER );
		} else if ( scriptElement.type === "x-shader/x-vertex" ) {
			shader = gl.createShader( gl.VERTEX_SHADER );
		} else {
			alert( "Unknown shader type: " + script.type );
			return null;
		}


		gl.shaderSource( shader, script );
		gl.compileShader( shader );

		if ( !gl.getShaderParameter( shader, gl.COMPILE_STATUS ) ) {
			var shaderInfoLog = gl.getShaderInfoLog(shader);
			alert("Compile error for " + shader_id + ":\n" + shaderInfoLog);
			return null;
		}

		return shader;
	}

	var program = gl.createProgram();
	var vs = getShader( vs_id );
	var fs = getShader( fs_id );

	gl.attachShader( program, vs );
	gl.attachShader( program, fs );

	gl.linkProgram( program );

	if ( !gl.getProgramParameter( program, gl.LINK_STATUS ) ) {
		alert( "Error while linking shaders " + vs_id + " and " + fs_id );
		return null;
	}

	return program;

}

function gl_createVBO(vertices) {
	var ret = gl.createBuffer();
	gl.bindBuffer( gl.ARRAY_BUFFER, ret );
	gl.bufferData( gl.ARRAY_BUFFER, new Float32Array( vertices ), gl.STATIC_DRAW );
	ret.itemSize = 3;
	ret.numItems = vertices.length / 3;
	return ret;
}

function gl_createStrip( vertices ) {
	var ret = gl.createBuffer();
	gl.bindBuffer( gl.ARRAY_BUFFER, ret );
	gl.bufferData( gl.ARRAY_BUFFER, new Float32Array( vertices ), gl.STATIC_DRAW );
	ret.itemSize = 3;
	ret.numItems = 4;
	return ret;
}

function gl_createQuad() {
	return gl_createStrip( [
			1.0, 1.0, 0.0,
			-1.0, 1.0, 0.0,
			1.0,-1.0, 0.0,
			-1.0,-1.0, 0.0,
	] );
}

