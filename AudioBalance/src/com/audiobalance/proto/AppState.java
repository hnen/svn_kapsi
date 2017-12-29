package com.audiobalance.proto;

public class AppState {

	public static final boolean ResearchVersion = false;
	public static final boolean DisableFrequency = true;
	public static final boolean DevBuild = false;
	
	
	public static String [] filesPlayedLast;
	
	
	public static CalibrationParameters activeParameters = new CalibrationParameters();
	public static boolean DevMode = false;
	
}
