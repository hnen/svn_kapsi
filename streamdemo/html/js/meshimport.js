

function meshimport_import(filename, onready) {

	file_loadBin(filename, function(data) {
		var object = {};
		p_meshimport_readChunk(data, object);
		onready(object);
	} );

}


function p_meshimport_readChunk(data, out_object) {
	var chunk_name = String.fromCharCode(data[0]) + String.fromCharCode(data[1]) + String.fromCharCode(data[2])
					+ String.fromCharCode(data[3]);
	var chunk_length = p_meshimport_bytesToInt32(data.subarray(4,4+4));
	var chunk_data = data.subarray(4+4,4+4+chunk_length);

	if (chunk_name === 'MESH') {
		p_meshimport_readMesh(chunk_data, out_object);
	} else if (chunk_name === 'POSI') {
		p_meshimport_readPositions(chunk_data, out_object);
	} else if (chunk_name === 'NORM') {
		p_meshimport_readNormals(chunk_data, out_object);
	} else if (chunk_name === 'INDS') {
		p_meshimport_readFaces(chunk_data, out_object);
	} else if (chunk_name === 'END') {
	} else {
		console.log("Unknown chunk \"" + chunk_name + "\", length=" + chunk_length);
	}

	return chunk_length + 4 + 4;
}

function p_meshimport_readMesh(data, out_object) {
	var vertex_count = p_meshimport_bytesToInt32(data.subarray(0,4));
	var face_count = p_meshimport_bytesToInt32(data.subarray(4,4+4));

	out_object.vertex_count = vertex_count;
	out_object.face_count = face_count;

	var i = 8;
	while(i < data.length) {
		i += p_meshimport_readChunk(data.subarray(i), out_object);
	}
}

function p_meshimport_readPositions(data, out_object) {
	out_object.positions = p_meshimport_dataToFloatArray(data);
}

function p_meshimport_readNormals(data, out_object) {
	out_object.normals = p_meshimport_dataToFloatArray(data);
}

function p_meshimport_readFaces(data, out_object) {
	out_object.quads = p_meshimport_dataToIntArray(data);

	// triangulate
	var triangles = [];
	for (var i = 0; i < out_object.quads.length; i += 4) {
		if (out_object.quads[i+3] < 0) {
			triangles.push(out_object.quads[i+0]);
			triangles.push(out_object.quads[i+1]);
			triangles.push(out_object.quads[i+2]);
		} else {
			triangles.push(out_object.quads[i+0]);
			triangles.push(out_object.quads[i+1]);
			triangles.push(out_object.quads[i+2]);
			triangles.push(out_object.quads[i+2]);
			triangles.push(out_object.quads[i+3]);
			triangles.push(out_object.quads[i+0]);
		}
	}
	out_object.triangles = new Uint32Array(triangles);
}

function p_meshimport_dataToFloatArray(data) {
	var fdata = new Float32Array(data.length / 4);
	for (var b = 0; b < data.length; b += 4) {
		var x = p_meshimport_bytesToFloat32(data.subarray(b+0,b+4));
		fdata[b/4] = x;
	}
	return fdata;
}

function p_meshimport_dataToIntArray(data) {
	var idata = new Uint32Array(data.length / 4);
	for (var b = 0; b < data.length; b += 4) {
		var x = p_meshimport_bytesToInt32(data.subarray(b+0,b+4));
		idata[b/4] = x;
	}
	return idata;
}

function p_meshimport_bytesToInt32(arr) {
	return arr[0] + (arr[1] << 8) + (arr[2] << 16) + (arr[3] << 24);
}

function p_meshimport_bytesToFloat32(arr) {
	var frac = arr[0] | (arr[1] << 8) | ((arr[2] & 127) << 16);
	var exp = (((arr[2] & 128) >> 7)&1) | ((arr[3] & 127) << 1);
	var sign = (arr[3]>>7) & 1;
	var number = 1.0;
	for (var b = 0; b < 23; b += 1) {
		var B = (frac >> (22-b)) & 1;
		if (B) {
			number += Math.pow(2.0, -(b+1));
		}
	}
	number *= Math.pow(2.0, exp - 127);
	number *= (sign - 0.5) * 2.0;
	return number;
}

