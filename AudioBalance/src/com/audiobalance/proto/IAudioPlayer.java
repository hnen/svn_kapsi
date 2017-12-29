package com.audiobalance.proto;

import java.io.IOException;

import android.app.Activity;

public interface IAudioPlayer {
	public boolean isFinished();
	public void play(Activity activity) throws IOException;
	public void setDelay(float delay);
	public void stop();
}
