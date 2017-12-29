package com.audiobalance.proto;

import android.content.Context;
import android.media.AudioManager;
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


public class CalibrateFrequencyFragment extends Fragment {
	
	public static final int TUNE_PAUSE_MIN = 1000;
	public static final int TUNE_PAUSE_MAX = 3000;
	public static final int TUNE_LENGTH = 2000;
	
	public static final int MIN_PUSH_TIME = 500;
	public static final float MAX_UNCERTAINTY = 1.0f;
	
	boolean stopped = false;
	
	AudioManager am;
	
	public interface OnFreqCalibrationReadyListener {
		void onReady(FreqState [] result);
	}
	
	private OnFreqCalibrationReadyListener onReady;
	View currentView;
	private CalibrationButton button;


	int savedVolume;
	
	public CalibrateFrequencyFragment(OnFreqCalibrationReadyListener onReady) {
		this.onReady = onReady;
	}
	
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
		currentView = inflater.inflate(R.layout.fragment_calibrate_frequency, container, false);

		am = (AudioManager)this.getActivity().getSystemService(Context.AUDIO_SERVICE);
		savedVolume = am.getStreamVolume(AudioManager.STREAM_MUSIC);

		stopped = false;
		AudioManager am = (AudioManager)this.getActivity().getSystemService(Context.AUDIO_SERVICE);
		am.setParameters("noise_suppression=off");
		am.requestAudioFocus(new AudioManager.OnAudioFocusChangeListener() {			
			@Override
			public void onAudioFocusChange(int focusChange) {
				Log.e("CalbrateFreq", "Audio focus lost lol");
			}
		} , AudioManager.STREAM_MUSIC, AudioManager.AUDIOFOCUS_GAIN);
		am.setStreamSolo(AudioManager.STREAM_MUSIC, true);
		Log.d("CalibrateFreq", am.getParameters("noise_suppression"));
		
		Button b = (Button)currentView.findViewById(R.id.button_calibration_frequency_skip);
		b.setOnClickListener(new OnClickListener() {				
			@Override
			public void onClick(View v) {
				onReady.onReady(null);
			}
		});
		
		button = (CalibrationButton)currentView.findViewById(R.id.button_calibrate_frequency_calibrationbutton);
		button.setOnPushListener(new CalibrationButton.OnPushListener() {
			@Override
			public void onPush(boolean down) {
				onPushCalibrationButton(down);
			}
		});
		
		InitFreqCalibState();
		
		TextView debugText1 = (TextView)currentView.findViewById(R.id.textview_calibrate_frequency_debug_currentestimates);
		TextView debugText2 = (TextView)currentView.findViewById(R.id.textview_calibrate_frequency_debug_status);
		
		if(AppState.DevMode) {
			debugText1.setVisibility(TextView.VISIBLE);
			debugText2.setVisibility(TextView.VISIBLE);
		} else {
			debugText1.setVisibility(TextView.INVISIBLE);
			debugText2.setVisibility(TextView.INVISIBLE);			
		}
		
		return currentView;
	}
	
	enum CalibrationState {
		WAITING, PLAYING, DONE
	}
	
	FreqState [] freqStates;
	int currentFrequencyN;
	float currentIntensity;
	CalibrationState state;
	long pushTime;
	boolean pushedWhilePlaying;
	boolean acceptPush;
	
	Handler startPlaySoundHandler = new Handler() {
		public void handleMessage(Message msg) {
			if (stopped) return;
			FreqState currState = freqStates[currentFrequencyN];
			ToneGenerator nGen = new ToneGenerator();
			nGen.PlaySound(getActivity(), currState.Frequency, currentIntensity, TUNE_LENGTH);
			setCalibrationState(CalibrationState.PLAYING);
			stopPlaySoundHandler.sendMessageDelayed(Message.obtain(), TUNE_LENGTH);
		}
	};
	Handler stopPlaySoundHandler = new Handler() {
		public void handleMessage(Message msg) {
			if (stopped) return;
			if (button.isDown() && System.currentTimeMillis() - pushTime < MIN_PUSH_TIME) {
				stopPlaySoundHandler.sendMessageDelayed(Message.obtain(), 200);
			} else {
				setCalibrationState(CalibrationState.DONE);
				startNextFreqHandler.sendMessageDelayed(Message.obtain(), 200);
			}
		}
	};
	Handler startNextFreqHandler = new Handler() {
		public void handleMessage(Message msg) {
			if (stopped) return;
			if (button.isDown()) {
				startNextFreqHandler.sendMessageDelayed(Message.obtain(), 200);
			} else {
				adjustFrequencyEstimate(acceptPush);
				startNewFreq();
			}
		}
	};
	
	@Override
	public void onDestroyView() {
		super.onDestroyView();
		startNextFreqHandler.removeMessages(0);
		startPlaySoundHandler.removeMessages(0);
		stopped = true;
		am.setStreamVolume(AudioManager.STREAM_MUSIC, this.savedVolume, 0);
	}
	
	void adjustFrequencyEstimate(boolean acceptInput) {
		FreqState s = freqStates[currentFrequencyN];
		//s.UpperLimit -= 0.5f;
		if (acceptInput) {
			s.UpperLimit = currentIntensity;
		} else {
			s.LowerLimit = currentIntensity;			
		}
	}
	
	void onPushCalibrationButton(boolean down) {
		if (down) {
			if (this.state == CalibrationState.PLAYING) {
				pushTime = System.currentTimeMillis();
				pushedWhilePlaying = true;
			}
		} else {
			if (pushedWhilePlaying) {
				pushedWhilePlaying = false;
				int pushLength = (int)(System.currentTimeMillis() - pushTime);
				if (pushLength >= MIN_PUSH_TIME) {
					acceptPush = true;
				}
			}
		}
	}
	
	
	private void InitFreqCalibState() {
		freqStates = new FreqState[CalibrationParameters.Frequencies.length];
		for(int i = 0; i < freqStates.length; i++) {
			freqStates[i] = new FreqState();
			freqStates[i].Frequency = CalibrationParameters.Frequencies[i];
			freqStates[i].LowerLimit = CalibrationParameters.GainMin;
			freqStates[i].UpperLimit = CalibrationParameters.GainMax;
		}
		currentFrequencyN=0;
		startNewFreq();
	}
	

	private void startNewFreq() {
		if (allFrequenciesAccurateEnough()) {
			onReady.onReady(freqStates);
		} else {
			if (frequencyAccurateEnough(freqStates[currentFrequencyN])) {
				currentFrequencyN++;
			}
			currentFrequencyN %= freqStates.length;
			FreqState s = freqStates[currentFrequencyN];
			currentIntensity = (s.LowerLimit + s.UpperLimit) * 0.5f;
			setCalibrationState(CalibrationState.WAITING);
			startPlaySoundHandler.sendMessageDelayed(Message.obtain(), pauseTime());
			pushedWhilePlaying = false;
			acceptPush = false;
		}
	}
	
	private int pauseTime() {
		return (int)(Math.random() * (TUNE_PAUSE_MAX - TUNE_PAUSE_MIN)) + TUNE_PAUSE_MIN;
	}
	
	private boolean allFrequenciesAccurateEnough() {
		for(int i = 0; i < freqStates.length; i++) {
			if(!frequencyAccurateEnough(freqStates[i])) {
				return false;
			}
		}
		return true;
	}
	
	private boolean frequencyAccurateEnough(FreqState freqState) {
		if (freqState.UpperLimit - freqState.LowerLimit < MAX_UNCERTAINTY) {
			return true;
		} else {
			return false;
		}
	}
	
	private void setCalibrationState(CalibrationState state) {
		this.state = state;
		TextView debugStatus = (TextView)currentView.findViewById(R.id.textview_calibrate_frequency_debug_status);
		debugStatus.setText(String.format("%dHz/%.1fdB %s", CalibrationParameters.Frequencies[currentFrequencyN], currentIntensity, this.state.toString()));

		TextView debugEstimates = (TextView)currentView.findViewById(R.id.textview_calibrate_frequency_debug_currentestimates);
		String estimateStr = "";
		for (int i = 0; i < freqStates.length; i++) {
			estimateStr += String.format("%dHz: %.1fdB - %.1fdB -> %.1fdB", freqStates[i].Frequency,  freqStates[i].LowerLimit, freqStates[i].UpperLimit, (freqStates[i].UpperLimit + freqStates[i].LowerLimit) * 0.5f);
			if (i < freqStates.length - 1) {
				estimateStr += "\n";
			}
		}
		debugEstimates.setText(estimateStr);
		
	}
	
	
	
}