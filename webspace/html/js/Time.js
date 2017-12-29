"use strict";

var Time = function() {
};

Time.time = function() {
	return + new Date();
};

exports.Time = Time;

