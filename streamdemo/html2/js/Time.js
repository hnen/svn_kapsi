
var dt = 0;
var lt = 0;
function time_frameChanged() {
	dt = t() - lt;
	lt = t();
}

function t() {
	return p_musa_audio_el.currentTime;
//	var d = new date();
//	return d.getmilliseconds() + 1000 * 
//		(d.getseconds() + 60 *
//		(d.getminutes() + 60 *
//	 	(d.gethours() + 24 * 
//		(d.getday()))));
}

function timer() {
	var d = new Date();
	return d.getMilliseconds() + 1000 * 
		(d.getSeconds() + 60 *
		(d.getMinutes() + 60 *
	 	(d.getHours() + 24 * 
		(d.getDay()))));
}


