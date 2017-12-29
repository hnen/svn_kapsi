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
