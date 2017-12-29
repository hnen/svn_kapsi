#pragma strict

public var otoPrefab : GameObject;
private var surfaceMesh : Mesh;

public var SPAWN : boolean;

var verts : Vector3[];
var inds : int[];

function Start () {
	this.surfaceMesh = this.GetComponent(MeshFilter).mesh;
	
	this.verts = this.surfaceMesh.vertices;
	this.inds = this.surfaceMesh.triangles;
	
	this.SPAWN = true;
	
}

function Update () {

	if (this.SPAWN) {
		this.SPAWN = false;
		for (var i = 0; i < 100; i++) {
			this.Spawn();
		}
	}
}

function Spawn() {
	var objo : GameObject = GameObject.Instantiate(this.otoPrefab);
	
	var t = this.GetRandomPoint();
	
	var q0 = this.verts[this.inds[t[0]*3+0]];
	var q1 = this.verts[this.inds[t[0]*3+1]];
	var q2 = this.verts[this.inds[t[0]*3+2]];	
	var q01 = new Vector3(q1.x - q0.x, q1.y - q0.y, q1.z - q0.z);
	var q02 = new Vector3(q2.x - q0.x, q2.y - q0.y, q2.z - q0.z);
	var p = new Vector3(q0.x + q01.x * t[1] + q02.x * t[2], q0.y + q01.y * t[1] + q02.y * t[2], q0.z + q01.z * t[1] + q02.z * t[2]);
	var n = new Vector3.Cross(new Vector3(q1.x - q0.x, q1.y - q0.y, q1.z - q0.z), new Vector3(q2.x - q0.x, q2.y - q0.y, q2.z - q0.z));
	n = Vector3.Normalize(n);
	var wn = this.transform.localToWorldMatrix.MultiplyVector(n);
		
	//objo.transform.parent = this.transform;
	objo.transform.SetParent(this.transform);
	objo.transform.localPosition = p;
	//objo.transform.up = wn;
	
}

function GetRandomPoint() {
	var polyareas = new Array();
	var totalArea : float = 0.0f;
	for(var i = 0; i < this.inds.length; i+=3) {
		var p0 = this.verts[this.inds[i+0]];
		var p1 = this.verts[this.inds[i+1]];
		var p2 = this.verts[this.inds[i+2]];
		var area = Vector3.Magnitude(Vector3.Cross(new Vector3(p2.x - p0.x, p2.y - p0.y, p2.z - p0.z),
			new Vector3(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z))) / 2.0f;
		polyareas.push(area);
		totalArea += area;
	}
	
	var polyF = Random.Range(0.0f, totalArea);
	
	var j : int = 0;
	for(var value : float in polyareas) {
		polyF -= value;
		if (polyF > 0) {
			j++;
		} else {
			break;
		}
	} 
	
	var q0 = this.verts[this.inds[j*3+0]];
	var q1 = this.verts[this.inds[j*3+1]];
	var q2 = this.verts[this.inds[j*3+2]];
	
	var q01 = new Vector3(q1.x - q0.x, q1.y - q0.y, q1.z - q0.z);
	var q02 = new Vector3(q2.x - q0.x, q2.y - q0.y, q2.z - q0.z);
	
	var a = Random.Range(0.0f, 1.0f);
	var b = Random.Range(0.0f, 1.0f);
	if (a + b > 1) {
		a = 1 - a;
		b = 1 - b;
	}

	//return new Vector3(q0.x + q01.x * a + q02.x * b, q0.y + q01.y * a + q02.y * b, q0.z + q01.z * a + q02.z * b);
	return [j, a, b];
}


