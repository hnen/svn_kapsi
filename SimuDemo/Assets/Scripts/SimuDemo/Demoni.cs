using UnityEngine;
using System.Collections;

public class Demoni : MonoBehaviour {


	public fftData fft;
	public Material material;
	public MeshRenderer[] mr;
	public float valX1 = 10.0f;
	public float valX2 = 50.0f;
	public int time;

	// Use this for initialization
	void Start () {
		var g = GameObject.Find("Biisi");
		if (g != null)
			fft = g.GetComponent<fftData>();

		Transform t = this.transform.GetChild(0);
		mr = t.GetComponents<MeshRenderer>();

		material = mr[0].material;
	
	}
	
	// Update is called once per frame
	void Update () {
		/*
		time = (int)(Time.time * 1000.0f);
		float val1 = fftData.getFft(time, 0, 10);
		float val2 = fftData.getFft(time, 7, 27);

		if ((time % 10000) < 5000)
		{
			val1 = 2.35f + 0.5f * Mathf.Sin(Time.time / 5000.0f);
			val2 = (val2 * valX2);
			val2 *= val2;
		}
		else
		{
			val1 = 4.35f + Mathf.Sin(Time.time / 1000.0f);
			val2 = (val2 * valX1);
			val2 *= val2;
		}
		material.SetFloat("_D", val1);
		material.SetFloat("_E", val2);
		material.SetFloat("_T", 100);
		*/
		
	}
}
