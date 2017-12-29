package com.audiobalance.proto;

import android.app.Activity;
import android.content.SharedPreferences;
import android.util.Log;

public class CalibrationParameters {
	
	public static final float DelayMin = -50;
	public static final float DelayMax = 50;
	
	public static final float GainMin = -10;
	public static final float GainMax = 40;
	
	public static final int [] Frequencies = {
		250, 500, 700, 1000, 1500, 2000, 3000, 4000, 6000, 8000	
	};
	public static final float [] ReferenceGain = {
		15.0f, 12.5f, 10.0f, 5.0f, 0.0f, -5.0f, -7.5f, -7.5f, -5.0f, 0.0f	
	};

	public float channelDelay;
	public float [] frequencyIntensities = new float[Frequencies.length];
	
	public enum ResearchPlayOrder {
		AB, AC, RANDOM, SPEECH
	}	
	public String researchSubjectId = "";	
	public ResearchPlayOrder researchPlayOrder = ResearchPlayOrder.RANDOM;
	
	public float adjustVolume = 0.0f;
	public int systemVolume = 10;
	
	public boolean playStaticTonesInCalibration = false;
	public int leftEarFrequency = 400;
	public int rightEarFrequency = 1000;
	public int impulseFrequency = 5;
	
	public CalibrationParameters() {
	}

	public CalibrationParameters(CalibrationParameters a) {
		this.channelDelay = a.channelDelay;
		this.frequencyIntensities = new float[a.frequencyIntensities.length];
		for (int i = 0; i < a.frequencyIntensities.length; i++) {
			this.frequencyIntensities[i] = a.frequencyIntensities[i]; 
		}
		
		if (AppState.ResearchVersion) {
			this.researchSubjectId = a.researchSubjectId;
			this.researchPlayOrder = a.researchPlayOrder;
			this.adjustVolume = a.adjustVolume;
			this.systemVolume = a.systemVolume;
		}
	}
	
	public void normalizeIntensities() {
		/*
		float avg = 0;
		for (float g : frequencyIntensities) {
			avg += g;
		}
		avg /= frequencyIntensities.length;
		for (int i = 0; i < frequencyIntensities.length; i++) {
			frequencyIntensities[i] -= avg;
		}
		*/
	}
	
	public void save(Activity activity) {
		SharedPreferences settings = activity.getSharedPreferences("calibration_params", 0);
		SharedPreferences.Editor editor = settings.edit();
		editor.putFloat("delay", this.channelDelay);
		for(int i = 0; i < Frequencies.length; i++) {
			editor.putFloat("freq_" + Frequencies[i], this.frequencyIntensities[i]);
		}
		if (AppState.ResearchVersion) {
			editor.putString("research_subject_id", this.researchSubjectId);
			editor.putString("research_play_order", this.researchPlayOrder.toString());
			editor.putFloat("research_adjust_volume", this.adjustVolume);
			editor.putInt("research_system_volume", this.systemVolume);
		}
		editor.commit();
	}
	
	public static CalibrationParameters load(Activity activity) {
		SharedPreferences settings = activity.getSharedPreferences("calibration_params", 0);
		
		CalibrationParameters params = new CalibrationParameters();
		params.channelDelay = settings.getFloat("delay", 0.0f);
		for(int i = 0; i < Frequencies.length; i++) {
			params.frequencyIntensities[i] = settings.getFloat("freq_" + Frequencies[i], 0.0f);
		}
		if (AppState.ResearchVersion) {
			params.researchSubjectId = settings.getString("research_subject_id", "");
			params.researchPlayOrder = ResearchPlayOrder.valueOf(settings.getString("research_play_order", "RANDOM"));
			params.adjustVolume = settings.getFloat("research_adjust_volume", 0.0f);
			params.systemVolume = settings.getInt("research_system_volume", 10);
			Log.d("CalibrationParamaters", String.format("Loaded adjustVolume=%.2f and systemVolume=%d", params.adjustVolume, params.systemVolume));
		}
		
		return params;
	}
	
}
