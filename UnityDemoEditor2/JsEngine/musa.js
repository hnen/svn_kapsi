
function musa_load(fileName, onReady) {
    p_musa_audio_el = new Audio(fileName);
    p_musa_audio_el.addEventListener("loadeddata", function() {
        onReady(p_musa_audio_el)
    });
    p_musa_audio_el.load();
}

function musa_start(song) {
    song.play();
}
