package com.audiobalance.proto;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.ActionBarActivity;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

public class TestVolumeActivity extends ActionBarActivity {

	boolean stopped;

	private static final float TEST_FREQUENCY = 1000;
	private static final int TUNE_LENGTH = 1000;

	SeekBar seekBar;
	TextView textView;

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.activity_testvolume);
		
		seekBar = (SeekBar) this.findViewById(R.id.seekbar_testvolume);
		seekBar.setMax(1000);
		seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
			}

			@Override
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				progressChanged();
			}
		});
		
		textView = (TextView)this.findViewById(R.id.textview_testvolume);
		progressChanged();
		
		startPlaySoundHandler.sendMessageDelayed(Message.obtain(),
				TUNE_LENGTH);
		
		Button btn_up = (Button)this.findViewById(R.id.button_testvolume_finetune_up);
		Button btn_dn = (Button)this.findViewById(R.id.button_testvolume_finetune_down);

		btn_up.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				seekBar.setProgress(seekBar.getProgress()+1);
				progressChanged();
			}
		});
		btn_dn.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				seekBar.setProgress(seekBar.getProgress()-1);
				progressChanged();
			}
		});
		
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		startPlaySoundHandler.removeMessages(0);
		stopped = true;
	}

	private void progressChanged() {
		textView.setText(String.format("%.2fdB", getCurrentIntensity()));
	}

	float getCurrentIntensity() {
		return CalibrationParameters.GainMin
				+ (CalibrationParameters.GainMax - CalibrationParameters.GainMin)
				* ((float) seekBar.getProgress()) / seekBar.getMax();
	}

	Handler startPlaySoundHandler = new Handler() {
		public void handleMessage(Message msg) {
			if (stopped)
				return;

			ToneGenerator nGen = new ToneGenerator();
			nGen.PlaySound(TestVolumeActivity.this, TEST_FREQUENCY, getCurrentIntensity(),
					TUNE_LENGTH);
			stopPlaySoundHandler.sendMessageDelayed(Message.obtain(),
					TUNE_LENGTH);
		}
	};
	Handler stopPlaySoundHandler = new Handler() {
		public void handleMessage(Message msg) {
			if (stopped)
				return;
			startPlaySoundHandler.sendMessageDelayed(Message.obtain(),
					100);
		}
	};

}
