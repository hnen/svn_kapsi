package com.audiobalance.proto;

import java.util.ArrayList;

import android.content.Context;
import android.content.SharedPreferences;

public class PlayHistoryManager {

	
	public static void addEntry(String path, Context ctx) {
		SharedPreferences settings = ctx.getSharedPreferences("playback_history", 0);
		String [] ohistory = getHistory(settings);
		
		int f = -1;
		for(int i = 0; i < ohistory.length; i++) {
			if (ohistory[i].equals(path)) {
				f = i;
				break;
			}
		}
		
		if (f != -1) {
			bringToFront(settings, f);
		} else {
			append(settings, path);
			bringToFront(settings, ohistory.length);
		}
		
	}
	
	public static String [] get(Context ctx) {
		SharedPreferences settings = ctx.getSharedPreferences("playback_history", 0);
		return getHistory(settings);
	}
	
	private static void bringToFront(SharedPreferences settings, int n) {
		if (n == 0) {
			return;
		}
		String [] ohist = getHistory(settings);
		String pathn = ohist[n];
		for(int i = n; i > 0; i--) {
			ohist[i] = ohist[i-1];
		}
		ohist[0] = pathn;
		SharedPreferences.Editor editor = settings.edit();
		for(int i = 0; i <= n; i++) {
			editor.putString("entry_" + i, ohist[i]);
		}
		editor.commit();
		
	}
	
	private static void append(SharedPreferences settings, String path) {
		int N = 0;
		while(settings.contains("entry_" + N)) {
			N++;
		}
		SharedPreferences.Editor editor = settings.edit();
		editor.putString("entry_" + N, path);
		editor.commit();
		
	}
	
	private static String [] getHistory(SharedPreferences settings) {
		ArrayList<String> historyList = new ArrayList<String>();
		int N = 0;
		while(settings.contains("entry_" + N)) {
			historyList.add(settings.getString("entry_" + N, ""));
			N++;
		}
		String [] ret = new String[historyList.size()];
		return historyList.toArray(ret);
	}
	
}
