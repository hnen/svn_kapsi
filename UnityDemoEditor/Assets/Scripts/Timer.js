#pragma strict

var audioSource : AudioSource;

function t() {
	if (audioSource != null) {
		return audioSource.time;
	} else {
		return Time.time;
	}
}
