#pragma strict

private var sound : AudioSource;

function Start () {
	this.sound = this.GetComponent( AudioSource );
	if ( this.sound == null )
		Debug.Log("no audio :D");
}

function Update () {
	var rewindTime = Input.GetKeyDown(KeyCode.LeftControl) ? 8 : 1;

	if ( Input.GetKeyDown(KeyCode.LeftArrow) ) 
		this.sound.time = Mathf.Max( 0, this.sound.time - rewindTime );
	if ( Input.GetKeyDown(KeyCode.RightArrow) )
		this.sound.time = this.sound.time + rewindTime;

}