
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


