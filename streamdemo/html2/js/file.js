
var file_loadedResources = 0;
var file_totalResources = 0;

function file_loadText(filename, onready) {
	var xmlhttp = new XMLHttpRequest();
	var done = false;
	xmlhttp.addEventListener("load", function () {
		if ( xmlhttp.responseText && !done )
		{
			onready( xmlhttp.responseText );
			done = true;
			file_loadedResources += 1;
		}
	} );

	xmlhttp.open( "GET", filename, true );
	xmlhttp.send();
	file_totalResources += 1;
}

function file_loadBin(filename, onready) {
	var xmlhttp = new XMLHttpRequest();
	var done = false;
	xmlhttp.addEventListener("load", function () {
		if (!done && xmlhttp.response) {
			onready(new Uint8Array(xmlhttp.response));
			done = true;
			file_loadedResources += 1;
		}
	} );

	xmlhttp.open("GET", filename, true);
	xmlhttp.responseType = "arraybuffer";
	xmlhttp.send();
	file_totalResources += 1;
}

