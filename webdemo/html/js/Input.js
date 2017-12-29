
var p_keysDown = {};

var KEY_DOWN = 40;
var KEY_UP = 38;
var KEY_LEFT = 37;
var KEY_RIGHT = 39;
var KEY_SPACE = 32;

var KEY_A = 65;
var KEY_B = 66;
var KEY_C = 67;
var KEY_D = 68;
var KEY_E = 69;
var KEY_F = 70;
var KEY_G = 71;
var KEY_H = 72;
var KEY_I = 73;
var KEY_J = 74;
var KEY_K = 75;
var KEY_L = 76;
var KEY_M = 77;
var KEY_N = 78;
var KEY_O = 79;
var KEY_P = 80;
var KEY_Q = 81;
var KEY_R = 82;
var KEY_S = 83;
var KEY_T = 84;
var KEY_U = 85;
var KEY_V = 86;
var KEY_W = 87;
var KEY_X = 88;
var KEY_Y = 89;
var KEY_Z = 90;

var KEY_0 = 48;
var KEY_1 = 49;
var KEY_2 = 50;
var KEY_3 = 51;
var KEY_4 = 52;
var KEY_5 = 53;
var KEY_6 = 54;
var KEY_7 = 55;
var KEY_8 = 56;
var KEY_9 = 57;

var lastkeydown;

var p_keyEvents = {};

function keyDown( keyCode ) {
	return p_keysDown[keyCode];
}

function onPressed( keyCode, func ) {
	p_keyEvents[keyCode] = func;
}

function p_input_onKeyDown(ev) {
	lastkeydown = ev.keyCode;
	p_keysDown[ev.keyCode] = true;
	if ( p_keyEvents[ev.keyCode] ) {
		p_keyEvents[ev.keyCode]();
	}
}
function p_input_onKeyUp(ev) {
	p_keysDown[ev.keyCode] = false;
}

document.onkeydown = p_input_onKeyDown;
document.onkeyup = p_input_onKeyUp;

