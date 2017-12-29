package com.audiobalance.proto;

import java.nio.ByteBuffer;

import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.os.PowerManager;
import android.util.Log;

import com.audiobalance.proto.AudioVideoPlayer.AudioPlayerProcessor;

public class AudioDecoder {
	
	protected MediaCodec codec;
	protected AudioTrack audioTrack;
	
	protected Boolean doStop = false;
	protected boolean pause;
	protected boolean playing;
	
	protected int inputBufIndex;
	protected int bufIndexCheck;
	
	private static final String LOG_TAG = "AudioDecoder";
	
	public AudioDecoder() {
		bufIndexCheck = 0;
	}
	
	public boolean isFinished() {
		return doStop;
	}
	
	public boolean isPaused() {
		return pause;
	}
	
	public boolean isPlaying() {
		return !pause && playing;
	}
	
	public void pause() {
		this.pause = !this.pause;
	}

	public void stop() {
		doStop = true;
	}

	public void runAudioDecodeLoop(MediaExtractor extractor, AudioPlayerProcessor processor) {

		doStop = false;
		playing = true;
				
		ByteBuffer[] codecInputBuffers;
		ByteBuffer[] codecOutputBuffers;

		int track = extractor.getTrackCount() - 1;
		for (int i = 0; i < extractor.getTrackCount(); i++) {
			MediaFormat format = extractor.getTrackFormat(i);
			String mime = format.getString(MediaFormat.KEY_MIME);
			if (mime.startsWith("audio/")) {
				track = i;
				break;
			}
		}

		MediaFormat format = extractor.getTrackFormat(track);
				
		String mime = format.getString(MediaFormat.KEY_MIME);
		
		Log.d(LOG_TAG, "TRACK COUNT: " + extractor.getTrackCount());			

		// the actual decoder
		try {
			codec = MediaCodec.createDecoderByType(mime);
		} catch (Exception e) {
			Log.e(LOG_TAG, "Problem creating decoder.");
			return;
		}
				
		codec.configure(format, null /* surface */, null /* crypto */, 0 /* flags */);
	
		codec.start();
		codecInputBuffers = codec.getInputBuffers();
		codecOutputBuffers = codec.getOutputBuffers();

		// get the sample rate to configure AudioTrack
		int sampleRate = format.getInteger(MediaFormat.KEY_SAMPLE_RATE);

		Log.i(LOG_TAG, "mime " + mime);
		Log.i(LOG_TAG, "sampleRate " + sampleRate);

		// create our AudioTrack instance
		audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRate,
				AudioFormat.CHANNEL_OUT_STEREO, AudioFormat.ENCODING_PCM_16BIT,
				AudioTrack.getMinBufferSize(sampleRate,
						AudioFormat.CHANNEL_OUT_STEREO,
						AudioFormat.ENCODING_PCM_16BIT), AudioTrack.MODE_STREAM);
		
		// start playing, we will feed you later
		audioTrack.play();
				
		extractor.selectTrack(track);

		// start decoding
		final long kTimeOutUs = 10000;
		MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
		boolean sawInputEOS = false;
		boolean sawOutputEOS = false;
		int noOutputCounter = 0;
		int noOutputCounterLimit = 50;
		
		
		while (!sawOutputEOS && noOutputCounter < noOutputCounterLimit
				&& !doStop) {
			if (pause) {
				try {
					Thread.sleep(25);
					continue;
				} catch(InterruptedException e) {
				}
			}
			//Log.i(LOG_TAG, "loop ");
			noOutputCounter++;
			if (!sawInputEOS) {

				inputBufIndex = codec.dequeueInputBuffer(kTimeOutUs);
				bufIndexCheck++;
				// Log.d(LOG_TAG, " bufIndexCheck " + bufIndexCheck);
				if (inputBufIndex >= 0) {
					ByteBuffer dstBuf = codecInputBuffers[inputBufIndex];

					int sampleSize = extractor
							.readSampleData(dstBuf, 0 /* offset */);

					long presentationTimeUs = 0;

					if (sampleSize < 0) {
						Log.d(LOG_TAG, "saw input EOS.");
						sawInputEOS = true;
						sampleSize = 0;
					} else {
						presentationTimeUs = extractor.getSampleTime();
					}
					// can throw illegal state exception (???)

					codec.queueInputBuffer(inputBufIndex, 0 /* offset */,
							sampleSize, presentationTimeUs,
							sawInputEOS ? MediaCodec.BUFFER_FLAG_END_OF_STREAM
									: 0);

					if (!sawInputEOS) {
						extractor.advance();
					}
				} else {
					Log.e(LOG_TAG, "inputBufIndex " + inputBufIndex);
				}
			} 

			int res = codec.dequeueOutputBuffer(info, kTimeOutUs);

			if (res >= 0) {
				// Log.d(LOG_TAG, "got frame, size " + info.size + "/" +
				// info.presentationTimeUs);
				if (info.size > 0) {
					noOutputCounter = 0;
				}

				int outputBufIndex = res;
				ByteBuffer buf = codecOutputBuffers[outputBufIndex];

				final byte[] chunk = new byte[info.size];
				buf.get(chunk);
				buf.clear();
				if (chunk.length > 0) {
					final byte [] processedChunk = processor.process(chunk, sampleRate);
					//final byte [] processedChunk = chunk;
					if (processedChunk == null || audioTrack == null) {
						doStop = true;
						break;
					} else {
						audioTrack.write(processedChunk, 0, processedChunk.length);
					}
					// if(this.mState != State.Playing)
					// {
					// mDelegateHandler.onRadioPlayerPlaybackStarted(MP3RadioStreamPlayer.this);
					// }
					// this.mState = State.Playing;
				}
				codec.releaseOutputBuffer(outputBufIndex, false /* render */);
				if ((info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
					Log.d(LOG_TAG, "saw output EOS.");
					sawOutputEOS = true;
				}
			} else if (res == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
				codecOutputBuffers = codec.getOutputBuffers();

				Log.d(LOG_TAG, "output buffers have changed.");
			} else if (res == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
				MediaFormat oformat = codec.getOutputFormat();

				Log.d(LOG_TAG, "output format has changed to " + oformat);
			} else {
				Log.d(LOG_TAG, "dequeueOutputBuffer returned " + res);
			}
		}

		
		Log.d(LOG_TAG, "stopping...");

		relaxResources(true);

		// this.mState = State.Stopped;
		doStop = true;

		// attempt reconnect
		if (sawOutputEOS) {
			// try {
			// MP3RadioStreamPlayer.this.play();
			// return;
			// } catch (IOException e) {
			// TODO Auto-generated catch block
			// e.printStackTrace();
			// }
		}

		if (noOutputCounter >= noOutputCounterLimit) {
			// mDelegateHandler.onRadioPlayerError(MP3RadioStreamPlayer.this);
		} else {
			// mDelegateHandler.onRadioPlayerStopped(MP3RadioStreamPlayer.this);
		}
	}

	private void relaxResources(Boolean release) {
		if (codec != null) {
			if (release) {
				codec.stop();
				codec.release();
				codec = null;
			}

		}
		if (audioTrack != null) {
			audioTrack.flush();
			audioTrack.release();
			audioTrack = null;
		}
	}
}
