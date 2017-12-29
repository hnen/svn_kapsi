package com.audiobalance.proto;

import java.io.IOException;

import com.audiobalance.proto.CalibrationParameters.ResearchPlayOrder;

import android.media.MediaExtractor;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class CalibrateDelayFragment extends Fragment {		
	
	interface OnCalibrationReadyListener {
		void onReady(boolean confirmed, float value);
	}
	
	private OnCalibrationReadyListener onReady;
	
	private IAudioPlayer samplePlayer;
	
	TextView textviewDebugValue;
	CalibrationWheel wheel;
	
	boolean stopped = false;
	
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
	
	public CalibrateDelayFragment(OnCalibrationReadyListener onReady) {
		this.onReady = onReady;
	}
	
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
		View view =inflater.inflate(R.layout.fragment_calibrate_delay, container, false);
		
		Button b = (Button)view.findViewById(R.id.button_calibrate_delay_skip);
		if (AppState.DevBuild) {
			b.setOnClickListener(new OnClickListener() {				
				@Override
				public void onClick(View v) {
					onReady.onReady(false, 0);
				}
			});
		} else {
			b.setVisibility(Button.INVISIBLE);
		}
		
		Button b1 = (Button)view.findViewById(R.id.button_calibrate_delay_accept);
		b1.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				onReady.onReady(true, getDelayValue());
			}
		});
		
		textviewDebugValue = (TextView)view.findViewById(R.id.textview_calibrate_delay_debug_delayvalue);
		wheel = (CalibrationWheel)view.findViewById(R.id.wheel_calibrate_delay);
		wheel.setSensitivity(0.2f);
		
		wheel.setOnValueChangedListener(new CalibrationWheel.OnValueChangedListener() {
			@Override
			public void onValueChanged(CalibrationWheel wheel) {
				onWheelValueChanged(wheel);
			}
		});
		
		if (AppState.DevMode) {
			textviewDebugValue.setVisibility(View.VISIBLE);
		} else {
			textviewDebugValue.setVisibility(View.INVISIBLE);
		}
	
		onWheelValueChanged(wheel);

		if (AppState.ResearchVersion) {
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
			
			Button skipButton = (Button)view.findViewById(R.id.button_calibrate_delay_skip);
			skipButton.setVisibility(Button.INVISIBLE);
			
		} else {
			playFiles = new String [] { "steve_jobs_01.mp3", "steve_jobs_02.mp3", "steve_jobs_03.mp3", "steve_jobs_04.mp3", "steve_jobs_05.mp3",
					"pamela_meyer_01.mp3", "pamela_meyer_02.mp3", "pamela_meyer_03.mp3", "pamela_meyer_04.mp3", "pamela_meyer_05.mp3" };
		}
		
		checkPlayNextHandler.sendMessageDelayed(Message.obtain(), 50);
		
		return view;
	}
	
	
	private void playFile() throws IOException {

		float [] gains = new float[CalibrationParameters.Frequencies.length];
		for (int i = 0; i < gains.length; i++) {
			gains[i] = 0;
		}
		
		if (samplePlayer != null) {
			samplePlayer.stop();
		}
		
		if (!AppState.activeParameters.playStaticTonesInCalibration) {
			MediaExtractor extractor = new MediaExtractor();
	
			nextFile = (int)(Math.random() * playFiles.length);
			
			String fileName = playFiles[nextFile%playFiles.length];
			//nextFile++;
			
			Log.d("CalibrateDelayFragment", "Playing " + fileName);
			
			try {
				//AssetFileDescriptor afd = this.getActivity().getAssets().openFd(fileName);
				//FileDescriptor fd = afd.getFileDescriptor();
				MediaFile mf = MediaFile.create("asset:" + fileName, this.getActivity());
				extractor.setDataSource(mf.fileInputStream.getFD());
			} catch (Exception e) {
				e.printStackTrace();
				Log.e("CalibrateDelayFragment", "Problem opening file.");
			}
			
			samplePlayer = new AudioVideoPlayer(this.getActivity(), extractor, getDelayValue(), gains);
		} else {
			//samplePlayer = new TonePlayer(this.getActivity(), AppState.activeParameters.leftEarFrequency, AppState.activeParameters.rightEarFrequency, getDelayValue(), gains);
			samplePlayer = new ImpulsePlayer(this.getActivity(), AppState.activeParameters.impulseFrequency, getDelayValue(), gains);
		}
		samplePlayer.play(this.getActivity());
		samplePlayer.setDelay(getDelayValue());

	}
	
	@Override
	public void onDestroyView() {
		super.onDestroyView();
		stopped = true;
		samplePlayer.stop();
	}
	
	private void onWheelValueChanged(CalibrationWheel wheel) {
		UpdateDebugValue();
		if (samplePlayer != null) {
			samplePlayer.setDelay(getDelayValue());
		}
	}
	
	private float getDelayValue() {
		return wheel.getValue() * (CalibrationParameters.DelayMax - CalibrationParameters.DelayMin) + CalibrationParameters.DelayMin;
	}
	
	private void UpdateDebugValue() {
		textviewDebugValue.setText(String.format("%.1fms", getDelayValue()));
	}
	
}