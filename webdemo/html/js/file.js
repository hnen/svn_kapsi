function file_loadText( filename, onready ) {

	var xmlhttp = new XMLHttpRequest();
	var done = false;
	xmlhttp.onreadystatechange = function () {
		if ( xmlhttp.responseText && !done )
		{
			onready( xmlhttp.responseText );
			done = true;
		}
	};

	xmlhttp.open( "GET", filename, true );
	xmlhttp.send();

}
