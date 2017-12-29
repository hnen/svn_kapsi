package com.audiobalance.proto;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Paint.Style;
import android.util.AttributeSet;
import android.view.View;

public class FrequencyChart extends View {
	
	static final float BG_STROKE_WIDTH = 1.0f;
	static final float FG_STROKE_WIDTH = 5.0f;
	static final float FG_REFERENCE_STROKE_WIDTH = 1.5f;
	static final float BORDER_STROKE_WIDTH = 5.0f;
	static final float TEXT_MARGIN_X = 35.0f;
	static final float TEXT_MARGIN_Y = 45.0f;
		
	static final int COLOR_DISABLED = 0xffcccccc;
	
	Paint bgLine;
	Paint bgLineDisabled;
	Paint fgLine;
	Paint fgFixLine;
	Paint fgReferenceLine;
	Paint borderLine;
	Paint borderLineDisabled;
	Paint text;
	Paint textDisabled;
	int w, h;
	int X, W, H;
	
	boolean disabled;
	
	FreqState [] freqResult;
	
	float minFreq, maxFreq;
	float minIntensity, maxIntensity;
	
	public FrequencyChart(Context context, AttributeSet attrs) {
		super(context, attrs);
		
		minIntensity = CalibrationParameters.GainMin;
		maxIntensity = CalibrationParameters.GainMax;
		
		minFreq = CalibrationParameters.Frequencies[0] - 50;
		maxFreq = CalibrationParameters.Frequencies[CalibrationParameters.Frequencies.length-1] + 3000;
				
		init();
		
		this.disabled = this.freqResult == null;		
	}
	
	
	public void SetFrequencyData(FreqState [] freqResult) {
		this.freqResult = freqResult;
		this.disabled = this.freqResult == null;		
		if (freqResult != null) {
			postInvalidate();
		}
	}
	
	@Override
	public void onDraw(Canvas canvas) {
		super.onDraw(canvas);

		drawBackground(canvas);
		drawGraph(canvas);
		
	}
	
	void drawGraph(Canvas canvas) {
		if (this.freqResult == null) {
			//this.freqResult = CalibrateActivity.createDummyResult();
			//disabled = false;
		}
		
		
		if (!disabled) {
			Path graphReferencePath = new Path();
			graphReferencePath.moveTo(getXForFreq(CalibrationParameters.Frequencies[0]), getYForDb(CalibrationParameters.ReferenceGain[0]));
			for(int i = 1; i < CalibrationParameters.Frequencies.length; i++) {
				graphReferencePath.lineTo(getXForFreq(CalibrationParameters.Frequencies[i]), getYForDb(CalibrationParameters.ReferenceGain[i]));
			}
			canvas.drawPath(graphReferencePath, fgReferenceLine);
			
			Path graphPath = new Path();
			float median = FreqState.ComputeMedianOffset(this.freqResult);
			graphPath.moveTo(getXForFreq(this.freqResult[0].Frequency), getYForDb(this.freqResult[0].getFreqEstimate(-median)));
			for(int i = 1; i < this.freqResult.length; i++) {
				graphPath.lineTo(getXForFreq(this.freqResult[i].Frequency), getYForDb(this.freqResult[i].getFreqEstimate(-median)));
			}
			canvas.drawPath(graphPath, fgLine);
			
			if (AppState.DevMode)
			{
				Path fixPath = new Path();
				fixPath.moveTo(getXForFreq(this.freqResult[0].Frequency), getYForDb(this.freqResult[0].getFreqFixForResult(-median)));
				for(int i = 1; i < this.freqResult.length; i++) {
					fixPath.lineTo(getXForFreq(this.freqResult[i].Frequency), getYForDb(this.freqResult[i].getFreqFixForResult(-median)));
				}
				canvas.drawPath(fixPath, fgFixLine);				
			}
			
		}
		
		Paint textPaint = disabled ? textDisabled : text;
		
		for(int i = 0; i < CalibrationParameters.Frequencies.length; i++) {
			float freq = CalibrationParameters.Frequencies[i];
			float x = getXForFreq(freq) - 15;
			String str = String.format("%d", (int)freq);
			if (i%2 == 0) {
				canvas.drawText(str, 0, str.length(), x, h-1, textPaint);
			} else {
				canvas.drawText(str, 0, str.length(), x, h-20, textPaint);
			}
		}
		
		for(int i = (int)CalibrationParameters.GainMin; i <= CalibrationParameters.GainMax; i+=10) {
			String str = "" + i;
			canvas.drawText(str, 0, str.length(), 0, getYForDb(i), textPaint);
		}
		
		canvas.drawText("Hz", 0, 2, w-25, h-55, textPaint);
		canvas.drawText("dB", 0, 2, 0, 20, textPaint);
	}
	
	void drawBackground(Canvas canvas) {
		
		Paint paint, border;
		if (this.freqResult != null) {
			paint = bgLine;
			border = borderLine;
		} else {
			paint = bgLineDisabled;
			border = borderLineDisabled;
		}
		
		canvas.drawLine(X+BORDER_STROKE_WIDTH/2, 0, X+BORDER_STROKE_WIDTH/2, H, border);
		canvas.drawLine(X+BORDER_STROKE_WIDTH/2, H-BORDER_STROKE_WIDTH/2, w, H-BORDER_STROKE_WIDTH/2, border);
		
		for(int i = 0; i < CalibrationParameters.Frequencies.length; i++) {
			float freq = CalibrationParameters.Frequencies[i];
			float x = getXForFreq(freq);
			canvas.drawLine(x, 0, x, H, paint);
		}
		
		for(int m = -1; m < 4; m++) {
			for(int f = 0; f < 10; f++) {
				float y = getYForLinear(f * (float)Math.pow(10.0, m));
				canvas.drawLine(X, y, w, y, paint);
			}
		}
		
	}
	
	float getXForFreq(float freq) {
		return X + W * (float)((Math.log10(freq) - Math.log10(minFreq)) / (Math.log10(maxFreq) - Math.log10(minFreq)));
	}
	
	float getYForLinear(float gain) {
		return H * (float)((Math.log10(gain) - minIntensity*0.1) / (maxIntensity*0.1 - minIntensity*0.1f));
	}
	float getYForDb(float db) {
		return H * (float)((db - minIntensity) / (maxIntensity - minIntensity));
	}

	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {
		this.w = w;
		this.h = h;
		this.X = (int)TEXT_MARGIN_X;
		this.W = (int)(w - TEXT_MARGIN_X);
		this.H = (int)(h - TEXT_MARGIN_Y);
	}


	private void init() {
		bgLine = new Paint();
		bgLine.setColor(0xff000000);
		bgLine.setStrokeWidth(BG_STROKE_WIDTH);
		
		bgLineDisabled = new Paint();
		bgLineDisabled.setColor(COLOR_DISABLED);
		bgLineDisabled.setStrokeWidth(BG_STROKE_WIDTH);
		
		fgLine = new Paint();
		fgLine.setStyle(Style.STROKE);
		fgLine.setColor(0xffff0000);
		fgLine.setStrokeWidth(FG_STROKE_WIDTH);
		
		fgFixLine = new Paint();
		fgFixLine.setStyle(Style.STROKE);
		fgFixLine.setColor(0xffaa00aa);
		fgFixLine.setStrokeWidth(FG_REFERENCE_STROKE_WIDTH);

		fgReferenceLine = new Paint();
		fgReferenceLine.setStyle(Style.STROKE);
		fgReferenceLine.setColor(0xff555555);
		fgReferenceLine.setStrokeWidth(FG_REFERENCE_STROKE_WIDTH);
		
		borderLine = new Paint();
		borderLine.setColor(0xff000000);
		borderLine.setStrokeWidth(BORDER_STROKE_WIDTH);
		
		borderLineDisabled = new Paint();
		borderLineDisabled.setColor(COLOR_DISABLED);
		borderLineDisabled.setStrokeWidth(BORDER_STROKE_WIDTH);
		
		text = new Paint();
		text.setColor(0xff000000);
		text.setTextSize(20.0f);
		
		textDisabled = new Paint();
		textDisabled.setColor(COLOR_DISABLED);
		textDisabled.setTextSize(20.0f);
	}

}
