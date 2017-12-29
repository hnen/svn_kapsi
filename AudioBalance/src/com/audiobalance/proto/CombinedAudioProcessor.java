package com.audiobalance.proto;

import android.util.Log;

public class CombinedAudioProcessor implements AudioVideoPlayer.AudioPlayerProcessor {
	
	private EqualizerAudioProcessor eq;
	private DelayAudioProcessor delay;
	private VolumeAudioProcessor volume;
	
	private boolean eqEnabled;
	
	public CombinedAudioProcessor(float [] frequencyIntensities, float Q, int sampleRate) {
		
		Log.d("CombinedAudioProcessor", "Disabling frequency adjustments...");
		for (int i = 0; i < frequencyIntensities.length; i++) {
			frequencyIntensities[i] = 0;
		}
		
		eq = new EqualizerAudioProcessor(frequencyIntensities, Q, sampleRate);
		this.delay = new DelayAudioProcessor();
		this.volume = new VolumeAudioProcessor();
		this.eqEnabled = true;
	}
	
	public void setDelay(float delay) {
		this.delay.setDelay(delay);
	}
	
	public void setVolume(float volume) {
		this.volume.setIntensityDb(volume);
	}
	
	public void setEqualizerEnabled(boolean set) {
		this.eqEnabled = set;
	}
	
	@Override
	public byte[] process(byte[] input, int sampleRate) {
		if (!AppState.ResearchVersion && this.eqEnabled) {
			input = eq.process(input, sampleRate);
		}
		if (AppState.ResearchVersion) {
			input = volume.process(input, sampleRate);
		}
		return delay.process(input, sampleRate);
		//return input;
	}

}
