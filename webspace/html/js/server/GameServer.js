"use strict";

var Game = require('../Game').Game;
var Utils = require( '../Utils' ).Utils;
var Time = require( '../Time' ).Time;
var Terrain = require('../Terrain').Terrain;

function GameServer(game_id) {
	this.game = Game.createNew(game_id);
	this.action_queue = [];
}

GameServer.prototype.Send = function(player, msg, params) {
	player.socket.emit(this.game.getGameId() + "_" + msg, params);
};

GameServer.prototype.getGameTick = function() {
	return this.game.getGameTick();
}

GameServer.prototype.broadcast = function(msg, params) {
	for (var i in this.game.players) {
		this.Send( this.game.players[i], msg, params );
	}
};

GameServer.create = function() {
	var ret = new GameServer(GameServer._generateGameId());
	return ret;
};

GameServer._generateGameId = function() {
	return "game_" + Utils.rndString(8);
}

GameServer.prototype.update = function() {
	for( var i in this.action_queue ) {
		var queue_item = this.action_queue[i];
		this.game.performAction( queue_item[0], queue_item[1] );
	}
	this.action_queue = [];
	this.game.update();
};


GameServer.prototype.startUpdate = function() {
	var self = this;
	this.next_tick = Time.time() + Game.TICK_INTERVAL;
	setTimeout( function() { self.runUpdate(self); }, 0 );
};

GameServer.prototype.runUpdate = function(self) {
	var T = Time.time();
	this.lastUpdateTime = T;
	var dt = T - this.next_tick;

	for( ; dt >= 0; dt -= Game.TICK_INTERVAL ) {
		self.update();
		self.next_tick += Game.TICK_INTERVAL;
	}

	var dt0 = self.next_tick - Time.time();

	setTimeout( function() { self.runUpdate(self); }, dt0 );
};

GameServer.prototype.part = function(plr_id) {
	if (this.game.player_exists(plr_id)) {
		this.game.part(plr_id);
		var action_data = {
			id: plr_id + "_" + Utils.rndString(8),
			T: this.game.getGameTick()-1,action_type: "part",
			id_plr: plr_id
		};
		this.broadcast("action", action_data);
	}
};

GameServer.prototype.spawn = function(plrId) {
	var ship_state = this.game.spawn( plrId );
	var action_data = {
		id: plrId + "_" + Utils.rndString(8),
		T: this.game.getGameTick()-1,
		action_type: "spawn",
		ship_data: ship_state, id_plr: plrId
	};
	this.broadcast("action", action_data);
};

GameServer.prototype.addToActionQueue = function( plr_id, action ) {
	this.action_queue.push( [plr_id, action] );
};

GameServer.prototype.getGameId = function() {
	return this.game.getGameId();
}

GameServer.prototype.join = function( plr ) {
	if ( this.game.join(plr) ) {
		this.broadcast( "joined", plr.data );
		var self = this;
		plr.socket.on( this.game.getGameId() + "_requestspawn", function() {
			console.log(plr.data.id + " requested spawning!");
			self.spawn( plr.data.id );
		} );
		plr.socket.on( this.game.getGameId() + "_action", function(action) {
			console.log( "action: " + JSON.stringify( action ) );
			var action_data = {
				id_plr: plr.data.id,
				action_type: action.action_type,
				id: action.id,
				T: self.game.getGameTick()-1
			};
			self.broadcast("action", action_data);
			self.addToActionQueue( plr.data.id, action );
		} );

		var gameState = this.game.getGameState();
		var t = this.game.getGameTick();
		plr.socket.emit('joined_game', {game_state: gameState, T: t });

		console.log("sending gamestate:");
		console.dir(gameState)
	}
};

exports.GameServer = GameServer;

