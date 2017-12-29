using UnityEngine;
using System.Collections;

public class SimuDemo : MonoBehaviour {
	
	public DemoTimeline tl;

	public GameObject SceneSimuLogo;
	public GameObject SceneEmpty;
	public GameObject SceneTekst1;
	public GameObject SceneTekst2;
	public GameObject SceneTekst3;
	public GameObject JoroScene;
	public GameObject SceneTekst4;
	public GameObject SceneFinalSimuLogo;
	public GameObject Background;

	public int TimeSceneTekst2 = 61750;
	public int TimeSceneTekst3 = 89000;
	public int TimeSceneTekst4 = 144000;
	public int TimeSceneJoro = 116500;

	void Update() {

		if (tl.PassedT(1)) {
			SetScene(SceneSimuLogo);
		}

		//if (tl.PassedT(30850)) {
		//	SetScene(SceneEmpty);
		//}

		if (tl.PassedT(34650)) {
			SetScene(SceneTekst1);
		}

		if (tl.PassedT(55250)) {
			SetScene(SceneEmpty);
		}

		if (tl.PassedT(TimeSceneTekst2)) {
			SetScene(SceneTekst2);
		}

		if (tl.PassedT(89000)) {
			// tähän toka skrolleri
			SetScene(SceneTekst3);
		}

		if (tl.PassedT(114750)) {
			SetScene(SceneEmpty);
		}

		if (tl.PassedT(116500)) {
			// joronjälki-montaasi
			SetScene(JoroScene);
		}

		if (tl.PassedT(TimeSceneTekst4)) {
			// loppupaukutukset
			SetScene(SceneTekst4);
		}

		if (tl.PassedT(157500)) {
			// loppukuva
			SetScene(SceneFinalSimuLogo);
		}

		if (tl.PassedT(170000)) {
			// loppukuva
			SetScene(SceneEmpty);
			Background.SetActive(false);
		}

	}

	void SetScene(GameObject sceneObj) {
		SceneSimuLogo.SetActive(false);
		SceneEmpty.SetActive(false);
		SceneTekst1.SetActive(false);
		SceneTekst2.SetActive(false);
		SceneTekst3.SetActive(false);
		JoroScene.SetActive(false);
		SceneTekst4.SetActive(false);
		SceneFinalSimuLogo.SetActive(false);
		sceneObj.SetActive(true);
	}


}
