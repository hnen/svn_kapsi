package com.audiobalance.proto;

import java.io.FileDescriptor;
import android.content.Intent;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.View;
import android.widget.ImageButton;
import android.widget.SeekBar;
import android.widget.TextView;

public class MediaPlayerActivity extends ActionBarActivity implements SurfaceHolder.Callback {

	@Override
	protected void onCreate(Bundle savedInstanceState) {		
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.activity_mediaplayer);
		
		ImageButton buttonFf = (ImageButton)this.findViewById(R.id.button_mediaplayer_ff);
		ImageButton buttonRewind = (ImageButton)this.findViewById(R.id.button_mediaplayer_rewind);
		buttonFf.setVisibility(View.INVISIBLE);
		buttonRewind.setVisibility(View.INVISIBLE);
		
		init();
	}
	
	AudioVideoPlayer audioPlayer;
	MediaFormat format;
	ImageButton buttonPlay;
	TextView textTime;
	SeekBar seekbarTime;
	MediaExtractor mediaExtractor;
	MediaFile mediaFile;
	boolean draggingSeekbar;
	
	Handler updateTimeDisplayHandler = new Handler() {
		public void handleMessage(Message msg) {
			if (!draggingSeekbar) {
				updateTimeDisplay();
			}
			if (mediaExtractor.getSampleTrackIndex() == -1 || getCurrentPlaybackTime() > mediaFile.duration) {
				playbackCompleted();
			} else
			if (audioPlayer != null && audioPlayer.isPlaying()) {
				updateTimeDisplayHandler.sendMessageDelayed(Message.obtain(), 50);
			}
		}
	};
	
	
	void init() {		
		buttonPlay = (ImageButton)this.findViewById(R.id.button_mediaplayer_play);
		buttonPlay.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				playPressed();
			}
		});
		
		seekbarTime = (SeekBar)this.findViewById(R.id.seekbar_mediaplayer_position);
		textTime = (TextView)this.findViewById(R.id.textview_mediaplayer_position);
		
		seekbarTime.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {			
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				draggingSeekbar = false;
				seekToSeekbarTime();
			}
			
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				draggingSeekbar = true;
			}
			
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				updateTimeDisplayFromSeekbar();
			}
		});
		
		PlayHistoryManager.addEntry(getPath(), this);
		loadSong(MediaFile.create(getPath(), this));
	}
	
	void seekToSeekbarTime() {
		int time = getSeekbarValueAsMs();
		Log.d("MediaPlayerActivity", "MEDIA EXTRACTOR SEEKTO");
		mediaExtractor.seekTo(time * 1000, MediaExtractor.SEEK_TO_CLOSEST_SYNC);
	}
	
	int getCurrentPlaybackTime() {
		return (int)(mediaExtractor.getSampleTime() / 1000);
	}
	
	void updateTimeDisplay() {
		if (textTime != null && mediaExtractor != null) {
			int currentTime = (int)(mediaExtractor.getSampleTime() / 1000);
			int totalTime = this.mediaFile.duration;
			textTime.setText(String.format("%s / %s", formatTime(currentTime), formatTime(totalTime)));
			
			seekbarTime.setMax(1000);
			if (totalTime > 0) {
				seekbarTime.setProgress(1000 * currentTime / totalTime);
			}
		}
	}
	
	void updateTimeDisplayFromSeekbar() {
		int totalTime = this.mediaFile.duration;
		int currentTime = getSeekbarValueAsMs(); 
		textTime.setText(String.format("%s / %s", formatTime(currentTime), formatTime(totalTime)));
	}
	
	int getSeekbarValueAsMs() {
		int totalTime = this.mediaFile.duration;
		int currentTime = seekbarTime.getProgress() * totalTime / 1000; 
		return currentTime;
	}
	
	String formatTime(int durationMs) {
		  int seconds = durationMs / 1000;
		  int minutes = seconds / 60;
		  int hours = minutes / 60;
		  if (hours > 0) {
			  return String.format("%d:%02d:%02d", hours, minutes % 60, seconds % 60);
		  } else {
			  return String.format("%02d:%02d", minutes, seconds % 60);				  
		  }
	}
	
	
	@Override
	public void onStop() {
		super.onStop();
		audioPlayer.stop();
	}
	
	void playPressed() {
		if (!audioPlayer.isPlaying()) {
			buttonPlay.setImageDrawable(getResources().getDrawable(R.drawable.mediaplayer_pause));
			if (audioPlayer.isPaused()) {
				audioPlayer.pause();				
			} else {
				try {
					audioPlayer.play(this);
				} catch(Exception e) {
					Log.e("MediaPlayerActivity", "Error playing the file");
				}
			}
			updateTimeDisplayHandler.sendMessageDelayed(Message.obtain(), 500);
		} else {
			buttonPlay.setImageDrawable(getResources().getDrawable(R.drawable.mediaplayer_continue));
			audioPlayer.pause();
			updateTimeDisplay();
		}
	}
	
	void playbackCompleted() {
		buttonPlay.setImageDrawable(getResources().getDrawable(R.drawable.mediaplayer_play));
		//Log.d("MediaPlayerActivity", "MEDIA EXTRACTOR RELEASE");
		//mediaExtractor.release();
		audioPlayer.stop();
		loadSong(this.mediaFile);
	}
	
	void loadSong(MediaFile mf) {		
		this.mediaFile = mf;
		TextView nameText = (TextView)this.findViewById(R.id.textview_mediaplayer_title);
		nameText.setText(mf.formatTitle());
		
		MediaExtractor me = new MediaExtractor();
		try {
			Log.d("MediaPlayerActivity", "Playing path " + mf.path);
			if (mf.path.startsWith("asset:")) {
				FileDescriptor fd = mf.fileInputStream.getFD();
				Log.d("MediaPlayerActivity", "FileDescriptor valid: " + fd.valid());
				me.setDataSource(fd);
			} else {
				me.setDataSource(mf.path);
			}
		} catch(Exception e) {
			Log.e("MediaPlayerActivity", String.format("couldn't open media file %s", mf.path));
			e.printStackTrace();
			return;
		}
		
		//me.seekTo(timeUs, mode)
		this.mediaExtractor = me;
		
		Log.d("MediaPlayerActivity", "Track Count: " + this.mediaExtractor.getTrackCount());
		
		
		float [] gains = new float[CalibrationParameters.Frequencies.length];
		for (int i = 0; i < gains.length; i++) {
			gains[i] = 0;
		}
		
		//audioPlayer = new AudioPlayer(this, me, AppState.activeParameters.channelDelay, AppState.activeParameters.frequencyIntensities);
		audioPlayer = new AudioVideoPlayer(this, me, AppState.activeParameters.channelDelay, AppState.activeParameters.frequencyIntensities);
		
		updateTimeDisplay();
	}
	
	String getPath() {
		//return this.getIntent().getStringExtra("path");
		Intent intent = this.getIntent();
		Bundle extras = intent.getExtras();
		if (extras != null && extras.containsKey("use_asset") && extras.getBoolean("use_asset")) {
			return extras.getString("asset_path");
		} else {
			return this.getIntent().getData().getPath();
		}
	}
	
	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		
	}
		


}
