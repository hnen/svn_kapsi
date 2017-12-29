
var Time = {
    get time() { return t(); },
    get smoothDeltaTime() { if (!dt) { return 0.0; } else { return dt; } }
};


UnityEngine.Time = Time;
