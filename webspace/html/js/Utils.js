"use strict";

function Utils() {}

Utils.rndString = function( length ) {
	var chars = "0123456789abcdefghijklmnopqrstuvwxyz";
	var ret = "";
	for ( var i = length; i > 0; i-- ) {
		ret = ret + chars[ Math.round( Math.random() * (chars.length-1) ) ];
	}
	return ret;
};

Utils.m_w = 123456789;
Utils.m_z = 987654321;
Utils.mask = 0xffffffff;

Utils.seed = function(i) {
	if(i==0)i=1;
	Utils.m_w = i & Utils.mask;
	Utils.m_z = 34242342;
}

Utils.rnd = function() {
	Utils.m_z = (36969 * (Utils.m_z & 65535) + (Utils.m_z >> 16)) & Utils.mask;
	Utils.m_w = (18000 * (Utils.m_w & 65535) + (Utils.m_w >> 16)) & Utils.mask;
	var result = ((Utils.m_z << 16) + Utils.m_w) & Utils.mask;
	result /= 4294967296;
	return result + 0.5;
}

Utils.lerp = function(a,b,t) {
	return a + (b-a)*t;
}

exports.Utils = Utils;

