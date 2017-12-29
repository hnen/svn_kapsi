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

