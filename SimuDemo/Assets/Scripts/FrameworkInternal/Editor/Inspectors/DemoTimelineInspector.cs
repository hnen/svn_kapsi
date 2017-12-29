using UnityEngine;
using UnityEditor;
using System.Collections.Generic;

[CustomEditor(typeof(DemoTimeline))]
public class DemoTimelineInspector : Editor 
{
	public override void OnInspectorGUI()
	{
		DemoTimeline demoTimeline = (DemoTimeline)target;

		if (demoTimeline.DemoSong != null) {
			demoTimeline.DemoLength = (int)(demoTimeline.DemoSong.clip.length * 1000.0f);
		}

		this.DrawDefaultInspector();
		
	}
}