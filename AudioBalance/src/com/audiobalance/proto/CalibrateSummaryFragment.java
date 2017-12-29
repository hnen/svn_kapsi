package com.audiobalance.proto;

import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.app.Activity;
import android.content.ContentValues;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Paint;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore.Images;
import android.provider.MediaStore.Images.Media;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class CalibrateSummaryFragment extends Fragment {
	
	private Runnable onReady;
	
	boolean delayAccepted;
	float delayResult;
	FreqState [] frequencyResult;
	
	public CalibrateSummaryFragment(Runnable onReady, boolean delayAccepted, float delayResult, FreqState [] frequencyResult) {
		this.onReady = onReady;
		this.delayAccepted = delayAccepted;
		this.delayResult = delayResult;
		this.frequencyResult = frequencyResult;
	}
	
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
		
		if (AppState.ResearchVersion) {
			View view = inflater.inflate(R.layout.fragment_calibration_summary_research, container, false);
			
			saveResultsToImage();
			
			Button b = (Button)view.findViewById(R.id.button_calibration_summary_research_continue);
			b.setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					onReady.run();
				}
			});
			
			return view;
		} else {
			if (AppState.DevBuild) {
				View view = inflater.inflate(R.layout.fragment_calibration_summary, container, false);
				
				Button b = (Button)view.findViewById(R.id.button_calibrate_summary_confirm);
				b.setOnClickListener(new OnClickListener() {
					@Override
					public void onClick(View v) {
						onReady.run();
					}
				});
				
				BalanceIndicator bi = (BalanceIndicator)view.findViewById(R.id.balanceindicator_calibration_summary);
				bi.setValue(this.delayResult);
				bi.setDisabled(!this.delayAccepted);
				
				TextView delay = (TextView)view.findViewById(R.id.textview_calibration_summary_delay);
				if (this.delayAccepted) {
					delay.setText(String.format("%.1fms", this.delayResult));
				} else {
					delay.setText("-- ms");
				}
				
				if (!AppState.DisableFrequency) {
					FrequencyChart chart = (FrequencyChart)view.findViewById(R.id.frequencychart_calibration_summary);
					chart.SetFrequencyData(this.frequencyResult);
				}
				
				return view;
			} else {
				final View view = inflater.inflate(R.layout.fragment_calibration_summary_no_frequency, container, false);
							
				Button listenAudioClipsButton = (Button)view.findViewById(R.id.button_calibration_summary_listen_audio_clips);
				listenAudioClipsButton.setOnClickListener(new OnClickListener() {
					@Override
					public void onClick(View v) {
				    	Intent openAudioSelectorIntent = new Intent(view.getContext(), AudioSelectorActivity.class);
				    	startActivity(openAudioSelectorIntent);
				    	((Activity) view.getContext()).overridePendingTransition(R.anim.push_left_in, R.anim.push_left_out);
					}
				});
				
				
				return view;
			}
		}
	}
	
	public void saveResultsToImage() {
		Bitmap bitmap = Bitmap.createBitmap(256, 162, Config.ARGB_8888);
		
		Canvas canvas = new Canvas(bitmap);
		Paint clrPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
		clrPaint.setColor(0xffffffff);
		canvas.drawPaint(clrPaint);
		Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
		paint.setTextSize(14);
		paint.setColor(0xff000000);
		
		SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
		Date date = new Date();
		
		String clipsPlayed = "";
		boolean first = true;
		for(int i = 0; i < AppState.filesPlayedLast.length; i++) {
			if (!first) {
				clipsPlayed += ", ";
			}
			clipsPlayed += AppState.filesPlayedLast[i];
			first = false;
		}
		
		String text = String.format(
				"Date: %s\n" +
				"Subject ID: %s\n" +
				"Clips Played: %s\n" +
				"System volume: %d\n" +
				"Volume adjustment: %.2fdB\n" +
				"RESULT: %.1fms\n",
				dateFormat.format(date),
				AppState.activeParameters.researchSubjectId,
				clipsPlayed,
				AppState.activeParameters.systemVolume,
				AppState.activeParameters.adjustVolume, this.delayResult);
		
		canvas.drawText(stripLine(text, 0), 0, 25 * 1, paint);
		canvas.drawText(stripLine(text, 1), 0, 25 * 2, paint);
		canvas.drawText(stripLine(text, 2), 0, 25 * 3, paint);
		canvas.drawText(stripLine(text, 3), 0, 25 * 4, paint);
		canvas.drawText(stripLine(text, 4), 0, 25 * 5, paint);
		canvas.drawText(stripLine(text, 5), 0, 25 * 6, paint);
		
		String title = String.format("Calibration Result %s %s", AppState.activeParameters.researchSubjectId, dateFormat.format(date));
		
	    ContentValues values = new ContentValues();
	    values.put(Media.TITLE, title);
	    values.put(Media.DESCRIPTION, text); 
	    values.put(Images.Media.DATE_TAKEN, System.currentTimeMillis());
	    values.put(Images.Media.MIME_TYPE, "image/jpeg");
	    //values.put(MediaStore.MediaColumns.DATA, filepath);		
	    Uri uri = this.getActivity().getContentResolver().insert(Media.EXTERNAL_CONTENT_URI, values);

	    try {
	    	OutputStream imageOut = this.getActivity().getContentResolver().openOutputStream(uri);
	    	bitmap.compress(Bitmap.CompressFormat.PNG, 50, imageOut);
	    	imageOut.close();
	    } catch(Exception e) {
	    	e.printStackTrace();
	    	//Log.e("CalibrateSummaryFragment", );
	    }
	    
		//MediaStore.Images.Media.insertImage(this.getActivity().getContentResolver(), bitmap, title, text);
		
	}
	
	private static String stripLine(String txt, int l) {
		int n = 0;
		int i = 0;
		while(n < l) {
			if (txt.charAt(i) == '\n') {
				n++;
			}
			i++;
		}
		String result = "";
		while(i < txt.length() && txt.charAt(i) != '\n') {
			result += txt.charAt(i);
			i++;
		}
		return result;
	}
	
}
