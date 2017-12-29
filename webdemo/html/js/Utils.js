
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

function noise(i) {
	return mod( (i+234828)*(i+28133)*(i+23131), 715517 ) / 715517;
}


