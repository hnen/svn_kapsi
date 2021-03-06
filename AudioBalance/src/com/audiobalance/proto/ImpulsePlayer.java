package com.audiobalance.proto;

import java.io.IOException;
import java.nio.ByteBuffer;
import android.app.Activity;
import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.media.MediaPlayer;
import android.os.AsyncTask;
import android.util.Log;

public class ImpulsePlayer implements IAudioPlayer {

	private final String LOG_TAG = "ImpulsePlayer";

	private float delay;
	private MediaPlayer player;

	protected AudioTrack audioTrack;
	protected int inputBufIndex;
	protected Boolean doStop = false;

	protected int bufIndexCheck;
	protected CombinedAudioProcessor processor;
	protected Activity activity;
	
	protected boolean pause;
	protected boolean playing;
	
	
	protected int frequency;

	public ImpulsePlayer(Context ctx, int frequency, float delay, float [] gains) {
		this.delay = delay;
		this.processor = new CombinedAudioProcessor(gains, 1.414f, 44100);
		
		// currently equalizer is 1-channel so it destroys the frequency from other ear
		processor.setEqualizerEnabled(false);
		this.frequency = frequency;
				
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
		return doStop;
	}
	
	public boolean isPlaying() {
		return !pause && playing;
	}

	public boolean isPaused() {
		return pause;
	}
	
	public void pause() {
		this.pause = !this.pause;
	}
	
	public void play(Activity activity) throws IOException {
		this.activity = activity;
		bufIndexCheck = 0;
		new ToneGenerateOperation(processor).executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);		
		doStop = false;
		playing = true;
	}

	public void setDelay(float delay) {
		this.delay = delay;
		this.processor.setDelay(delay);
	}
	
	public void setVolume(float volume) {
		this.processor.setVolume(volume);
	}

	public void stop() {
		doStop = true;
	}

	protected class ToneGenerateOperation extends AsyncTask<Void, Void, Void> {

		private AudioVideoPlayer.AudioPlayerProcessor processor;
		
		public ToneGenerateOperation(AudioVideoPlayer.AudioPlayerProcessor processor) {
			this.processor = processor;
		}
		
		@Override
		protected Void doInBackground(Void... values) {
			Log.d("DecodeOperation", "START decode operation");
			ImpulsePlayer.this.decodeLoop(this.processor);
			return null;
		}

		@Override
		protected void onPreExecute() {
		}

		@Override
		protected void onProgressUpdate(Void... values) {
		}
	}

	private void decodeLoop(AudioVideoPlayer.AudioPlayerProcessor processor) {

		ByteBuffer[] codecInputBuffers;
		ByteBuffer[] codecOutputBuffers;

		int sampleRate = 44100;

		int bufSize = AudioTrack.getMinBufferSize(sampleRate,
				AudioFormat.CHANNEL_OUT_STEREO,
				AudioFormat.ENCODING_PCM_16BIT);
		
		// create our AudioTrack instance
		audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRate,
				AudioFormat.CHANNEL_OUT_STEREO, AudioFormat.ENCODING_PCM_16BIT,
				bufSize, AudioTrack.MODE_STREAM);

		
		// start playing, we will feed you later
		audioTrack.play();
		

		// start decoding
		final long kTimeOutUs = 10000;
		int noOutputCounter = 0;
		int noOutputCounterLimit = 50;

		int writtenFrames = 0;

		final byte[] chunk = new byte[bufSize];
		final double [] imd = new double[bufSize/4]; 
		final byte [] left = new byte[bufSize/2];
		final byte [] right = new byte [bufSize/2];
		while (!doStop) {
			if (pause) {
				try {
					Thread.sleep(25);
					continue;
				} catch(InterruptedException e) {
				}
			}

			
			fillWithImpulse(chunk, this.frequency, 44100, writtenFrames);
			
			if (chunk.length > 0) {
				final byte [] processedChunk = processor.process(chunk, sampleRate);
				//final byte [] processedChunk = chunk;
				if (processedChunk == null || audioTrack == null) {
					doStop = true;
					break;
				} else {
					audioTrack.write(processedChunk, 0, processedChunk.length);
					writtenFrames += chunk.length / 4;
				}
			}
			/*
			// Wait until there's less than 1/10 sec left in buffer to play
			while (writtenFrames - audioTrack.getPlaybackHeadPosition() > 441000 / 2) {
				Log.d(LOG_TAG, "waiting for buffer to fill... writtenFrames=" + writtenFrames + ", playbackHeadPosition=" + audioTrack.getPlaybackHeadPosition());
				try {
					Thread.sleep(100);
					continue;
				} catch(InterruptedException e) {
				}
			}
			*/
			//Log.d(LOG_TAG, "writing more data to buffer");
			
			
		}

		Log.d(LOG_TAG, "stopping...");

		relaxResources(true);

		// this.mState = State.Stopped;
		doStop = true;


	}
	
	private void fillWithImpulse(byte [] buffer, int freq, int rate, int offsetInSamples) {
		/*
		for (int i = 0; i < sampleCount; i++) {
			int sample = (int)((wave[i]) * 32767);
			data[i*2+0] = (byte)(sample & 0x00ff); 
			data[i*2+1] = (byte)((sample & 0xff00) >> 8);
		}
		*/
		
		float pulseWidthMs = 0.2f;
		int pulseWidthInSamples = (int)(rate * pulseWidthMs / 1000);
		//int pulseWidthInSamples = 100;
		int pulseFreqInSamples = rate / freq;
		for(int i = 0; i < buffer.length; i+=4) {
			int sampleI = ((i/4) + offsetInSamples) % pulseFreqInSamples;
			int sample = -32767;
			if (sampleI < pulseWidthInSamples) {
				sample = 32767;
			}
			buffer[i+0] = (byte)(sample & 0xff);
			buffer[i+1] = (byte)((sample & 0xff00) >> 8);
			buffer[i+2] = buffer[i+0];
			buffer[i+3] = buffer[i+1];
		}
	}

	private void relaxResources(Boolean release) {
		if (audioTrack != null) {
			audioTrack.flush();
			audioTrack.release();
			audioTrack = null;
		}
	}

}
