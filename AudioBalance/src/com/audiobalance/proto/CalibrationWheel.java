package com.audiobalance.proto;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;

public class CalibrationWheel extends View {
	Paint wheelPaint;
	int w, h;

	GestureDetector scrollDetector;
	float scrollValue;
	float sensitivity;
	
	OnValueChangedListener onValueChangedListener;
	
	public interface OnValueChangedListener {
		void onValueChanged(CalibrationWheel wheel);
	}

	public CalibrationWheel(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.setSensitivity(1.0f);
	}
	
	@Override
	public void onFinishInflate() {
		init();
	}
	
	public void setSensitivity(float sensitivity) {
		this.sensitivity = sensitivity;
	}
	
	public void setOnValueChangedListener(OnValueChangedListener listener) {
		this.onValueChangedListener = listener;
	}

	public float getValue() {
		return scrollValue;
	}
	
	@Override
	public void onDraw(Canvas canvas) {
		super.onDraw(canvas);
		
		float height = 60.0f * h / 200;
		float gap = 20.0f * h / 200;
		
		float yoff = -scrollValue * 2500.0f * h / (200.0f * this.sensitivity);
		float ymod = height + gap;
		yoff = yoff - (float)Math.floor(yoff / ymod) * ymod;
		
		for(float y = yoff - ymod; y < h; y += ymod) {
			RectF rect = new RectF(0, y, w, y + height);
			//RectF frect = new RectF(0, Math.max(rect.top,0), w, Math.min(rect.bottom, h));
			canvas.drawRect(rect, wheelPaint);
		}
		
	}

	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {
		this.w = w;
		this.h = h;
		scrollDetector = new GestureDetector(this.getContext(), new Listener(h));
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
 		boolean valid = scrollDetector.onTouchEvent(event);
		return true;
	}

	private void init() {
		wheelPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
		wheelPaint.setStyle(Paint.Style.FILL);
		wheelPaint.setColor(0xffffffff);
		int elementHeight = this.getHeight();		
		scrollDetector = new GestureDetector(this.getContext(), new Listener(elementHeight));
		scrollValue = 0.5f;
	}

	private void scrolled(float amount) {
		float prevValue = scrollValue;
		scrollValue += amount * this.sensitivity;
		if (scrollValue < 0) {
			scrollValue = 0;
		}
		if (scrollValue > 1) {
			scrollValue = 1;
		}
		if (prevValue != scrollValue && this.onValueChangedListener != null) {
			this.onValueChangedListener.onValueChanged(this);
		}
		Log.d("CalibrationWheel", "scrollValue: " + scrollValue);
		postInvalidate();
	}

	class Listener extends GestureDetector.SimpleOnGestureListener {
		
		float elementHeight;
		
		public Listener(float elementHeight) {
			Log.i("CalibrationWheel", "elementHeight: " + elementHeight);
			this.elementHeight = elementHeight;
		}

		@Override
		public boolean onScroll(MotionEvent e1, MotionEvent e2,
				float distanceX, float distanceY) {
			//TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_PX, distanceY)
			CalibrationWheel.this.scrolled(distanceY / (15.0f * elementHeight));
			return true;
		}

		@Override
		public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
			//scroller.fling(0, 0, 0, (int) velocityY, 0, Integer.MAX_VALUE, 0, Integer.MAX_VALUE);
			return false;
		}
		
	}

}
