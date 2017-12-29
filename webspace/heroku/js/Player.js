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

