#pragma strict

function OnDrawGizmosSelected() {
	this.transform.parent.GetComponent(Path).OnDrawGizmosSelected();
}

