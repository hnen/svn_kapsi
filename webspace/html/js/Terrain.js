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


