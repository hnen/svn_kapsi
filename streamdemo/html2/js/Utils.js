
function max(a,b) {
	return a>b?a:b;
}
function min(a,b) {
	return a<b?a:b;
}
function mod(a,b) {
	return ((a%b)+b)%b;
}
function frac(a) {
	return mod(a,1);
}

function lerp(a,b,t,t0) {
	return a + (b-a) * t/t0;
}
function lerpc(a,b,t,t0) {
	var x = t/t0;
	if (x < 0) x = 0;
	if (x > 1) x = 1;
	return a + (b-a) * x;
}

// f(x) =  ax^3 +  bx^2 + cx
// f'(x)= 3ax^2 + 2bx	+ c
// f(1) = A
// f'(0) = 0
// f'(1) = 0
// c = 0
// 3a + 2b = 0
// b = -3/2a
// a - 3/2a = A
// -1/2a = A
// a = -2A
// b = 3A
// c = 0

function cberp(a,b,t) {
	if (t<0) t=0;
	if (t>1) t=1;
	var A = b-a;
	return a + (3-2*t)*t*t*A;
}

function noise(i) {
	return mod( (i+234828)*(i+28133)*(i+23131), 715517 ) / 715517;
}

function assert(condition, message) {
	if (!condition) {
		throw message || "Assertion failed";
	}
}

