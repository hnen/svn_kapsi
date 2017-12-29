package com.audiobalance.proto;

import com.audiobalance.proto.CalibrationParameters.ResearchPlayOrder;

import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.SeekBar;
import android.widget.TextView;

public class DashboardActivity extends ActionBarActivity {
	
	private CalibrationParameters dashboardParameters;
	private ICalibrationParameterChangeListener calibrationListener;
	
	static float MIN_GAIN = CalibrationParameters.GainMin - 20;
	static float MAX_GAIN = CalibrationParameters.GainMax;
	
    DashboardSelectPlayOrderFragment researchFragment;
    DashboardDelayFragment delayFragment;
	
	public interface ICalibrationParameterChangeListener {
		void OnDelayChange(float delay);
		void OnFrequencyIntensityChange(int frequency, float intensity);
	}
	
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d("DashboardActivity", "onCreate");
        this.dashboardParameters = new CalibrationParameters(AppState.activeParameters);
        setContentView(R.layout.activity_developer);
        this.calibrationListener = new CalibrationParameterChangeListener(dashboardParameters);
        createDelayView(calibrationListener);
        if (AppState.ResearchVersion) {
        	createResearchUI();
        } else {
        	createFrequencyList(calibrationListener);
        }
    }
    
    public void resetValues(View view) {
    	this.dashboardParameters.channelDelay = 0;
		delayFragment.seekBar.setProgress( (int)((0 - CalibrationParameters.DelayMin) / (CalibrationParameters.DelayMax - CalibrationParameters.DelayMin) * delayFragment.seekBar.getMax()) );
    }
    
    public void saveAndExit(View view) {
    	AppState.activeParameters = new CalibrationParameters(this.dashboardParameters);
    	if (AppState.ResearchVersion) {
        	EditText et = (EditText)findViewById(R.id.edittext_dashboard_testsubject_id);
    		AppState.activeParameters.researchSubjectId = et.getText().toString();
    		
    		if (researchFragment.rbRandom.isChecked()) {
    			AppState.activeParameters.researchPlayOrder = ResearchPlayOrder.RANDOM;
    		} else if (researchFragment.rbAb.isChecked()) {
    			AppState.activeParameters.researchPlayOrder = ResearchPlayOrder.AB;    			
    		} else if (researchFragment.rbAc.isChecked()) {
    			AppState.activeParameters.researchPlayOrder = ResearchPlayOrder.AC;    			
    		} else if (researchFragment.rbSpeech.isChecked()) {
    			AppState.activeParameters.researchPlayOrder = ResearchPlayOrder.SPEECH;
    		}
    	}
    	AppState.activeParameters.save(this);
    	this.finish();
    }
    
    public void normalize(View view) {
    	this.dashboardParameters.normalizeIntensities();
    }
    
    private void createDelayView(ICalibrationParameterChangeListener changeListener) {
    	DashboardDelayFragment fragment = new DashboardDelayFragment(changeListener, dashboardParameters.channelDelay);
    	this.delayFragment = fragment;
    	getSupportFragmentManager().beginTransaction().add(R.id.frame_dashboard_delay_content, fragment).commit();
    }

    
    private void createResearchUI() {
    	LinearLayout view = (LinearLayout)this.findViewById(R.id.linearlayout_dashboard);
    	
    	DashboardSelectPlayOrderFragment f1 = new DashboardSelectPlayOrderFragment();
    	findViewById(R.id.textview_dashboard_frequency_caption).setVisibility(TextView.GONE);
    	findViewById(R.id.listview_dashboard_content).setVisibility(TextView.GONE);
    	getSupportFragmentManager().beginTransaction().add(R.id.linearlayout_dashboard, f1).commit();
    	
    	//Button saveButton = (Button)findViewById(R.id.button_dashboard_save);
    	//view.removeView(saveButton);
    	//view.addView(saveButton, 2, 0);
    	
    	
    	//EditText et = (EditText)findViewById(R.id.edittext_dashboard_testsubject_id);
    	//et.setText(AppState.researchSubjectId);
    	
    	this.researchFragment = f1;
    }
    
    private void createFrequencyList(ICalibrationParameterChangeListener changeListener) {
    	DashboardFreqListItem [] items = new DashboardFreqListItem[CalibrationParameters.Frequencies.length]; 
    	for(int i = 0; i < CalibrationParameters.Frequencies.length; i++) {
    		items[i] = new DashboardFreqListItem(CalibrationParameters.Frequencies[i]);
    	}
    	
    	DashboardArrayAdapter adapter = new DashboardArrayAdapter(this.getBaseContext(), items,
    			dashboardParameters.frequencyIntensities, changeListener);
    	
    	ListView list = (ListView)this.findViewById(R.id.listview_dashboard_content);
    	list.setAdapter(adapter);
    }
    
    public class DashboardFreqListItem {
    	public int Frequency;
    	public DashboardFreqListItem(int frequency) {
    		this.Frequency = frequency;
    	}
    }

    
    
	public class DashboardDelayFragment extends Fragment {	
		private ICalibrationParameterChangeListener listener;
		private float initialValue;
		
		public SeekBar seekBar;
		
		public DashboardDelayFragment(ICalibrationParameterChangeListener listener0, float initialValue) {
			this.listener = listener0;
			this.initialValue = initialValue;
		}
		
		public View onCreateView(LayoutInflater inflater, ViewGroup container,
	            Bundle savedInstanceState) {
			View view = inflater.inflate(R.layout.listitem_dashboard_delay, container, false);
			
			SeekBar seekBar = (SeekBar)view.findViewById(R.id.seekbar_dashboard_delay);
			this.seekBar = seekBar;
			seekBar.setProgress( (int)((this.initialValue - CalibrationParameters.DelayMin) / (CalibrationParameters.DelayMax - CalibrationParameters.DelayMin) * seekBar.getMax()) );
			
			TextView valueText = (TextView)view.findViewById(R.id.textview_dashboard_delay_amount);
			DelayChangeListener changeListener = new DelayChangeListener(valueText, this.listener);
			seekBar.setOnSeekBarChangeListener(changeListener);
			changeListener.onProgressChanged(seekBar, seekBar.getProgress(), false);
			
			return view;
		}
		
		  public class DelayChangeListener implements SeekBar.OnSeekBarChangeListener {
			private ICalibrationParameterChangeListener calibrationParamChangeListener;
			private TextView valueText;
			
			public DelayChangeListener(TextView valueText, ICalibrationParameterChangeListener calibrationParamChangeListener0) {
				this.calibrationParamChangeListener = calibrationParamChangeListener0;
				this.valueText = valueText;
			}
			  
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				
				float value = ((float)progress / seekBar.getMax() * (CalibrationParameters.DelayMax - CalibrationParameters.DelayMin)) + CalibrationParameters.DelayMin;
				calibrationParamChangeListener.OnDelayChange(value);
				this.valueText.setText("" + value + "ms");
			}
	
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {}
		  }		
	}
    
	public class DashboardSelectPlayOrderFragment extends Fragment {			
		public DashboardSelectPlayOrderFragment() {
		}
		
		public RadioButton rbRandom;
		public RadioButton rbAb;
		public RadioButton rbAc;
		public RadioButton rbSpeech;
		
		public View onCreateView(LayoutInflater inflater, ViewGroup container,
	            Bundle savedInstanceState) {
			View view = inflater.inflate(R.layout.listitem_dashboard_playorder_research, container, false);
			
	    	EditText et = (EditText)view.findViewById(R.id.edittext_dashboard_testsubject_id);
	    	et.setText(AppState.activeParameters.researchSubjectId);
	    	
    		this.rbRandom = (RadioButton)view.findViewById(R.id.radiobutton_dashboard_playorder_random);
    		this.rbAb = (RadioButton)view.findViewById(R.id.radiobutton_dashboard_playorder_ab);
    		this.rbAc = (RadioButton)view.findViewById(R.id.radiobutton_dashboard_playorder_ac);
    		this.rbSpeech = (RadioButton)view.findViewById(R.id.radiobutton_dashboard_playorder_speech);
	    	if (AppState.activeParameters.researchPlayOrder == ResearchPlayOrder.RANDOM) {
	    		rbRandom.setChecked(true);
	    	} else if (AppState.activeParameters.researchPlayOrder == ResearchPlayOrder.AB) {
	    		rbAb.setChecked(true);
	    	} else if (AppState.activeParameters.researchPlayOrder == ResearchPlayOrder.AC) {
	    		rbAc.setChecked(true);
	    	} else if (AppState.activeParameters.researchPlayOrder == ResearchPlayOrder.SPEECH) {
	    		rbSpeech.setChecked(true);
	    	}
			
			return view;
		}
	}
	
	
    public class DashboardArrayAdapter extends ArrayAdapter<DashboardFreqListItem> {
		  private final Context context;
		  private final DashboardFreqListItem[] values;
		  private float [] initialFrequencies;
		  private ICalibrationParameterChangeListener listener;

		  public DashboardArrayAdapter(Context context, DashboardFreqListItem[] values, float [] initialFrequencies, ICalibrationParameterChangeListener listener) {
		    super(context, R.layout.listitem_dashboard_frequency, values);
		    this.context = context;
		    this.values = values;
		    this.initialFrequencies = initialFrequencies;
		    this.listener = listener;
		  }
		  
		  @Override
		  public View getView(int position, View convertView, ViewGroup parent) {
			    LayoutInflater inflater = (LayoutInflater) context
			            .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			    View rowView = inflater.inflate(R.layout.listitem_dashboard_frequency, parent, false);
			    initView(position, rowView, position);
			    return rowView;			  
		  }
		  
		  private void initView(int i, View rowView, int position) {
			    TextView fileText = (TextView)rowView.findViewById(R.id.textview_dashboard_frequency_intensity_frequency);
			    int freq = CalibrationParameters.Frequencies[position];
			    fileText.setText("" + freq + " Hz");			    
			    SeekBar freqSeekBar = (SeekBar)rowView.findViewById(R.id.seekbar_dashboard_frequency_intensity);
			    freqSeekBar.setProgress((int)(((this.initialFrequencies[i] - MIN_GAIN) / (MAX_GAIN - MIN_GAIN)) * freqSeekBar.getMax()));
			    TextView valueText = (TextView)rowView.findViewById(R.id.textview_dashboard_frequency_intensity_amount);
			    
			    FrequencyChangeListener freqListener = new FrequencyChangeListener(valueText, freq, listener);
			    freqSeekBar.setOnSeekBarChangeListener(freqListener);
			    freqListener.onProgressChanged(freqSeekBar, freqSeekBar.getProgress(), false);
		  }
		  
		  public class FrequencyChangeListener implements SeekBar.OnSeekBarChangeListener {
				private ICalibrationParameterChangeListener calibrationParamChangeListener;
				private int frequency;
				private TextView valueText;
				public FrequencyChangeListener(TextView valueText, int frequency, ICalibrationParameterChangeListener calibrationParamChangeListener) {
					this.calibrationParamChangeListener = calibrationParamChangeListener;
					this.frequency = frequency;
					this.valueText = valueText;
				}
				  
				@Override
				public void onProgressChanged(SeekBar seekBar, int progress,
						boolean fromUser) {
					float value = ((float)progress / seekBar.getMax() * (MAX_GAIN - MIN_GAIN)) + MIN_GAIN;
					calibrationParamChangeListener.OnFrequencyIntensityChange(this.frequency, value);
					if (value > 0) {
						this.valueText.setText("+" + value + "dB");
					} else {
						this.valueText.setText("" + value + "dB");						
					}
				}

				@Override
				public void onStartTrackingTouch(SeekBar seekBar) {}
				@Override
				public void onStopTrackingTouch(SeekBar seekBar) {}
		  }
		  
    }
    

    public class CalibrationParameterChangeListener implements ICalibrationParameterChangeListener {
    	
    	private CalibrationParameters destParameters;
    	
    	public CalibrationParameterChangeListener(CalibrationParameters destParameters) {
    		this.destParameters = destParameters;
    	}
    	
    	public void OnDelayChange(float delay) {
			this.destParameters.channelDelay = delay;
		}
    	
		public void OnFrequencyIntensityChange(int frequency, float intensity) {
			int I = -1;
			for(int i = 0; i < this.destParameters.frequencyIntensities.length; i++) {
				if (CalibrationParameters.Frequencies[i] == frequency) {
					I = i;
					break;
				}
			}
			if (I >= 0) {
				this.destParameters.frequencyIntensities[I] = intensity;
			}
		}
    }    
    
}
