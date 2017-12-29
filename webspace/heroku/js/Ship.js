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

