
var p_musa_audio_el;
var p_musa_isloaded = false;


function getSpectrum(  index )
{
	if ( index > spectrum_size )
	{
		console.log( "out of spectrum " + index);
		return 0;
	}
	var time = p_musa_audio_el.currentTime
	var specTime = time * 1000 / spectrum_step_ms;

	var m = spectrum_length-1;
	specTime = Math.min( specTime, m );
	var s1 = Math.floor(specTime);
	var s2 = Math.min( s1+1, m );
	var d = specTime - s1;
	return (spectrum[index][s1] * (1-d) + spectrum[index][s2] * d) / 1000;
}

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


