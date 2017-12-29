package com.audiobalance.proto;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class VolumeAudioProcessor implements AudioVideoPlayer.AudioPlayerProcessor {
	
	private static final String LOG_TAG = "VolumeAudioProcessor";
	
	public static final int MAX_INPUT_BUFFER_LENGTH = 200; 
	
	private float intensityDb;
	
	public VolumeAudioProcessor() {
	}
	
	public void setIntensityDb(float db) {
		this.intensityDb = db;
	}
	
	public byte [] process(byte [] input, int sampleRate) {
		short [] shorts = new short[input.length / 2];
		ByteBuffer.wrap(input).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().get(shorts);
		
		float x = (float)Math.pow(10.0, intensityDb / 10.0);
		for (int i = 0; i < shorts.length; i++) {
			shorts[i] = (short)(shorts[i] * x);
		}
		
		byte [] ret = new byte[shorts.length * 2];
		ByteBuffer.wrap(ret).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().put(shorts);
		return ret;
	}
	
}
