package com.audiobalance.proto;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import android.util.Log;

public class EqualizerAudioProcessor implements AudioVideoPlayer.AudioPlayerProcessor {

	private Biquad [] filters;
	
	float [] frequencyIntensities;
	float Q;
	int sampleRate;
	
	public EqualizerAudioProcessor(float [] frequencyIntensities, float Q, int sampleRate) {
		this.Q = Q;
		this.frequencyIntensities = frequencyIntensities;
		this.sampleRate = sampleRate;
		initFilters();
	}
	
	private void initFilters() {
		filters = new Biquad[frequencyIntensities.length];
		for (int i = 0; i < frequencyIntensities.length; i++) {
			float gain = frequencyIntensities[i];
			float freq = CalibrationParameters.Frequencies[i];
			Log.i("Eq", String.format("gain @ %.0f = %.1f", freq, gain));
			filters[i] = new Biquad(freq, sampleRate, Q, gain);
		}		
	}
	
	@Override
	public byte[] process(byte[] input, int sampleRate) {
		if (this.sampleRate != sampleRate) {
			this.sampleRate = sampleRate;
			initFilters();
		}
		short [] shorts = new short[input.length / 2];
		ByteBuffer.wrap(input).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().get(shorts);
		for(int i = 0; i < shorts.length; i += 2) {
			double sample = (double)shorts[i] / 32767; 
			for (int j = 0; j < filters.length; j++) {
				sample = filters[j].process(sample);
			}
			//sample = filters[0].process(sample);
			sample = Math.min(sample, 1.0);
			sample = Math.max(sample,-1.0);
			short nsample = (short)(sample * 32767);
			shorts[i] = nsample;
			shorts[i+1] = nsample;
		}
		ByteBuffer.wrap(input).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().put(shorts);
		return input;
	}

	
	
}
