package com.audiobalance.proto;

import android.support.v7.app.ActionBarActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.TextView;


public class MainActivity extends ActionBarActivity {
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        if (AppState.DevBuild) {
        	setContentView(R.layout.activity_main);
        } else {
        	setContentView(R.layout.activity_main_release);
        }
        
        if (savedInstanceState == null) {
        	AppState.activeParameters = CalibrationParameters.load(this);
        	setDeveloperMode(false);
        }
        
        CheckBox devCheckbox = (CheckBox)this.findViewById(R.id.checkbox_developer_mode);
        
        if (AppState.ResearchVersion) {
        	TextView versionText = (TextView)this.findViewById(R.id.textview_versiontext);
        	versionText.setText(versionText.getText() + " - Modified for research purproses");
        }
        
        if (AppState.ResearchVersion) {
        	devCheckbox.setVisibility(CheckBox.INVISIBLE);
        	Button devDashboardButton = (Button)this.findViewById(R.id.button_open_developer_dashboard);
        	devDashboardButton.setVisibility(Button.VISIBLE);
        	devDashboardButton.setText("Researcher Dashboard");
        	Button devVolumeTesterbutton = (Button)this.findViewById(R.id.button_main_volumetester);
        	devVolumeTesterbutton.setVisibility(Button.VISIBLE);
        	devVolumeTesterbutton.setText("Volume Calibration");
        } else {
        	if (AppState.DevBuild) {
		        devCheckbox.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener() {
					@Override
					public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
						setDeveloperMode(isChecked);
					}
				});
		        devCheckbox.setChecked(AppState.DevMode);
		        setDeveloperMode(AppState.DevMode);
        	}
        } 
    }
    
    private void setDeveloperMode(boolean set) {
    	if (AppState.DevBuild) {
	    	Button devDashboardButton = (Button)this.findViewById(R.id.button_open_developer_dashboard);
	    	Button devVolumeTesterbutton = (Button)this.findViewById(R.id.button_main_volumetester);
	    	if (set) {
	    		devDashboardButton.setVisibility(Button.VISIBLE);
	    		devVolumeTesterbutton.setVisibility(Button.VISIBLE);
	    	} else {
	    		devDashboardButton.setVisibility(Button.INVISIBLE);
	    		devVolumeTesterbutton.setVisibility(Button.INVISIBLE);
	    	}
	    	AppState.DevMode = set;
    	}
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
    
    public void openCalibrationActivity(View view) {
    	Intent openCalibrationIntent = new Intent(this, CalibrateActivity.class);
    	startActivity(openCalibrationIntent);
    	overridePendingTransition(R.anim.push_left_in, R.anim.push_left_out);
    }
    
    public void openVolumeTester(View view) {
    	if (AppState.ResearchVersion) {
	    	Intent openVolumetesterIntent = new Intent(this, CalibrateVolumeActivity.class);
	    	startActivity(openVolumetesterIntent);
    	} else {
	    	Intent openVolumetesterIntent = new Intent(this, TestVolumeActivity.class);
	    	startActivity(openVolumetesterIntent);
    	}
    	overridePendingTransition(R.anim.push_left_in, R.anim.push_left_out);    		
    }
    
    public void openDevDashboard(View view) {
    	Intent openDashboardIntent = new Intent(this, DashboardActivity.class);
    	startActivity(openDashboardIntent);
    	overridePendingTransition(R.anim.push_left_in, R.anim.push_left_out);    	
    }

    public void openAudioSelector(View view) {
    	Intent openAudioSelectorIntent = new Intent(this, AudioSelectorActivity.class);
    	startActivity(openAudioSelectorIntent);
    	overridePendingTransition(R.anim.push_left_in, R.anim.push_left_out);
    }
}
