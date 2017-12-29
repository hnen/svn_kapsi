

// fuckings to apollonius

function gasket_circleradius( n1, n2, n3 )
{
	return n1*n2*n3 / (n1*n2 + n2*n3 + n3*n1 + 2 * Math.sqrt( n1*n2*n3 *(n1+n2+n3) ) );
//	return n1*n2*n3 / (n1*n2 + n2*n3 + n3*n1 + 2 * Math.sqrt( n1*n2 + n2*n3 + n3*n1 ) );
}
function sqr( a )
{
	return a*a;
}
function gasket_drawCircle( c, mat, m )
{
	shader = shaders.seppo;
	var sc = [ c[3], c[3], c[3] ];
	var matr = mat_mul( mat_translate(c), mat );
	matr = mat_mul(mat_scale(sc),matr);
	matr = mat_mul(matr, mat_view(cam_pos, cam_view, cam_up));
	renderMesh( shaders.white, m, matr, null, {uniform_t: t()} );
}

// vitut toimin
function gasket_circles( c1, c2, c3, step, mat, m )
{
	if ( step > 0 )
	{
		var c = [];

		c[3] = gasket_circleradius( c1[3], c2[3], c3[3] );

		var a1 = sqr(c1[0])-sqr(c2[0]) + sqr(c1[2])-sqr(c2[2]) + sqr(c2[3]+c[3]) - sqr(c1[3]+c[3]);
		var a2 = sqr(c1[0])-sqr(c3[0]) + sqr(c1[2])-sqr(c3[2]) + sqr(c3[3]+c[3]) - sqr(c1[3]+c[3]);
		var x1 = 2*(c2[0]-c1[0]);
		var x2 = 2*(c3[0]-c1[0]);
		var y1 = 2*(c2[2]-c1[2]);
		var y2 = 2*(c3[2]-c1[2]);
		var d = (x2*y1-x1*y2);
		c[0] = (a1*y2-a2*y1) / d;
		c[1] = 0;
		c[2] = -(a1*x2-a2*x1) / d;
		gasket_drawCircle(c, mat, m);

		if ( c[3] > 0.02 )
		{
			gasket_circles( c1, c2, c, step-1, mat, m);
			gasket_circles( c1, c3, c, step-1, mat, m);
			gasket_circles( c2, c3, c, step-1, mat, m);
		}
	}

}

