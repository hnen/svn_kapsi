
var p_musa_audio_el;
var p_musa_isloaded = false;

function musa_init() {
	p_musa_audio_el = new Audio("data/biisi.mp3" );
	p_musa_audio_el.addEventListener("loadeddata", function() {
		p_musa_isloaded = true;
		file_loadedResources += 1;
	});
	p_musa_audio_el.load();
	file_totalResources += 1;
}

function musa_start() {
	p_musa_audio_el.play();
}

function musa_isloaded() {
	return p_musa_isloaded;
}


