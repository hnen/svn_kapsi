function meshimport_import(filename, onready) {

	file_loadBin(filename, function(data) {
		var dataOutput = {};
		var object = new Mesh();
		p_meshimport_readMesh(data, dataOutput);
		console.log("Loaded mesh! filename= "+ filename +" bytes=" + data.length);

		object.vertices = dataOutput.vertices;
		object.normals = dataOutput.normals;
		if (dataOutput.uv) {
			object.uv = dataOutput.uv;
		}
		//object.triangles = dataOutput.triangleSubmeshes[0];
		//object._triangles = dataOutput.triangleSubmeshes;
		for (var i = 0; i < dataOutput.triangleSubmeshes.length; i++) {
			object.SetTriangles(dataOutput.triangleSubmeshes[i], i);
		}

		onready(object);
	});

}

function p_meshimport_readMesh(data, dst_object) {
	var vertex_count = p_meshimport_bytesToInt32(data.subarray(0, 4));

	dst_object.vertexCount = vertex_count;

	var P = 4;
	while (P < data.length) {
		var chunk_name = String.fromCharCode(data[P + 1]) + String.fromCharCode(data[
			P + 2]) + String.fromCharCode(data[P + 3]);
		P += 4;
		if (chunk_name === 'POS') {
			dst_object.vertices = p_meshimport_dataToVec3Array(data.subarray(P, P + 4 *
				3 * vertex_count));
			//dst_object.vertices_farr = p_meshimport_dataToFloatArray(data.subarray(P, P + 4 *
			//	3 * vertex_count));
			P += 4 * 3 * vertex_count;
		} else if (chunk_name === 'NRM') {
			dst_object.normals = p_meshimport_dataToVec3Array(data.subarray(P, P + 4 *
				3 * vertex_count));
			//dst_object.normals_farr = p_meshimport_dataToFloatArray(data.subarray(P, P + 4 *
			//	3 * vertex_count));
			P += 4 * 3 * vertex_count;
		} else if (chunk_name === 'UV0') {
			dst_object.uv = p_meshimport_dataToVec2Array(data.subarray(P, P + 4 * 2 *
				vertex_count));
			//dst_object.uv_farr = p_meshimport_dataToFloatArray(data.subarray(P, P + 4 * 2 *
			//	vertex_count));
				P += 4 * 2 * vertex_count;
		} else if (chunk_name === 'IND') {
			P += p_meshimport_readIndices(data.subarray(P), dst_object);
		} else {
			console.log("Unknown chunk \"" + chunk_name + "\" P = " + P +
				", datalength = " + data.length + ", vertexcount = " + vertex_count);
			break;
		}
	}

}

function p_meshimport_readIndices(data, dst_object) {
	var submesh_count = p_meshimport_bytesToInt32(data.subarray(0, 4));
	dst_object.subMeshCount = submesh_count;
	var P = 4;
	dst_object.triangleSubmeshes = [];
	for (var i = 0; i < submesh_count; i++) {
		var index_count = p_meshimport_bytesToInt32(data.subarray(P, P + 4));
		P += 4;
		var indices = p_meshimport_dataToIntArray(data.subarray(P, P + index_count *
			4));
		//dst_object.triangleSubmeshes = dst_object.triangleSubmeshes.concat(indices);
		dst_object.triangleSubmeshes.push(indices);
		P += index_count * 4;
	}
	return P;
}



function p_meshimport_dataToFloatArray(data) {
	var fdata = new Float32Array(data.length / 4);
	for (var b = 0; b < data.length; b += 4) {
		var x = p_meshimport_bytesToFloat32(data.subarray(b + 0, b + 4));
		fdata[b / 4] = x;
	}
	return fdata;
}

function p_meshimport_dataToVec3Array(data) {
	var fdata = [];
	for (var b = 0; b < data.length; b += 12) {
		var _x = p_meshimport_bytesToFloat32(data.subarray(b + 0, b + 4));
		var _y = p_meshimport_bytesToFloat32(data.subarray(b + 4, b + 8));
		var _z = p_meshimport_bytesToFloat32(data.subarray(b + 8, b + 12));
		fdata.push({x: _x, y: _y, z: _z});
	}
	return fdata;
}

function p_meshimport_dataToVec2Array(data) {
	var fdata = [];
	for (var b = 0; b < data.length; b += 8) {
		var _x = p_meshimport_bytesToFloat32(data.subarray(b + 0, b + 4));
		var _y = p_meshimport_bytesToFloat32(data.subarray(b + 4, b + 8));
		fdata.push({x: _x, y: _y});
	}
	return fdata;
}


function p_meshimport_dataToIntArray(data) {
	var idata = new Uint32Array(data.length / 4);
	for (var b = 0; b < data.length; b += 4) {
		var x = p_meshimport_bytesToInt32(data.subarray(b + 0, b + 4));
		idata[b / 4] = x;
	}
	return idata;
}

function p_meshimport_bytesToInt32(arr) {
	return arr[0] + (arr[1] << 8) + (arr[2] << 16) + (arr[3] << 24);
}

function p_meshimport_bytesToFloat32(arr) {
	var frac = arr[0] | (arr[1] << 8) | ((arr[2] & 127) << 16);
	var exp = (((arr[2] & 128) >> 7) & 1) | ((arr[3] & 127) << 1);
	var sign = (arr[3] >> 7) & 1;
	var number = 1.0;
	for (var b = 0; b < 23; b += 1) {
		var B = (frac >> (22 - b)) & 1;
		if (B) {
			number += Math.pow(2.0, -(b + 1));
		}
	}
	number *= Math.pow(2.0, exp - 127);
	number *= (sign - 0.5) * 2.0;
	return number;
}
