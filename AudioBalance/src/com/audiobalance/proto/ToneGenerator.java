package com.audiobalance.proto;

import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class ToneGenerator {

	public static final int SAMPLE_RATE = 44100;
	public static final float MAX_INTENSITY = CalibrationParameters.GainMax;

	private AudioTrack audioTrack = null;
	private AudioManager am = null;
	private int activeVolume = -1;
	private int previousVolume = -1;
	
	private long startTime;
	private long totalDuration;
	
	private Handler restoreVolumeHandler = new Handler() {
		public void handleMessage(Message msg) {
			//Log.d("ToneGenerator", "refreshVolumeHandler set previous volume");
			//am.setStreamVolume(AudioManager.STREAM_MUSIC, previousVolume, 0);
		}
	};
	
	private Handler refreshVolumeHandler = new Handler() {
		public void handleMessage(Message msg) {
			if (audioTrack != null) {
				if (System.currentTimeMillis() - startTime < totalDuration) {
					refreshVolumeHandler.sendMessageDelayed(Message.obtain(), 500);					
					if (activeVolume <= 1) {
						am.setStreamVolume(AudioManager.STREAM_MUSIC, activeVolume-1, 0);
						am.setStreamVolume(AudioManager.STREAM_MUSIC, activeVolume, 0);
					}
				} else {
					restoreVolumeHandler.sendMessageDelayed(Message.obtain(), 600);
				}
			}
		}
	};
	
	
	public void PlaySound(Context ctx, float frequency, float intensity, int length) {

		am = (AudioManager)ctx.getSystemService(Context.AUDIO_SERVICE);
		//am.setStreamVolume(AudioManager.STREAM_MUSIC, 1, 0);
		
		int Vdmax = am.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
		float Vdb = intensity;
		float Vmin = CalibrationParameters.GainMin + 30.0f;
		float Vd = (float)Math.ceil( /*1.0f +*/ (Vdb - Vmin) * (Vdmax-1) / (CalibrationParameters.GainMax - Vmin));
		if (Vd < 1) Vd = 1;
		float Vdbr = (Vd)/(Vdmax-1) * (CalibrationParameters.GainMax - Vmin) + Vmin;
		float Vs = (float)Math.pow(10.0, (intensity - Vdbr) / 10);
		
		startTime = System.currentTimeMillis();
		totalDuration = length;
		
		previousVolume = am.getStreamVolume(AudioManager.STREAM_MUSIC);
		am.setStreamVolume(AudioManager.STREAM_MUSIC, 0, 0);
		activeVolume = (int)Vd; 
		am.setStreamVolume(AudioManager.STREAM_MUSIC, (int)Vd, 0);
		am.setParameters("noise_suppression=0");
		am.setParameters("media.a1026.enableA1026=0");
		refreshVolumeHandler.sendMessageDelayed(Message.obtain(), 500);
		byte [] rawSound = GenerateRawSoundLinearIntensity(frequency, Vs, length);
		Log.d("ToneGenerator", "Generated " + rawSound.length + " bytes of sound.");
		audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, SAMPLE_RATE, AudioFormat.CHANNEL_OUT_MONO,
				AudioFormat.ENCODING_PCM_16BIT, rawSound.length, AudioTrack.MODE_STATIC);
		audioTrack.write(rawSound, 0, rawSound.length);
		audioTrack.play();
	}
	
	private static byte [] GenerateRawSound(float frequency, float intensity, int length) {
		double magnitude = 1.0 * Math.pow(10.0, (intensity - MAX_INTENSITY) / 10.0);
		return GenerateRawSoundLinearIntensity(frequency, magnitude, length);
	}
	
	public static byte [] GenerateRawSoundLinearIntensity(float frequency, double magnitude, int length) {
		//double magnitude = 1.0;
		int sampleCount = SAMPLE_RATE * length / 1000;
		Log.d("ToneGenerator", "Generating " + (sampleCount * 2) + " bytes of sound. Gain " + magnitude + "x, linear gain: " + magnitude + " -> " + (magnitude * 32767) );
		double [] wave = new double[sampleCount];
		byte [] data = new byte[sampleCount * 2];
		GenerateRawSoundLinearIntensitySampleCount(frequency, magnitude, true, 0, sampleCount, wave, data);
		return data;
	}
			
	public static void GenerateRawSoundLinearIntensitySampleCount(float frequency, double magnitude, boolean doFalloff, int offsetInSamples, int sampleCount, double [] wave, byte [] data) {
		int falloff = SAMPLE_RATE * 100 / 1000;
	
		for (int i = 0; i < sampleCount; i++) {
			wave[i] = Math.sin(frequency * 2.0 * Math.PI * (i + offsetInSamples) / SAMPLE_RATE) * magnitude;
			if (doFalloff) {
				if (i < falloff) {
					wave[i] *= (double)i / falloff;
				}
				if (i > sampleCount - falloff) {
					wave[i] *= (double)(sampleCount - i) / falloff;
				}
			}
		}
		
		for (int i = 0; i < sampleCount; i++) {
			int sample = (int)((wave[i]) * 32767);
			data[i*2+0] = (byte)(sample & 0x00ff); 
			data[i*2+1] = (byte)((sample & 0xff00) >> 8);
		}
		
	}
	
}
