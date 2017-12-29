package com.audiobalance.proto;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.support.v4.view.MotionEventCompat;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

public class CalibrationButton extends View {

	public interface OnPushListener {
		void onPush(boolean down);
	}
	
	private OnPushListener onPushListener;
	
	private Paint buttonPaint;
	
	int centerX, centerY;
	int radius;
	
	boolean down;
	
	public CalibrationButton(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}
	
	public boolean isDown() {
		return down;
	}
	
	public void setOnPushListener(OnPushListener listener) {
		this.onPushListener = listener;
	}
	
	@Override
	public boolean onTouchEvent(MotionEvent event) {
		
		int action = MotionEventCompat.getActionMasked(event);
			
		if (action == MotionEvent.ACTION_DOWN) {
			if (!down && onPushListener != null) {
				onPushListener.onPush(true);
			}
			down = true;
			postInvalidate();
			return true;
		}
		if (action == MotionEvent.ACTION_CANCEL || action == MotionEvent.ACTION_UP) {
			if (down && onPushListener != null) {
				onPushListener.onPush(false);
			}
			down = false;
			postInvalidate();
			return true;
		}
		
		return super.onTouchEvent(event);
	}
	
	@Override
	public void onDraw(Canvas canvas) {
		super.onDraw(canvas);

		buttonPaint.setColor(0xffffffff);
		canvas.drawCircle(centerX, centerY, radius, buttonPaint);
		
		buttonPaint.setColor(0xff000000);
		canvas.drawCircle(centerX, centerY, radius - 10, buttonPaint);
		
		if (down) {
			buttonPaint.setColor(0xffffffff);
			canvas.drawCircle(centerX, centerY, radius - 20, buttonPaint);
		}
	}
	
    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        float xpad = (float)(getPaddingLeft() + getPaddingRight());
        float ypad = (float)(getPaddingTop() + getPaddingBottom());    	
        //mBounds = new RectF(0, 0, w, h);
    	centerX = w/2;
    	centerY = h/2;
    	radius = (int)Math.min(((float)w-xpad)/2, ((float)h-ypad)/2);
    }
		
	private void init() {
		buttonPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
		buttonPaint.setStyle(Paint.Style.FILL);
	}

	
}
