#pragma strict

@DrawGizmo (GizmoType.SelectedOrChild)

public var Loop : boolean; 
 
function OnDrawGizmosSelected() {
	Gizmos.color = Color.white;
	var pathlen = this.GetPathLength();
	var step = pathlen / 100;
	for (var f = step; f < pathlen; f += step) {
		var p0 = this.GetPosition(f-step);
		var p1 = this.GetPosition(f);
		var r = this.GetRotation(f);
		var v = new Vector3(0,0,1);
		Gizmos.color = Color.white;
		Gizmos.DrawLine(p0, p1);
		Gizmos.color = Color.grey;
		Gizmos.DrawLine(p1, p1 + (r * v));
	}
}

function GetRotation(p : float) {
	var i = this.SelectEdge(p);
	return this.GetRotationFromEdge(i[0], i[1]);	
}

function GetPosition(p : float) {
	var i = this.SelectEdge(p);
	return this.GetPositionFromEdge(i[0], i[1]);	
}

function SelectEdge(p : float) {

	if (this.Loop) {
		var pathlen = this.GetPathLength();
		p = Mathf.Repeat(p, pathlen);
	}

	for(var i = 0; i < this.NumOfSamples() - (this.Loop ? 0 : 1); i++) {
		var p0 = GetSample(i).position;
		var p1 = GetSample((i+1) % this.NumOfSamples()).position;
		var edgelen = Vector3.Magnitude(new Vector3(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z));
		if (p <= edgelen) {
			return [i, p/edgelen];
		} else {
			p -= edgelen;
		}
	}
	
	return [this.NumOfSamples() - 1, 1.0];
}

function GetPositionFromEdge(i : int, x : float) {
	var p0 = GetSample(i-1).position;
	var p1 = GetSample(i).position;
	var p2 = GetSample(i+1).position;
	var p3 = GetSample(i+2).position;

	return new Vector3(
		0.5f * ( (2.0f * p1.x) + (-p0.x + p2.x) * x + (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * x*x + (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x)*x*x*x),
		0.5f * ( (2.0f * p1.y) + (-p0.y + p2.y) * x + (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * x*x + (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y)*x*x*x),
		0.5f * ( (2.0f * p1.z) + (-p0.z + p2.z) * x + (2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * x*x + (-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z)*x*x*x));
}


function GetRotationFromEdge(i : int, x : float) {
	var p0 = GetSample(i).rotation;
	var p1 = GetSample(i+1).rotation;
	return Quaternion.Slerp(p0, p1, x);
}

function GetPathLength() {
	var ret : float = 0;
	for (var i = 1; i < this.NumOfSamples(); i++) {
		var p0 = GetSample(i-1).position;
		var p1 = GetSample(i).position;
		ret += Vector3.Magnitude(new Vector3(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z));
	}
	if (this.Loop) {
		var q0 = GetSample(this.NumOfSamples() - 1).position;
		var q1 = GetSample(0).position;
		ret += Vector3.Magnitude(new Vector3(q1.x - q0.x, q1.y - q0.y, q1.z - q0.z));
	}
	return ret;
}

function NumOfSamples() {
	return this.transform.childCount;
}

function GetSample(p : int) {
	if (p < 0) p = 0;
	if (p >= this.transform.childCount) p = this.transform.childCount - 1;
	return this.transform.GetChild(p);
}

