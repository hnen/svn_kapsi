package com.audiobalance.proto;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Paint.Style;
import android.util.AttributeSet;
import android.view.View;

public class BalanceIndicator extends View {
	Paint linePaint;
	Paint linePaintDisabled;
	int w, h;
	Path triangle;
	
	static final int COLOR_DISABLED = 0xffcccccc;
	static final float BG_LINE_WIDTH = 7.5f;
	static final float CAP_LINE_HEIGHT = 0.75f;
	static final float CENTER_LINE_HEIGHT = 0.5f;
	
	float value;
	
	boolean disabled;
	
	public BalanceIndicator(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}
	
	public void setValue(float delayValue) {
		this.value = delayValue;
		postInvalidate();
	}
	
	public void setDisabled(boolean disabled) {
		this.disabled = disabled;
		postInvalidate();
	}
	
	@Override
	public void onDraw(Canvas canvas) {
		super.onDraw(canvas);
		
		Paint line = disabled ? linePaintDisabled : linePaint;
		
		canvas.drawLine(0, h/2, w - BG_LINE_WIDTH/2, h/2, line);
		canvas.drawLine(BG_LINE_WIDTH/2, 	 h/2-CAP_LINE_HEIGHT*(h/2), 	BG_LINE_WIDTH/2, 	 h/2+CAP_LINE_HEIGHT*(h/2), line);
		canvas.drawLine(w-BG_LINE_WIDTH/2-1, h/2-CAP_LINE_HEIGHT*(h/2),  w-BG_LINE_WIDTH/2-1, h/2+CAP_LINE_HEIGHT*(h/2), line);
		canvas.drawLine(w/2, h/2-CENTER_LINE_HEIGHT*(h/2), w/2, h/2+CENTER_LINE_HEIGHT*(h/2), line);
		
		if (!disabled) {
			float x = getTrianglePosition();
			triangle = new Path();
			triangle.moveTo( x, h/2);
			triangle.lineTo( x -h/4, h);		
			triangle.lineTo( x +h/4, h);
			triangle.close();
			canvas.drawPath(triangle, linePaint);
		}
	}
	
	protected float getTrianglePosition() {
		return w * (value - CalibrationParameters.DelayMin) / (CalibrationParameters.DelayMax - CalibrationParameters.DelayMin);
	}

	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {
		this.w = w;
		this.h = h;
	}


	private void init() {
		linePaint = new Paint();
		linePaint.setColor(0xff000000);
		linePaint.setStyle(Style.FILL);
		linePaint.setStrokeWidth(BG_LINE_WIDTH);
		
		linePaintDisabled = new Paint();
		linePaintDisabled.setColor(COLOR_DISABLED);
		linePaintDisabled.setStyle(Style.FILL);
		linePaintDisabled.setStrokeWidth(BG_LINE_WIDTH);
		
	}

}
