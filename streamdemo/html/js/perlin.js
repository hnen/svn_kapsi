function perlin_create_fractal_uint8(size) {
	var p32 = perlin_create_fractal_float32(size);
	var p8 = new Uint8Array(size*size);
	for (i = 0; i < size*size; i++) {
		var val = Math.floor(p32[i] * 255);
		if (val < 0) val = 0;
		if (val > 255) val = 255;
		p8[i] = val;
	}
	return p8;
}

function perlin_create_fractal_float32(size) {
	var div = 0;
	var sdiv = size;
	var arr = new Float32Array(size*size);
	var f = 1;
	var tf = 0;
	var p = 2;
	while(sdiv > 32) {
		var res = perlin_create_float32(size,div);
		for (var i = 0; i < size*size; i++) {
			arr[i] += Math.pow(res[i],p)*f;
		}
		sdiv /= 2;
		p = 1 + p * 0.45;
		tf += f;
		f/=1.2;
		div += 1;
	}
	for (var i = 0; i < size*size; i++) {
		arr[i] /= tf;
		//arr[i] = Math.pow(arr[i], 1.5);
	}
	return arr;
}

function perlin_create_uint8(size, div) {
	var p32 = perlin_create_float32(size, div);
	var p8 = new Uint8Array(size*size);
	for (i = 0; i < size*size; i++) {
		var val = Math.floor(p32[i] * 255);
		if (val < 0) val = 0;
		if (val > 255) val = 255;
		p8[i] = val;
	}
	return p8;
}

function perlin_create_float32(size, div) {
	var dirs_size = Math.pow(2, div) + 1;
	var dirs = new Float32Array(dirs_size * dirs_size * 2);
	var i = 0;
	for (var y = 0; y < dirs_size; y += 1) {
		for (var x = 0; x < dirs_size; x += 1) {
			var v_x = Math.random() - 0.5;
			var v_y = Math.random() - 0.5;
			var m = Math.sqrt(v_x*v_x + v_y*v_y);
			v_x /= m;
			v_y /= m;
			dirs[i+0] = v_x;
			dirs[i+1] = v_y;
			i += 2;
		}
	}

	var perlin = new Float32Array(size*size);
	var grid_size = Math.floor(size / (dirs_size-1));

	i=0;
	for (var y = 0; y < size; y += 1) {
		for (var x = 0; x < size; x += 1) {
			var dx = Math.floor(x / grid_size);
			var dy = Math.floor(y / grid_size);
			var gx = dx * grid_size;
			var gy = dy * grid_size;
			var cx = (x - gx) / grid_size;
			var cy = (y - gy) / grid_size;
			var di = (dy*dirs_size+dx)*2;
			var dot0 = dirs[di+0] * (x-gx) + dirs[di+1] * (y-gy);
			var dot1 = dirs[di+2+0] * (x-(gx+grid_size)) + dirs[di+2+1] * (y-gy);
			var dot2 = dirs[di+dirs_size*2+0] * (x-gx) + dirs[di+dirs_size*2+1] * (y-(gy+grid_size));
			var dot3 = dirs[di+dirs_size*2+2+0] * (x-(gx+grid_size)) + dirs[di+dirs_size*2+2+1] * (y-(gy+grid_size));
			var z0 = cberp(dot0, dot2, cy);
			var z1 = cberp(dot1, dot3, cy);
			perlin[i] = (cberp(z0,z1,cx) / grid_size + 1.0) * 0.5;
			i+=1;
		}
	}

	return perlin;
}



