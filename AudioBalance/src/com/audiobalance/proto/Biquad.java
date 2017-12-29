package com.audiobalance.proto;

public class Biquad {

	double a0, a1, a2, b0, b1, b2;
	//double Fc, Q, peakGainDb;
	double z1, z2;
	
	public Biquad(double Fc, int sampleRate, double Q, double peakGainDb) {
		//this.Fc = Fc;
		//this.Q = Q;
		//this.peakGainDb = peakGainDb;
		this.z1 = 0;
		this.z2 = 0;
		calcCoeff(Fc / sampleRate, Q, peakGainDb);
	}
	
	double xn1, xn2, yn1, yn2;
	
	public double process(double in) {
		double xn = in;
		double yn = (b0/a0)*xn + (b1/a0) * xn1 + (b2/a0)* xn2 - (a1/a0)*yn1 - (a2/a0)*yn2;
		xn2 = xn1;
		xn1 = xn;
		yn2 = yn1;
		yn1 = yn;
		return yn;
		//double out = in * a0 + z1;
		//z1 = in * a1 + z2 - b1 * out;
		//z2 = in * a2 - b2 * out;
		//return out;
	}
	
	private final double SQRT_2 = 1.41421356237;
	
	private void calcCoeff(double Fc, double Q, double peakGainDb) {
		double A = Math.sqrt(Math.pow(10.0, peakGainDb/40.0));
		double w0 = 2.0 * Math.PI * Fc;
		//double alpha = Math.sin(w0) / (2.0 * Q);
		double alpha = Math.sin(w0) * Math.sinh( Math.log(2.0) / 2.0 * Q * w0/Math.sin(w0));
		b0 = 1.0 + alpha * A;
		b1 =-2.0 * Math.cos(w0);
		b2 = 1.0 - alpha * A;
		a0 = 1.0 + alpha / A;
		a1 = b1;
		a2 = 1.0 - alpha / A;
		/*
		double norm = 0.0;
		peakGainDb = -peakGainDb;
		double V = Math.pow(10.0, Math.abs(peakGainDb / 20.0));
		double K = Math.tan(Math.PI * Fc);
		if (peakGainDb >= 0) {
			norm = 1.0 / (1.0 + K/Q + K*K);
			a0 = (1.0 + V/Q * K + K*K) * norm;
			a1 = 2.0 * (K*K - 1.0) * norm;
			a2 = (1.0 - V/Q * K + K*K) * norm;
			b1 = a1;
			b2 = (1.0 - K/Q + K*K) * norm;
		} else {
			norm = 1.0 / (1.0 + V/Q * K + K*K);
			a0 = (1.0 + V/Q * K + K*K) * norm;
			a1 = 2.0 * (K*K - 1.0) * norm;
			a2 = (1.0 - 1/Q * K + K*K) * norm;
			b1 = a1;
			b2 = (1 - V/Q * K + K*K) * norm;
		}
		*/
	}
	
	
}
