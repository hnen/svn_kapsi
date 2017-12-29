;(function e(t,n,r){function s(o,u){if(!n[o]){if(!t[o]){var a=typeof require=="function"&&require;if(!u&&a)return a(o,!0);if(i)return i(o,!0);throw new Error("Cannot find module '"+o+"'")}var f=n[o]={exports:{}};t[o][0].call(f.exports,function(e){var n=t[o][1][e];return s(n?n:e)},f,f.exports,e,t,n,r)}return n[o].exports}var i=typeof require=="function"&&require;for(var o=0;o<r.length;o++)s(r[o]);return s})({1:[function(require,module,exports){
"use strict";

var Ship = require( './Ship' ).Ship;
var ShipState = require( './Ship' ).ShipState;
var Time = require( './Time' ).Time;
var Utils = require( './Utils' ).Utils;
var Terrain = require( './Terrain' ).Terrain;

function GameState() {
	this.id = "";
	this.display_name = "";
	this.player_ids = [];
	this.ship_datas = {};
	this.terrain_seed = 0;
}


GameState.clone = function(game_data) {
	var clone = new GameState();
	clone.id = game_data.id;
	clone.display_name = game_data.display_name;
	clone.player_ids = game_data.player_ids.slice();
	clone.terrain_seed = game_data.terrain_seed;
	for( var ship in game_data.ship_datas ) {
		clone.ship_datas[ship] = ShipState.clone( game_data.ship_datas[ship] );
	}
	return clone;
};

GameState.interpolate = function(gamedata_a, gamedata_b, t) {
	var ret = GameState.clone(gamedata_b);
	for (var ship_i in ret.ship_datas) {
		if (gamedata_a.ship_datas.hasOwnProperty(ship_i)) {
			ret.ship_datas[ship_i] = ShipState.interpolate(gamedata_a.ship_datas[ship_i],
														  gamedata_b.ship_datas[ship_i],
														  t);
		}
	}
	return ret;
}

function Game(game_id) {
	this.gamestate = new GameState();
	this.gamestate.id = game_id;
	this.players = [];
	this.terrain_data = null;
	this.T = 0;
}

Game.createNew = function(game_id) {
	var game = new Game(game_id);
	game.gamestate.terrain_seed = Math.floor(Math.random() * 10000000);
	return Game.create(game.gamestate);
}

Game.create = function(game_state) {
	var game = new Game(game_state.id);
	game.gamestate = GameState.clone(game_state);
	game.terrain_data = {
		data: Terrain.generateTerrain(256,256,game.gamestate.terrain_seed, 20),
		width: 256,
		height: 256
	};
	return game;
}

Game.prototype.getTerrainData = function() {
	return this.terrain_data;
}

Game.GRAVITY = 9.81;
Game.TICK_INTERVAL = 1000/20;

Game.ADD_LATENCY = true;
Game.ADD_LATENCY_MIN = 100;
Game.ADD_LATENCY_MAX = 200;
Game.LATENCY_FLUC_INTERVAL = 6000;
Game.LATENCY_NOISE = 5;

Game.ComputeSimulatedLatency = function(t) {
	var ft = (t % Game.LATENCY_FLUC_INTERVAL);
	var latencyBaseRnd = ((ft*(ft+715517)) % 715517) / 715517;
	var latencyBase = Utils.lerp(Game.ADD_LATENCY_MIN, Game.ADD_LATENCY_MAX, latencyBaseRnd);
	var latencyNoiseRnd = ((t*(t+715517)) % 715517) / 715517;
	var latencyNoise = (latencyNoiseRnd * Game.LATENCY_NOISE - Game.LATENCY_NOISE * 0.5) * 2.0;
	return Math.floor(latencyBase + latencyNoise);
}

Game.prototype.getGameId = function() {
	return this.gamestate.id;
}

Game.prototype.getGameState = function() {
	return GameState.clone(this.gamestate);
}

Game.prototype.join = function(plr) {
	this.gamestate.player_ids.push( plr.data.id );
	this.players.push( plr );
	plr.data.current_game_id = this.gamestate.id;
	return true;
};

Game.prototype.part = function(plrId) {
	if ( this.gamestate.ship_datas[plrId] ) {
		delete this.gamestate.ship_datas[plrId];
	}

	var i = this.gamestate.player_ids.indexOf( plrId );
	this.gamestate.player_ids.splice( i, 1 );
};

Game.prototype.spawn = function(plrId) {
	var ship_data = new ShipState();
	ship_data.id_player = plrId;
	this.spawnShip( ship_data );
	return ship_data;
};

Game.prototype.spawnShip = function(ship_data) {
	this.gamestate.ship_datas[ship_data.id_player] = ship_data;
};

Game.prototype.setState = function(game_data) {
	this.gamestate = GameState.clone( game_data );
};

Game.prototype.getGameTick = function() {
	return this.T;
}
Game.prototype.setGameTick = function(T) {
	this.T = T;
}

Game.prototype.update = function() {
	this.T++;

	if ( !this.gamestate ) {
		return;
	}

	for (var i in this.gamestate.ship_datas) {
		var ship_data = this.gamestate.ship_datas[i];
		Ship.update(ship_data);
	}
};

Game.prototype.player_exists = function(plr_id) {
	return this.gamestate.ship_datas.hasOwnProperty(plr_id);
};

Game.prototype.performAction = function(plrId, action) {
	action.performed = true;

	if ( action.action_type === "spawn" ) {
		this.spawnShip( action.ship_data );
	} else if (action.action_type === "part" ) {
		this.part( action.id_plr );
	} else if ( this.gamestate.ship_datas.hasOwnProperty(plrId) ) {
		var ship_data = this.gamestate.ship_datas[plrId];
		Ship.doAction(ship_data, action);
	}
};


exports.Game = Game;
exports.GameState = GameState;

},{"./Ship":3,"./Terrain":4,"./Time":5,"./Utils":6}],2:[function(require,module,exports){
"use strict";

function PlayerData() {
	this.id = "";
	this.display_name = "";
	this.current_game_id = "";
}

function Player() {
	this.data = new PlayerData();
	this.socket = null;
}


exports.Player = Player;


},{}],3:[function(require,module,exports){
"use strict";

function ShipState() {
	this.x = 0.0;
	this.y = 0.0;
	this.dir = 0.0;

	this.v_x = 0.0;
	this.v_y = 0.0;
	this.a_x = 0.0;
	this.a_y = 0.0;

	this.accelerating = 0.0;
	this.turning = 0.0;

	this.id_player = "";
}

ShipState.clone = function(shipstate) {
	var clone = new ShipState();
	clone.x = shipstate.x;
	clone.y = shipstate.y;
	clone.dir = shipstate.dir;

	clone.v_x = shipstate.v_x;
	clone.v_y = shipstate.v_y;
	clone.a_x = shipstate.a_x;
	clone.a_y = shipstate.a_y;

	clone.accelerating = shipstate.accelerating;
	clone.turning = shipstate.turning;

	clone.id_player = shipstate.id_player;
	return clone;
};

ShipState.interpolate = function(shipstate_a, shipstate_b, t) {
	var ret = ShipState.clone(shipstate_b);
	ret.x = shipstate_b.x * t + shipstate_a.x * (1-t);
	ret.y = shipstate_b.y * t + shipstate_a.y * (1-t);
	ret.dir = shipstate_b.dir * t + shipstate_a.dir * (1-t);
	ret.v_x = shipstate_b.v_x * t + shipstate_a.v_x * (1-t);
	ret.v_y = shipstate_b.v_y * t + shipstate_a.v_y * (1-t);
	ret.a_x = shipstate_b.a_x * t + shipstate_a.a_x * (1-t);
	ret.a_y = shipstate_b.a_y * t + shipstate_a.a_y * (1-t);
	ret.accelerating = shipstate_b.accelerating * t + shipstate_a.accelerating * (1-t);
	ret.turning = shipstate_b.turning * t + shipstate_a.turning * (1-t);
	return ret;
}


function Ship(game, player_id) {
	this.game = game;
	this.player_id = player_id;
}

Ship.TURN_SPEED = 0.3;
Ship.ACCELERATION = 0.01;

Ship.update = function(data) {
	data.dir += data.turning * Ship.TURN_SPEED;

	this._updateMovement(data);
	this._clampPosition(data);

};

Ship._updateMovement = function(data) {
	data.a_x = 0.0;
	data.a_y = 0.0;

	var dirv = Ship.dirVec( data.dir );
	data.a_x += dirv.x * Ship.ACCELERATION * data.accelerating;
	data.a_y += dirv.y * Ship.ACCELERATION * data.accelerating;

	data.v_x += data.a_x;
	data.v_y += data.a_y;

	data.x += data.v_x;
	data.y += data.v_y;
};

Ship._clampPosition = function(data) {
	if ( data.x < -4 ) {
		data.x = -4;
		if ( data.v_x < 0 ) {
			data.v_x = 0;
		}
	}
	if ( data.x > 4 ) {
		data.x = 4;
		if ( data.v_x > 0 ) {
			data.v_x = 0;
		}
	}
	if ( data.y < -2 ) {
		data.y = -2;
		if ( data.v_y < 0 ) {
			data.v_y = 0;
		}
	}
	if ( data.y > 2 ) {
		data.y = 2;
		if ( data.v_y > 0 ) {
			data.v_y = 0;
		}
	}
};

Ship.doAction = function(data, action) {
	if ( action.action_type === "accelerate_start" ) {
		data.accelerating = 1.0;
	}
	else if ( action.action_type === "accelerate_stop" ) {
		data.accelerating = 0.0;
	}
	else if ( action.action_type === "turn_left" ) {
		data.turning = -1.0;
	}
	else if ( action.action_type === "turn_right" ) {
		data.turning = 1.0;
	}
	else if ( action.action_type === "turn_stop" ) {
		data.turning = 0.0;
	}
}

Ship.dirVec = function(d) {
	return { x: Math.sin( d ), y: Math.cos( d ) };
};

exports.Ship = Ship;
exports.ShipState = ShipState;


},{}],4:[function(require,module,exports){
"use strict";

var Utils = require('./Utils').Utils;

function Terrain() {
	this.terrain = null;
	this.terrainWidth = 0;
	this.terrainHeight = 0;
	this.blockWidth = 0.1;
	this.blockHeight = 0.1;
}

Terrain.generateTerrain = function(width, height, seed, iterationCount) {
	var data = new Uint8Array(width*height);

	Utils.seed(34234234);
	for (var i = 0; i < width*height; i += 1) {
		data[i] = (Utils.rnd() > 0.5 ? 1 : 0);
	}
	console.dir(data);

	var N = iterationCount;
	while (N > 0) {
		data = Terrain._iterateLevelGenAutomaton(data, width, height);
		N -= 1;
	}

	return data;
}

Terrain._iterateLevelGenAutomaton = function(data, width, height) {
	var ndata = new Uint8Array(width*height);
	var i = 0;
	for (var y = 0; y < height; y += 1) {
		for (var x = 0; x < width; x += 1) {
			var n0 = (x == 0 || y == 0) || data[i-width-1];
			var n1 = (y == 0) || data[i-width];
			var n2 = (x == width-1 || y == 0) || data[i-width+1];
			var n3 = (x == 0) || data[i-1];
			var n4 = (x == width-1) || data[i+1];
			var n5 = (y == height-1 || x == 0) || data[i+width-1];
			var n6 = (y == height-1) || data[i+width];
			var n7 = (y == height-1 || x == width-1) || data[i+width+1];

			var l = 0;
			if (n0) l++;
			if (n1) l++;
			if (n2) l++;
			if (n3) l++;
			if (n4) l++;
			if (n5) l++;
			if (n6) l++;
			if (n7) l++;

			var rule;
			if (data[i] != 0) {
				rule = 34568;
			}  else {
				rule = 678;
			}

			ndata[i] = 0;
			while (rule != 0) {
				var s = rule%10;
				if (s == l) {
					ndata[i] = 1;
					break;
				}
				rule=Math.floor(rule/10);
			}
			i += 1;
		}
	}
	return ndata;
}

exports.Terrain = Terrain;



},{"./Utils":6}],5:[function(require,module,exports){
"use strict";

var Time = function() {
};

Time.time = function() {
	return + new Date();
};

exports.Time = Time;


},{}],6:[function(require,module,exports){
"use strict";

function Utils() {}

Utils.rndString = function( length ) {
	var chars = "0123456789abcdefghijklmnopqrstuvwxyz";
	var ret = "";
	for ( var i = length; i > 0; i-- ) {
		ret = ret + chars[ Math.round( Math.random() * (chars.length-1) ) ];
	}
	return ret;
};

Utils.m_w = 123456789;
Utils.m_z = 987654321;
Utils.mask = 0xffffffff;

Utils.seed = function(i) {
	if(i==0)i=1;
	Utils.m_w = i & Utils.mask;
	Utils.m_z = 34242342;
}

Utils.rnd = function() {
	Utils.m_z = (36969 * (Utils.m_z & 65535) + (Utils.m_z >> 16)) & Utils.mask;
	Utils.m_w = (18000 * (Utils.m_w & 65535) + (Utils.m_w >> 16)) & Utils.mask;
	var result = ((Utils.m_z << 16) + Utils.m_w) & Utils.mask;
	result /= 4294967296;
	return result + 0.5;
}

Utils.lerp = function(a,b,t) {
	return a + (b-a)*t;
}

exports.Utils = Utils;


},{}],7:[function(require,module,exports){
/* jshint browser: true */
/* global input_isKeyDown */
/* global input_wasKeyPressed */
/* global input_wasKeyReleased */
/* global p_input_frameChanged */
/* global KEY_UP */
/* global KEY_LEFT */
/* global KEY_RIGHT */

"use strict";

var Game = require('../Game').Game;
var Timeline = require('./Timeline').Timeline;
var Utils = require('../Utils').Utils;
var Time = require('../Time').Time;

GameClient.SEEK_THRESHOLD = 3;

var el;
function setStatusText(txt) {
	if (!el) {
		el = document.getElementById( "statustext" );
	}
	el.innerText = txt;
}

function GameClient(client, state, player_local) {
	this.game = Game.create(state);
	this.client = client;
	this.timeline = new Timeline(this.game);
	this.sent_spawn_request = false;

	console.log('Initial gamestate:');
	console.log(JSON.stringify(this.game.gamestate));

	this.player_local = player_local;

}

GameClient.prototype.initialize = function(T, ping) {
	this.game.T = T;
	if (this.player_local && this.player_local.socket) {
		this.initListeners(this.player_local.socket);
	}
}

GameClient.prototype.initListeners = function(socket) {
	var self = this;
	socket.on( self.game.getGameId() + "_action", function(actiondata) {
		var plrId = actiondata.id_plr;
		var action = actiondata;
		self.performAction(plrId, action);
	} );
};

GameClient.prototype._seekGameStateToPredictedServerTime = function(time, ping) {
	var dest_t = Math.round(time + ping/Game.TICK_INTERVAL);
	var timeDifference = Math.abs(dest_t - this.game.getGameTick());
	if (timeDifference > GameClient.SEEK_THRESHOLD) {
		this.timeline.seek(dest_t);
	}
};

GameClient.prototype.serverTimeReceived = function(time, ping) {
	if (!this.sent_spawn_request) {
		this.sent_spawn_request = true;
		this.sendSpawn();
	}
	this.ping = ping;
	this._seekGameStateToPredictedServerTime(time, ping);
};

GameClient.prototype.startUpdate = function() {
	var self = this;
	this.next_tick = Time.time() + Game.TICK_INTERVAL;
	setTimeout( function() { self.runUpdate(self); }, 0 );
}

GameClient.prototype.runUpdate = function(self) {
	var T = Time.time();
	this.lastUpdateTime = T;
	var dt = T - this.next_tick;

	for( ; dt >= 0; dt -= Game.TICK_INTERVAL ) {
		self.update();
		self.next_tick += Game.TICK_INTERVAL;
	}

	var dt0 = self.next_tick - Time.time();

	setTimeout(function() { self.runUpdate(self); }, dt0);
};

GameClient.prototype.performActionLocal = function(action) {
	console.log('perform ' + action.action_type + ', t: ' + this.game.getGameTick());
	action.id = this.player_local.data.id + "_" + Utils.rndString(8);
	this.send('action', action);
	this.performAction(this.player_local.data.id, action);
};

GameClient.prototype.performAction = function(id_plr, action) {
	if (!action.id) {
		action.id = id_plr + "_" + Utils.rndString(8);
	}
	if (!action.hasOwnProperty('T')) {
		action.T = this.game.T;
		action.T_orig = action.T;
	}
	action.id_plr = id_plr;
	this.timeline.addAction(action);
};

GameClient.prototype.updatePlayerControl = function( player ) {
	if (input_wasKeyPressed(KEY_UP)) {
		this.performActionLocal( { action_type: "accelerate_start" } );
	}
	if (input_wasKeyReleased(KEY_UP)) {
		this.performActionLocal( { action_type: "accelerate_stop" } );
	}

	if ( input_wasKeyPressed( KEY_LEFT ) ) {
		this.performActionLocal( { action_type: "turn_left" } );
	}
	if ( input_wasKeyPressed( KEY_RIGHT ) ) {
		this.performActionLocal( { action_type: "turn_right" } );
	}
	if ( (input_wasKeyReleased( KEY_LEFT ) || input_wasKeyReleased( KEY_RIGHT )) ) {
		this.performActionLocal( { action_type: "turn_stop" } );
	}
};

GameClient.prototype.send = function( msg, params ) {
	this.player_local.socket.emit( this.game.getGameId() + "_" + msg, params );
};

GameClient.prototype.sendSpawn = function() {
	this.send( 'requestspawn' );
};

GameClient.prototype.update = function() {
	if (this.client) {
		setStatusText("" + this.client.status_client + "\n" +
					"ping " + this.client.ping + "ms\n" +
					"T: " + this.game.getGameTick() + "\n" +
					"time error: " + this.timeline.dt + " frames\n" +
					"FPS: " + (Math.round(this.fps*100)/100) + "\n" +
					(Game.ADD_LATENCY ? "ADDED LATENCY: "+Game.ComputeSimulatedLatency(Time.time())+"ms.\n" : "")
		);
	}
	if (this.player_local) {
		this.updatePlayerControl( this.player_local );
	}
	this.timeline.update();
	p_input_frameChanged();
};

GameClient.prototype.part = function( plrId ) {
	game.part(plrId);
};


exports.GameClient = GameClient;



},{"../Game":1,"../Time":5,"../Utils":6,"./Timeline":9}],8:[function(require,module,exports){
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


},{"../Game":1,"../Ship":3,"../Time":5}],9:[function(require,module,exports){

"use strict";

var Game = require('../Game').Game;
var GameState = require('../Game').GameState;

function Timeline(game) {
	this.game = game;

	this.actions = [];
	this.action_id_to_array_index = {};

	this.time_last_snapshot_taken = -1000;
	this.snapshots = [];

	this._takeGameSnapshot();

	this.dt = 0;
}

Timeline.SNAPSHOT_INTERVAL = Math.round(1000 / Game.TICK_INTERVAL);

Timeline.prototype.update = function() {
	this.seek(this.game.getGameTick()+1);
	this.dt -= 1;
};

Timeline.prototype.seek = function(t) {
	this.dt += t - this.game.getGameTick();
	if (t > this.game.getGameTick()) {
		this._playGameStateFromTimeToDestTime(this.game.getGameTick(), t);
	}
	if (t < this.game.getGameTick()) {
		var time_delta = t - this.game.getGameTick();
		console.log("seeking game state " + this.game.getGameTick() + " -> " + t +
					" (" + time_delta + ")");
		this._playGameStateFromTimeToDestTime(t,t);
	}
};


Timeline.prototype.addAction = function(action) {
	if (!this.action_id_to_array_index.hasOwnProperty(action.id)) {
		action.performed = false;
		this.action_id_to_array_index[action.id] = this.actions.length;
		this.actions.push(action);
		if (action.T < this.game.getGameTick()) {
			this._replayGameStateFromTime(action.T);
		}
	} else {
		this.rescheduleAction(action.id, action.T);
	}
};


Timeline.prototype.rescheduleAction = function(item_id, t_new) {
	var action = this._getEventById(item_id);
	if (action.T === t_new) {
		return;
	}
	var dt = t_new - action.T;
	console.log("Rescheduling action " + action.id + "(" + action.action_type + "). " +
				action.T + " -> " + t_new + "(d:"+dt+", "+(dt*Game.TICK_INTERVAL)+"m)");
	var time_to_seek_to = Math.min(t_new, action.T);
	action.T = t_new;
	this._replayGameStateFromTime(time_to_seek_to);
};

Timeline.prototype.setGameToBeginning = function() {
	this._setGameToSnapshot(this.snapshots[1]);
};

Timeline.prototype._replayGameStateFromTime = function(time) {
	this._playGameStateFromTimeToDestTime(time-1, this.game.getGameTick());
};


Timeline.prototype._playGameStateFromTimeToDestTime = function(time, dest_time) {
	if (time < this.game.getGameTick()) {
		this._rewindToLatestSnapshotByTime(time);
	}

	if (dest_time > this.game.getGameTick()) {
		this._fforwardToTime(dest_time);
	}

};

Timeline.prototype._fforwardToTime = function(time) {
	while (this.game.getGameTick() < time) {
		this._performFrameActions();
		this.game.update();
		if (this.time_last_snapshot_taken + Timeline.SNAPSHOT_INTERVAL < this.game.getGameTick()) {
			this._takeGameSnapshot();
		}
	}
};

Timeline.prototype._rewindToLatestSnapshotByTime = function(time) {
	var snapshot = this._getLatestGameSnapshotByTime(time);
	this.time_last_snapshot_taken = snapshot.T;
	this._removeSnapshotsAfterTime(time);
	this._setGameToSnapshot(snapshot);
};

Timeline.prototype._setGameToSnapshot = function(snapshot) {
	this.game.T = snapshot.T;
	this.game.setState(snapshot);
	this.time_last_snapshot_taken = snapshot.T;
};

Timeline.prototype._removeSnapshotsAfterTime = function(t) {
	for (var snapshot_i in this.snapshots) {
		var snapshot = this.snapshots[snapshot_i];
		if (snapshot.T > t) {
			this.snapshots.splice(snapshot_i);
			return true;
		}
	}
	return false;
};

Timeline.prototype._getLatestGameSnapshotByTime = function(t) {
	var best_time = 0;
	var best_snapshot = null;
	for (var snapshot_i in this.snapshots) {
		var snapshot = this.snapshots[snapshot_i];
		if (snapshot.T > best_time && snapshot.T <= t) {
			best_time = snapshot.T;
			best_snapshot = snapshot;
		}
	}
	return best_snapshot;
};

Timeline.prototype._getEventById = function(event_id) {
	return this.actions[this.action_id_to_array_index[event_id]];
};

Timeline.prototype._takeGameSnapshot = function() {
	var game_snapshot = GameState.clone(this.game.getGameState());
	game_snapshot.T = this.game.getGameTick();
	this.snapshots.push(game_snapshot);
	this.time_last_snapshot_taken = this.game.getGameTick();
};

Timeline.prototype._performFrameActions = function() {
	for (var i in this.actions) {
		var ev = this.actions[i];
		if (ev.T === this.game.getGameTick()) {
			this.game.performAction(ev.id_plr, ev);
		}
	}
};


exports.Timeline = Timeline;



},{"../Game":1}],10:[function(require,module,exports){
/* jshint browser: true */
/* global gl: true */
/* global io: false */
/* global gl_create: false */
/* global alert: false */

"use strict";

var Player = require('../Player').Player;
var GameClient = require('./GameClient').GameClient;
var Game = require("../Game").Game;
var GameRenderer = require('./GameRenderer').GameRenderer;
var Time = require('../Time').Time;

var NOT_CONNECTED = "DISCONNECTED";
var CONNECTED = "CONNECTED";

var Client = function() {
	this.status_client = NOT_CONNECTED;

	this.socket = null;

	this.renderer = null;
	this.game_current = null;
	this.player_local = null;

	this.is_logged_in = false;

	this.time_ping_sent = 0;
	this.time_join_sent = false;

	this.ping = 0;
};

Client.prototype.startGame = function(gamestate, T, ping) {
	var g = new GameClient(this, gamestate, this.player_local);
	g.initialize(T, ping);
	this.game_current = g;
	this.game_current.startUpdate();
	this.renderer = new GameRenderer( gl, this.game_current.game );
};

Client.prototype.initListeners = function( socket ) {
	var self = this;
	socket.on( 'login_response', function(data) {
		self.player_local = new Player();
		self.player_local.data = data;
		self.player_local.socket = socket;
		self.is_logged_in = true;
		socket.emit( 'ping', {} );
		self.time_ping_sent = Time.time();
		self.status_client = CONNECTED;
		//alert( "Connected: id=" + data.id + ", display_name=" + data.display_name );
		self.sendJoin();
	} );
	socket.on( 'pong' , function(data) {
		self.ping = Time.time() - self.time_ping_sent;
		if ( self.game_current ) {
			self.game_current.serverTimeReceived(data, self.ping);
		}
		setTimeout( function() {
			socket.emit( 'ping', {} );
			self.time_ping_sent = Time.time();
			}, 1000 );
	} );
	socket.on( 'joined_game', function(join_info) {
		var game_state = join_info.game_state;
		console.log(JSON.stringify(game_state));
		self.ping = Time.time() - self.time_join_sent;
		self.startGame(game_state, join_info.T, self.ping);
	} );
	socket.on( 'error', function(data) {
		self.status_client = NOT_CONNECTED;
		alert( data || "random socket error" );
	} );
	socket.on( 'connect_failed', function(msg) {
		self.status_client = NOT_CONNECTED;
		alert( "connection failed: " + msg );
	} );
};


Client.prototype.sendJoin = function() {
	this.socket.emit( 'join', {} );
	this.time_join_sent = Time.time();
};

Client.prototype.sendLogin = function() {
	this.socket.emit( 'login', {} );
};

Client.prototype.isRunningLocally = function() {
	switch(window.location.protocol) {
	case 'http:':
	case 'https:':
		return false;
	case 'file:':
		return true;
	default:
		return false;
	}
};

Client.prototype.start = function() {
	var self = this;

	gl = gl_create( "GLCanvas" );

	//this.socket = io.connect('http://spacegame-spacegame.rhcloud.com:20508');
	//this.socket = io.connect('127.4.55.129:20508');
	//this.socket = io.connect('http://stark-tundra-6897.herokuapp.com');
	if (this.isRunningLocally()) {
		this.socket = io.connect('http://127.0.0.1:1337');
	} else {
		this.socket = io.connect('http://stark-tundra-6897.herokuapp.com');
		Game.ADD_LATENCY = 0;
	}

	if ( Game.ADD_LATENCY ) {
		this.socket.emitBase = this.socket.emit;
		this.socket.emit = function(msg, data) {
			var emitFunc = function() {
				self.socket.emitBase( msg, data );
			};

			setTimeout(emitFunc, Game.ComputeSimulatedLatency(Time.time()));
		};
	}

	this.initListeners(this.socket);

	this.sendLogin();


	setInterval( function() { self.render.call(self); }, 1000/60 );

};

Client.prototype.render = function() {
	gl.useProgram( null );
	gl.viewport( 0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight );
	gl.clearColor( 0.0, 0.0, 0.0, 1.0 );
	gl.clear( gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT );

	if ( this.renderer ) {
		this.renderer.render();
	}

};

new Client().start();


},{"../Game":1,"../Player":2,"../Time":5,"./GameClient":7,"./GameRenderer":8}]},{},[10])
;