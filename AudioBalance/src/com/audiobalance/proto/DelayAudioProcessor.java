package com.audiobalance.proto;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

import android.util.Log;

public class DelayAudioProcessor implements AudioVideoPlayer.AudioPlayerProcessor {
	
	private static final String LOG_TAG = "DelayAudioProcessor";
	
	public static final int MAX_INPUT_BUFFER_LENGTH = 200; 
	
	private float delay;
	
	public DelayAudioProcessor() {
		inputBuffer = new ArrayList<Sample>();
	}
	
	public void setDelay(float delay) {
		this.delay = delay;
	}
	
	ArrayList<Sample> inputBuffer;
	
	private class Sample {
		short L;
		short R;
	}
	
	byte [] ret = null;
	short [] shorts = null;
			
	public byte [] process(byte [] input, int sampleRate) {
		final int maxSampleCount = MAX_INPUT_BUFFER_LENGTH * sampleRate / 1000;
		try {
			if (shorts == null || shorts.length != input.length / 2) {
				shorts = new short[input.length / 2];
			}
			ByteBuffer.wrap(input).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().get(shorts);
			for(int i = 0; i < shorts.length; i += 2) {
				Sample sample = new Sample();
				sample.L = shorts[i+0];
				sample.R = shorts[i+1];
				inputBuffer.add(sample);
			}
			
			if (inputBuffer.size() > maxSampleCount) {				
				inputBuffer.subList(0, inputBuffer.size() - maxSampleCount).clear();
			}
			final int offsetInSamples = (int)(this.delay * sampleRate / 1000);
			int loffset = offsetInSamples > 0 ? -offsetInSamples : 0;
			int roffset = offsetInSamples < 0 ?  offsetInSamples : 0;
			//int loffset = 0, roffset = 0;
			
			if (ret == null || ret.length != shorts.length * 2) {
				ret = new byte[shorts.length * 2];
			}
			
			int N = 0;
			for(int i = inputBuffer.size() - input.length/4; i < inputBuffer.size(); i++) {
				if (i + loffset < 0) {
					shorts[N*2+0] = 0;
				} else if (i + loffset < inputBuffer.size()) {
					shorts[N*2+0] = inputBuffer.get(i + loffset).L;
				}
				if (i + roffset < 0) {
					shorts[N+2+1] = 0;
				} else if (i + roffset < inputBuffer.size()) {
					shorts[N*2+1] = inputBuffer.get(i + roffset).R;
				}
				N++;
			}
			ByteBuffer.wrap(ret).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().put(shorts);
			return ret;
		} catch(Exception e) {
			Log.e(LOG_TAG, "Got exception: sampleRate = " + sampleRate + ", input.length = " + input.length);
			e.printStackTrace();
			return null;
		}
	}
	
}
