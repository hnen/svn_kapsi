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


