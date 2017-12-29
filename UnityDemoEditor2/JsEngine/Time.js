
var dt = 0;
var lt = 0;
var t_this_frame = 0;
function time_frameChanged() {
	t_this_frame = p_musa_audio_el.currentTime;
	lt = t();
}


function set_t(t) {
	return p_musa_audio_el.currentTime = t;
}

function t() {
//	return p_musa_audio_el.currentTime;
	return t_this_frame;
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
