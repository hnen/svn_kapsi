"use strict";

var Utils = require( '../Utils' ).Utils;
var GameServer = require("./GameServer").GameServer;
var Game = require("../Game").Game;
var GameState = require("../Game").GameState;
var Player = require("../Player").Player;
var Time = require("../Time").Time;

var dbg = require('util');

var Server = function() {
	this.games = [];
	this.connections = {};
	this.player_id_to_game = {};
	this.player_last_message_time = {};
	this.TIMEOUT_PLAYER_CONNECTION = 5000;
};

Server.prototype.createNewGame = function() {
	var ngame = GameServer.create();
	this.games.push( ngame );
	ngame.startUpdate();
	return ngame;
};

Server.prototype.connectPlayer = function( socket, plr ) {
	this.connections[plr.data.id] = plr;
	plr.socket = socket;
};

Server.prototype.logout = function(plr, reason) {
	console.log(plr.data.id + " logout: " + reason );
	if ( plr.data.current_game_id ) {
		var game = this.getGame( plr.data.current_game_id );
		game.part( plr.data.id );
	}
	delete this.connections[plr.data.id];
};

Server.prototype.getGame = function( gameId ) {
	for ( var i in this.games ) {
		if (this.games[i].getGameId() === gameId) {
			return this.games[i];
		}
	}
	return null;
};

Server.prototype.getPlayerById = function( plr_id ) {
	return this.connections[plr_id];
};

Server.prototype.getPlayerBySocket = function( socket, func ) {
	var self = this;
	socket.get('plr_id', function(err,plr_id) {
		func(self.connections[plr_id]);
	} );
};

Server.prototype.joinPlayer = function( player, game ) {
	dbg.debug( "Joining player " + player.data.id + " to game " + game.getGameId() );
	this.player_id_to_game[player.data.id] = game.getGameId();
	game.join( player );
	return true;
};

Server.prototype.joinPlayerToRandomGame = function( player ) {
	if ( this.games.length === 0 ) {
		this.createNewGame();
	}
	var game = this.games[ Math.round( Math.random() * (this.games.length-1) ) ];
	if ( this.joinPlayer( player, game ) ) {
		return game;
	} else {
		return null;
	}
};

Server.prototype.getPlayerGame = function(plr) {
	var gameId = this.player_id_to_game[plr.data.id];
	return this.getGame( gameId );
};

Server.prototype.createNewPlayer = function() {
	var ret = new Player();
	ret.data.id = Utils.rndString(8);
	ret.data.display_name = "anon_" + Utils.rndString( 6 );
	return ret;
};

Server.prototype.checkTimeouts = function() {
	for( var plr_id in this.player_last_message_time ) {
		var time_last_msg = this.player_last_message_time[plr_id];
		var time_since_msg = Time.time() - time_last_msg;
		if (time_since_msg > this.TIMEOUT_PLAYER_CONNECTION) {
			this.logout(this.getPlayerById(plr_id), "Ping timeout");
		}
	}
};

Server.prototype.initListeners = function(io) {
	var self = this;
//		setInterval( function() { self.checkTimeouts.call(self) } , 5000 );

	io.sockets.on('connection', function (socket) {

		if ( Game.ADD_LATENCY ) {
			socket.emitBase = socket.emit;
			socket.emit = function( msg, data ) {
				var emitFunc = function() { socket.emitBase(msg, data); };
				setTimeout(emitFunc, Game.ComputeSimulatedLatency(Time.time()));
			};
		}

		socket.on('login', function (login_params) {
			var plr = self.createNewPlayer();
			socket.set('plr_id', plr.data.id, function() {
				self.player_last_message_time[plr.data.id] = Time.time;
				console.log(plr.data.id + " logged in");
				self.connectPlayer( socket, plr );
				socket.emit( 'login_response', plr.data );
			});
		});

		socket.on('join', function (join_params) {
			self.getPlayerBySocket( socket, function(plr) {
				self.player_last_message_time[plr.data.id] = Time.time;
				var game = self.joinPlayerToRandomGame( plr );
				if (game === null) {
					socket.emit( 'error', "Couldn't join to the game" );
				}
			} );
		});
		socket.on('ping', function() {
			self.getPlayerBySocket( socket, function(plr) {
				if (plr == null) {
					socket.emit( 'disconnect', 'tried to send ping when not logged in' );
				} else {
					self.player_last_message_time[plr.data.id] = Time.time;
					var game = plr && self.getPlayerGame( plr );
					if (game && plr) {
						socket.emit( 'pong', game.getGameTick() );
					} else {
						socket.emit( 'pong', 0 );
					}
				}
			} );
		});
		socket.on('disconnect', function() {
			self.getPlayerBySocket( socket, function(plr) {
				if (plr != null) {
					self.player_last_message_time[plr.data.id] = Time.time;
					self.logout( plr, "Client disconnected" );
				}
			});
		});
	});
};

exports.Server = Server;
