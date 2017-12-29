#pragma strict


static function Lerp(a : float, b : float, x : float) {
	if (x < 0) return a;
	if (x > 1) return b;
	return a + (b-a) * x;
}
