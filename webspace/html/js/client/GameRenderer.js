/* global gl_createShader */
/* global gl_createVBO */

"use strict";

var Game = require('../Game').Game;
var GameState = require('../Game').GameState;
var Ship = require('../Ship').Ship;
var Time = require('../Time').Time;
var gl;

function GameRenderer(_gl, game) {
	gl = _gl;
	this.shader_ship = null;
	this.shader_terrain = null;
	this.game = game;
	this.frame_count = 0;
	this.last_fps_counted = Time.time();
	this.render_gamedata = game.getGameState();
	this.vb_ship = null;
	this.vb_block = null;
	this.init();
}

GameRenderer.prototype.render = function() {
	this.frame_count++;
	if ( Time.time() - this.last_fps_counted >= 1000 ) {
		this.game.fps = 1000 * this.frame_count / (Time.time() - this.last_fps_counted);
		this.last_fps_counted = Time.time();
		this.frame_count = 0;
	}

	this.render_gamedata = GameState.interpolate(this.render_gamedata,
												 this.game.getGameState(),
												 0.15);

	for (var plrId in this.render_gamedata.ship_datas) {
		var ship = this.render_gamedata.ship_datas[plrId];
		this.drawShip(ship);
	}
//	this.drawTerrain(this.game.getTerrainData());
};

GameRenderer.prototype.init = function() {
	this.shader_ship = gl_createShader( "vs", "fs" );
	this.shader_ship.vertattrib_pos = gl.getAttribLocation(this.shader_ship, "in_Pos");
	this.shader_ship.uniform_dir = gl.getUniformLocation(this.shader_ship, "g_Dir");
	this.shader_ship.uniform_pos = gl.getUniformLocation(this.shader_ship, "g_Pos");
	this.shader_ship.uniform_aspect = gl.getUniformLocation(this.shader_ship, "g_Aspect");

	this.shader_terrain = gl_createShader("vs_block", "fs");
	this.shader_terrain.vertattrib_pos = gl.getAttribLocation(this.shader_terrain, "in_Pos");
	this.shader_terrain.uniform_pos = gl.getUniformLocation(this.shader_terrain, "g_pos");
	this.shader_terrain.uniform_aspect = gl.getUniformLocation(this.shader_terrain, "g_aspect");
	this.shader_terrain.uniform_scale = gl.getUniformLocation(this.shader_terrain, "g_scale");

	this.vb_ship = gl_createVBO( [
			0.0, 1.0, 0.0,
			0.85,-1.0, 0.0,
			-0.85,-1.0, 0.0,
	] );
	this.vb_block = gl_createVBO( [
			0.5, 0.5, 0.0,
			0.5,-0.5, 0.0,
		   -0.5, 0.5, 0.0,
		   -0.5,-0.5, 0.0,
	] );
};

GameRenderer.prototype.drawTerrain = function(terrain_data) {
	for (var y = 0; y <	terrain_data.height; y += 1) {
		for (var x = 0; x < terrain_data.width; x += 1) {
			var wx = (x-terrain_data.width*0.5)*0.02;
			var wy = (y-terrain_data.height*0.5)*0.02;
			if (terrain_data.data[y*terrain_data.width+x]) {
				this.drawBlock(wx, wy);
			}
		}
	}
}

GameRenderer.prototype.drawBlock = function(x, y) {
	gl.useProgram(this.shader_terrain);

	var aspect = gl.drawingBufferWidth / gl.drawingBufferHeight;
	gl.uniform2f(this.shader_terrain.uniform_pos, x, y);
	gl.uniform1f(this.shader_terrain.uniform_aspect, aspect );
	gl.uniform1f(this.shader_terrain.uniform_scale, 0.02 );
	gl.enableVertexAttribArray(this.shader_terrain.vertattrib_pos);
	gl.vertexAttribPointer(this.shader_terrain.vertattrib_pos, this.vb_ship.itemSize,
							gl.FLOAT, false, 0, 0 );

	gl.bindBuffer(gl.ARRAY_BUFFER, this.vb_block);
	gl.drawArrays(gl.TRIANGLE_STRIP, 0, this.vb_block.numItems);
	gl.disableVertexAttribArray(this.shader_terrain.vertattrib_pos);

}

GameRenderer.prototype.drawShip = function(ship_data) {

	var t = (Time.time() - this.game.lastUpdateTime) / Game.TICK_INTERVAL;

	gl.useProgram(this.shader_ship);

	var dir_x = Ship.dirVec(ship_data.dir).x;
	var dir_y = Ship.dirVec(ship_data.dir).y;
	var pos_x = ship_data.x;
	var pos_y = ship_data.y;

	var aspect = gl.drawingBufferWidth / gl.drawingBufferHeight;
	gl.uniform2f(this.shader_ship.uniform_dir, dir_x, dir_y);
	gl.uniform2f(this.shader_ship.uniform_pos, pos_x, pos_y );
	gl.uniform1f(this.shader_ship.uniform_aspect, aspect );
	gl.enableVertexAttribArray(this.shader_ship.vertattrib_pos);
	gl.vertexAttribPointer(this.shader_ship.vertattrib_pos, this.vb_ship.itemSize,
							gl.FLOAT, false, 0, 0 );

	gl.bindBuffer(gl.ARRAY_BUFFER, this.vb_ship);
	gl.drawArrays(gl.TRIANGLES, 0, this.vb_ship.numItems);

	gl.disableVertexAttribArray(this.shader_ship.vertattrib_pos);
};

exports.GameRenderer = GameRenderer;

