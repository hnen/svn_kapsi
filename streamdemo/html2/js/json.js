

function p_json_ischar(c) {
	return (c.charCodeAt(0) >= "a".charCodeAt(0) && c.charCodeAt(0) <= "z".charCodeAt(0)) || (c.charCodeAt(0) >= "A".charCodeAt(0) && c.charCodeAt(0) <= "Z".charCodeAt(0)) || c == "_" ;
} 
function p_json_isnumber(c) {
	return (c.charCodeAt(0) >= "0".charCodeAt(0) && c.charCodeAt(0) <= "9".charCodeAt(0));
} 

function json_parse(str) {
	var final_str = "";
	var in_str = false;
	var in_comment = false;
	for ( var i = 0; i < str.length ; i++ ) {
		var c = str.charAt(i);
		if ( !in_comment ) {
			if ( c == '/' && str.charAt(i+1) == '/' ) {
				in_comment = true;
			} else {
				if( p_json_ischar(c) && !in_str ) {
					in_str = true;
					final_str += '"';
				} 
				if( (!p_json_ischar(c) && !p_json_isnumber(c)) && in_str ) {
					in_str = false;
					final_str += '"';
				} 
				final_str = final_str + c;
			}
		} else {
			if ( c == '\n' ) {
				in_comment = false;
				final_str = final_str + c;
			}
		}
	}
	//console.log( final_str );
	return JSON.parse( final_str );
}


