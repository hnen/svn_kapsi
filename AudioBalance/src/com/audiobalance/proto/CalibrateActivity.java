package com.audiobalance.proto;


import android.support.v4.app.Fragment;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.os.Bundle;

public class CalibrateActivity extends ActionBarActivity {
	
	FreqState [] frequencyCalibrationResult;
	boolean delayAccepted;
	float delayValue;
	
	public static final Class<?>[] calibrateFragments = {
		CalibrateInfoFragment.class,
		CalibrateDelayFragment.class,
		CalibrateFrequencyFragment.class,
		CalibrateSummaryFragment.class
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {		
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_calibrate);
		
		if (savedInstanceState == null) {
			showInfoFragment();
		}
	}
	
	private void showInfoFragment() {
		
		String introText = AppState.ResearchVersion ? getString(R.string.text_calibrate_intro_research) : getString(R.string.text_calibrate_intro);
		
		CalibrateInfoFragment infoFragment = new CalibrateInfoFragment(
			getString(R.string.caption_calibrate_intro), introText,
			new Runnable() {			
				@Override
				public void run() {
					if (!AppState.ResearchVersion) {
						showDelayCalibrationInfo();
					} else {
						showDelayCalibrateFragment();
					}
			}
		});
		getSupportFragmentManager().beginTransaction().add(R.id.calibration_fragment_view, infoFragment).commit();
		setProgressIndicator(1);
	}
	
	private void showDelayCalibrationInfo() {
		CalibrateInfoFragment infoFragment = new CalibrateInfoFragment(
				getString(R.string.caption_calibrate_delay), getString(R.string.text_calibrate_delay),
				new Runnable() {
					@Override
					public void run() {
						showDelayCalibrateFragment();
				}
		});
		replaceFragment(infoFragment);
		setProgressIndicator(2);
	}
	
	private void showDelayCalibrateFragment() {
		CalibrateDelayFragment delayFragment = new CalibrateDelayFragment(new CalibrateDelayFragment.OnCalibrationReadyListener() {
			@Override
			public void onReady(boolean confirmed, float value) {
				Log.d("CalibrateActivity", "delay ready, confirmed: " + confirmed + ", value: " + value);
				delayAccepted = confirmed;
				delayValue = value;
				if (confirmed) {
					saveDelayValue(value);
				}
				if (AppState.DisableFrequency) {
					showSummaryFragment();				
				} else {
					if (!AppState.ResearchVersion) {
						showFrequencyCalibrateInfo();
					} else {
						showSummaryFragment();
					}
				}
			}
		});
		replaceFragment(delayFragment);
		setProgressIndicator(3);		
	}
	
	private void showFrequencyCalibrateInfo() {
		CalibrateInfoFragment frequencyInfoFragment = new CalibrateInfoFragment(
				getString(R.string.caption_calibrate_frequency), getString(R.string.text_calibrate_frequency),
				new Runnable() {
					@Override
					public void run() {
						showFrequencyCalibrateFragment();
					}
		});
		replaceFragment(frequencyInfoFragment);
		setProgressIndicator(4);		
	}
	
	private void showFrequencyCalibrateFragment() {
		CalibrateFrequencyFragment frequencyFragment = new CalibrateFrequencyFragment(
				new CalibrateFrequencyFragment.OnFreqCalibrationReadyListener() {
					@Override
					public void onReady(FreqState [] result) {
						frequencyCalibrationResult = result;
						if (result != null) {
							saveFreqResult(result);
						} else {
							if(AppState.DevMode) {
								//frequencyCalibrationResult = createDummyResult();
							}
						}
						showSummaryFragment();
					}
				});
		replaceFragment(frequencyFragment);
		setProgressIndicator(5);
	}
	
	public static FreqState [] createDummyResult() {
		FreqState [] ret = new FreqState[CalibrationParameters.Frequencies.length];
		for (int i = 0; i < ret.length; i++) {
			ret[i] = new FreqState();
			ret[i].Frequency = CalibrationParameters.Frequencies[i];
			float a = (float)Math.random() * (CalibrationParameters.GainMax - CalibrationParameters.GainMin) + CalibrationParameters.GainMin;
			float b = (float)Math.random() * (CalibrationParameters.GainMax - CalibrationParameters.GainMin) + CalibrationParameters.GainMin;
			if (a<b) {
				ret[i].LowerLimit = a;
				ret[i].UpperLimit = b;				
			} else {
				ret[i].LowerLimit = b;
				ret[i].UpperLimit = a;								
			}
		}
		return ret;
	}
	
	void saveDelayValue(float value) {
		AppState.activeParameters.channelDelay = value;
		AppState.activeParameters.save(this);
	}
	
	void saveFreqResult(FreqState [] state) {
		float median = FreqState.ComputeMedianOffset(state);
		for(int i = 0; i < state.length; i++) {
			AppState.activeParameters.frequencyIntensities[i] = state[i].getFreqFixForResult(-median);
		}
		AppState.activeParameters.normalizeIntensities();
		
		AppState.activeParameters.save(this);
	}
	
	private void showSummaryFragment() {
		CalibrateSummaryFragment summaryFragment = new CalibrateSummaryFragment(new Runnable() {
			@Override
			public void run() {
				finish();
			}
		}, delayAccepted, delayValue, frequencyCalibrationResult);
		replaceFragment(summaryFragment);
		setProgressIndicator(6);
	}
	
	private void replaceFragment(Fragment fragmentInstance) {
		getSupportFragmentManager()
		.beginTransaction()
		.setCustomAnimations(R.anim.push_left_in,  R.anim.push_left_out, R.anim.push_right_in, R.anim.push_right_out)
		.replace(R.id.calibration_fragment_view, fragmentInstance)
		.addToBackStack(null)
		.commit();
	}

	private void setProgressIndicator(int progress) {
		TextView progressText = (TextView)this.findViewById(R.id.textview_calibrate_progress);
		progressText.setText("" + progress + "/6");
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
    	overridePendingTransition(R.anim.push_right_in, R.anim.push_right_out);
	}
	
	public class CalibrateInfoFragment extends Fragment {
		
		private Runnable nextPressed;
		private CharSequence captionText;
		private CharSequence infoText;
		
		public CalibrateInfoFragment(CharSequence captionText, CharSequence infoText, Runnable nextPressed) {
			this.nextPressed = nextPressed;
			this.captionText = captionText;
			this.infoText = infoText;
		}
		
		public View onCreateView(LayoutInflater inflater, ViewGroup container,
	            Bundle savedInstanceState) {
			View view = inflater.inflate(R.layout.fragment_calibrate_info, container, false);
			
			Button b = (Button)view.findViewById(R.id.button_fragment_calibrate_info_next);
			b.setOnClickListener(new OnClickListener() {				
				@Override
				public void onClick(View v) {
					nextPressed.run();
				}
			});
			
			TextView captionElement = (TextView)view.findViewById(R.id.textview_calibrate_info_caption);
			TextView textElement = (TextView)view.findViewById(R.id.textview_calibrate_info_text);
			
			captionElement.setText(captionText);
			textElement.setText(infoText);
			
			return view;
		}
	}


}
