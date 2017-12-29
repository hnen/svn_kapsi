package com.audiobalance.proto;

import java.io.IOException;

import com.audiobalance.proto.CalibrationParameters.ResearchPlayOrder;

import android.content.Context;
import android.media.AudioManager;
import android.media.MediaExtractor;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

public class CalibrateVolumeActivity extends ActionBarActivity {

	boolean stopped;

	private static final float MIN_ADJUSTMENT = -20.0f;
	
	SeekBar seekBarAdjustment;
	TextView textViewAdjustment;
	
	SeekBar seekBarSystemVolume;	
	TextView textViewSystemVolume;
	
	private AudioVideoPlayer samplePlayer;
	
	CalibrationParameters editParameters;
	
	AudioManager am;

	Handler checkPlayNextHandler = new Handler() {
		public void handleMessage(Message msg) {
			if (stopped) {
				return;
			}
			if (samplePlayer == null || samplePlayer.isFinished()) {
				try {
					playFile();
				} catch(Exception e) {
					Log.e("CalibrateDelayFragment", "Error while playing the file.");
				}
			}
			checkPlayNextHandler.sendMessageDelayed(Message.obtain(), 50);
		}
	};
	int nextFile = 0;	
	String [] playFiles;
	
	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.activity_calibrate_volume);
		
		Log.d("CalibrateVolumeActivity", String.format("Has adjustVolume=%.2f and systemVolume=%d", AppState.activeParameters.adjustVolume, AppState.activeParameters.systemVolume));
		
		editParameters = new CalibrationParameters(AppState.activeParameters);
		
		Log.d("CalibrateVolumeActivity", String.format("EditParamsadjustVolume=%.2f and systemVolume=%d", editParameters.adjustVolume, editParameters.systemVolume));		
				
		seekBarAdjustment = (SeekBar) this.findViewById(R.id.seekbar_calibrate_volume_adjust_volume);
		seekBarAdjustment.setMax(1000);
		float currAdjustment = AppState.activeParameters.adjustVolume;
		float max = seekBarAdjustment.getMax();
		seekBarAdjustment.setProgress((int)((1.0f - currAdjustment / MIN_ADJUSTMENT) * max));
		seekBarAdjustment.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}

			@Override
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				adjustmentChanged();
			}
		});
		
		am = (AudioManager)this.getSystemService(AUDIO_SERVICE);
		int maxVolume = am.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
		seekBarSystemVolume = (SeekBar) this.findViewById(R.id.seekbar_calibrate_volume_system_volume);
		seekBarSystemVolume.setMax(maxVolume);
		seekBarSystemVolume.setProgress(AppState.activeParameters.systemVolume);
		seekBarSystemVolume.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}

			@Override
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				systemVolumeChanged();
			}
		});
		

		Button saveButton = (Button)this.findViewById(R.id.button_calibrate_volume_save);
		saveButton.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				AppState.activeParameters = new CalibrationParameters(CalibrateVolumeActivity.this.editParameters);
		    	AppState.activeParameters.save(CalibrateVolumeActivity.this);
				checkPlayNextHandler.removeMessages(0);
				stopped = true;		    	
		    	CalibrateVolumeActivity.this.finish();				
			}
		});
		
		textViewAdjustment = (TextView)this.findViewById(R.id.textview_calibrate_volume_adjust_volume);
		textViewSystemVolume = (TextView)this.findViewById(R.id.textview_calibrate_volume_system_volume);
		
		adjustmentChanged();
		systemVolumeChanged();
	
		ResearchPlayOrder rpo = AppState.activeParameters.researchPlayOrder;
		if (rpo == ResearchPlayOrder.RANDOM) {
			rpo = Math.random() > 0.5 ? ResearchPlayOrder.AB : ResearchPlayOrder.AC;
		}
		if (rpo == ResearchPlayOrder.AB) {
			playFiles = new String [] { "list1.mp3", "list2.mp3" };
		} else if (rpo == ResearchPlayOrder.AC) {
			playFiles = new String [] { "list1.mp3", "list3.mp3" };
		} else if (rpo == ResearchPlayOrder.SPEECH) {
			playFiles = new String [] { "speech2.mp3" };
		}
		AppState.filesPlayedLast = playFiles;
		checkPlayNextHandler.sendMessageDelayed(Message.obtain(), 50);		
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		checkPlayNextHandler.removeMessages(0);
		stopped = true;
		samplePlayer.stop();
	}

	private void adjustmentChanged() {
		textViewAdjustment.setText(String.format("%.2fdB", getCurrentAdjustmentIntensity()));
		if (this.samplePlayer != null) {
			this.samplePlayer.setVolume(getCurrentAdjustmentIntensity());
		}
		editParameters.adjustVolume = getCurrentAdjustmentIntensity();
	}
	
	private void systemVolumeChanged() {
		textViewSystemVolume.setText(String.format("%d", seekBarSystemVolume.getProgress()));
		editParameters.systemVolume = seekBarSystemVolume.getProgress();
		AudioManager am = (AudioManager)this.getSystemService(Context.AUDIO_SERVICE);
		am.setStreamVolume(AudioManager.STREAM_MUSIC, editParameters.systemVolume, 0);
	}

	float getCurrentAdjustmentIntensity() {
		return -(MIN_ADJUSTMENT * (((float) seekBarAdjustment.getProgress()) / seekBarAdjustment.getMax()) - MIN_ADJUSTMENT);
	}
	
	private void playFile() throws IOException {
		MediaExtractor extractor = new MediaExtractor();

		String fileName = playFiles[nextFile%playFiles.length];
		nextFile++;
		
		Log.d("CalibrateVolumeActivity", "Playing " + fileName);
		
		try {
			MediaFile mf = MediaFile.create("asset:" + fileName, this);
			extractor.setDataSource(mf.fileInputStream.getFD());
		} catch (Exception e) {
			e.printStackTrace();
			Log.e("CalibrateVolumeActivity", "Problem opening file.");
		}

		float [] gains = new float[CalibrationParameters.Frequencies.length];
		for (int i = 0; i < gains.length; i++) {
			gains[i] = 0;
		}
		
		if (samplePlayer != null) {
			samplePlayer.stop();
		}
		
		samplePlayer = new AudioVideoPlayer(this, extractor, 0, gains);
		samplePlayer.play(this);
		samplePlayer.setVolume(this.getCurrentAdjustmentIntensity());

	}
	

}
