package com.audiobalance.proto;

import java.util.Arrays;

class FreqState {
	public int Frequency;
	public float UpperLimit;
	public float LowerLimit;
	
	public float getFreqEstimate(float offset) {
		return (UpperLimit + LowerLimit) * 0.5f + offset;
	}
	public float getFreqFixForResult(float offset) {
		int i = -1;
		for (int j = 0; j < CalibrationParameters.Frequencies.length; j++) {
			if (CalibrationParameters.Frequencies[j] == Frequency) {
				i = j;
				break;
			}
		}
		
		float freq = getFreqEstimate(offset);
		float reference = CalibrationParameters.ReferenceGain[i];
		
		return -(freq-reference) * 4.0f / 10.0f;
	}
	
	public static float ComputeMedianOffset(FreqState [] result) {
		float [] offset = new float[result.length];
		
		float avg = 0;
		for(int i = 0; i < result.length; i++) {
			offset[i] = result[i].getFreqEstimate(0) - CalibrationParameters.ReferenceGain[i];
			avg += offset[i];
		}
		avg /= result.length;
		
		Arrays.sort(offset);
		
		/*
		if (offset.length%2 == 0) {
			return offset[offset.length/2];
		} else {
			return (offset[offset.length/2] + offset[offset.length/2+1]) * 0.5f;
		}
		*/
		return avg;
	}	
}