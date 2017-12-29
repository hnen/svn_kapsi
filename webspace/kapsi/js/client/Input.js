/* jshint browser: true */
/* exported KEY_DOWN */
/* exported KEY_UP */
/* exported KEY_LEFT */
/* exported KEY_RIGHT */
/* exported input_isKeyDown */
/* exported input_wasKeyPressed */
/* exported input_wasKeyReleased */
/* exported p_input_frameChanged */

"use strict";

var p_keysDown = {};
var p_keysDown_previousFrame = {};

var KEY_DOWN = 40;
var KEY_UP = 38;
var KEY_LEFT = 37;
var KEY_RIGHT = 39;

function input_isKeyDown( keyCode ) {
	return p_keysDown[keyCode];
}
function input_wasKeyPressed( keyCode ) {
	return p_keysDown[keyCode] && !p_keysDown_previousFrame[keyCode];
}
function input_wasKeyReleased( keyCode ) {
	return !p_keysDown[keyCode] && p_keysDown_previousFrame[keyCode];
}

function p_input_onKeyDown(ev) {
	p_keysDown[ev.keyCode] = true;
}
function p_input_onKeyUp(ev) {
	p_keysDown[ev.keyCode] = false;
}

function p_input_frameChanged() {
	p_keysDown_previousFrame = {};
	for (var i in p_keysDown) {
		p_keysDown_previousFrame[i] = p_keysDown[i];
	}
}

document.onkeydown = p_input_onKeyDown;
document.onkeyup = p_input_onKeyUp;

