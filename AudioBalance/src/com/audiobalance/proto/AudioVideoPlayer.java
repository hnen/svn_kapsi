package com.audiobalance.proto;

import java.io.IOException;
import java.nio.ByteBuffer;
import android.app.Activity;
import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaCodec;
import android.media.MediaCodecList;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.media.MediaPlayer;
import android.os.AsyncTask;
import android.os.PowerManager;
import android.util.Log;


public class AudioVideoPlayer implements IAudioPlayer{

	private final String LOG_TAG = "AudioPlayer";

	protected MediaExtractor extractor;
	protected CombinedAudioProcessor processor;
	protected Activity activity;
	
	protected Context ctx;
	
	PowerManager.WakeLock wl;
	
	AudioDecodeOperation activeDecodeOperation;

	public AudioVideoPlayer(Context ctx, MediaExtractor data, float delay, float [] gains) {
		//this.delay = delay;
		this.extractor = data;
		this.processor = new CombinedAudioProcessor(gains, 1.414f, 44100);
		this.ctx = ctx;
		processor.setDelay(delay);
		
		if (AppState.ResearchVersion) {
			applyVolume(ctx, AppState.activeParameters);
		}
		
	}
	
	private void applyVolume(Context ctx, CalibrationParameters params) {
		this.setVolume(params.adjustVolume);
		AudioManager am = (AudioManager)ctx.getSystemService(Context.AUDIO_SERVICE);
		am.setStreamVolume(AudioManager.STREAM_MUSIC, params.systemVolume, 0);
	}
	
	
	public boolean isFinished() {
		return activeDecodeOperation != null ? activeDecodeOperation.ad.isFinished() : false;
	}
	
	public MediaFormat getMediaFormat() {
		return extractor.getTrackFormat(0);
	}
	
	public boolean isPlaying() {
		return activeDecodeOperation != null ? activeDecodeOperation.ad.isPlaying() : false;
	}

	public boolean isPaused() {
		return activeDecodeOperation != null ? activeDecodeOperation.ad.isPaused() : false;
	}
	
	public void pause() {
		if (activeDecodeOperation != null) {
			activeDecodeOperation.ad.pause();
		}
	}
	
	public void play(Activity activity) throws IOException {
		this.activity = activity;
		this.activeDecodeOperation = new AudioDecodeOperation(extractor, processor);
		this.activeDecodeOperation.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);

		PowerManager pm = (PowerManager)ctx.getSystemService(Context.POWER_SERVICE);
		this.wl = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "My Tag");
		wl.acquire();		
		
	}

	public void setDelay(float delay) {
		//this.delay = delay;
		this.processor.setDelay(delay);
	}
	
	public void setVolume(float volume) {
		this.processor.setVolume(volume);
	}

	public void stop() {
		if (activeDecodeOperation != null) {
			activeDecodeOperation.ad.stop();
		}
	}
	
	// TODO: Refactor this elsewhere.
	public static interface AudioPlayerProcessor {
		byte [] process(byte [] input, int sampleRate);
	}

	protected class AudioDecodeOperation extends AsyncTask<Void, Void, Void> {

		private AudioPlayerProcessor processor;
		private MediaExtractor extractor;
		
		public AudioDecoder ad;
		
		public AudioDecodeOperation(MediaExtractor extractor, AudioPlayerProcessor processor) {
			this.processor = processor;
			this.extractor = extractor;
		}
		
		@Override
		protected Void doInBackground(Void... values) {
			Log.d("AudioDecodeOperation", "START decode operation");
			this.ad = new AudioDecoder();
			ad.runAudioDecodeLoop(this.extractor, this.processor);
			wl.release();
			return null;
		}

		@Override
		protected void onPreExecute() {
		}

		@Override
		protected void onProgressUpdate(Void... values) {
		}
	}

	protected class VideoDecodeOperation extends AsyncTask<Void, Void, Void> {

		private MediaExtractor extractor;
		
		public VideoDecodeOperation(MediaExtractor extractor) {
			this.extractor = extractor;
		}
		
		@Override
		protected Void doInBackground(Void... values) {
			Log.d("VideoDecodeOperation", "START video decode operation");
			AudioVideoPlayer.this.videoDecodeLoop(this.extractor);
			return null;
		}

		@Override
		protected void onPreExecute() {
		}

		@Override
		protected void onProgressUpdate(Void... values) {
		}
	}
	
	private void videoDecodeLoop(MediaExtractor extractor) {
	}
	

}
