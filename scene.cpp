//==========================================
// 
// 画面管理処理[mode.cpp]
// Author : Kajino Hiyori _Create2026/06/29
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "scene.h"
#include "title.h"
#include "game.h"
#include "result.h"
#include "ranking.h"
#include "camera.h"
#include "light.h"
#include "input.h"

//=============================
// コンストラクタ
//=============================
CScene::CScene(MODE mode) : CObject(CObject::PRIORITY_SCENE)
{
	m_mode = mode;
}

//=============================
// デストラクタ
//=============================
CScene::~CScene()
{

}

//=============================
// シーン生成処理
//=============================
CScene* CScene::Create(MODE mode, bool bClear)
{
	CScene* pScene = NULL;	// シーンクラス型のポインタを用意

	switch (mode)
	{
	case MODE_TITLE:	// タイトル画面
		pScene = new CTitle(mode);
		break;
	
	case MODE_GAME:	// ゲーム画面
		pScene = new CGame(mode);
		break;

	case MODE_RESULT:	// リザルト画面
		pScene = new CResult(mode);
		break;

	case MODE_RANKING:	// ランキング画面
		pScene = new CRanking(mode);
		break;
	}

	// モードごとの初期化処理
	pScene->Init();

	// カメラの初期化
	CCamera* pCamera = CManager::GetCamera();
	pCamera->SetSceneCamera(mode, pScene);

	// ライトの初期化
	CLight* pLight = CManager::GetLight();
	pLight->Set(pScene);

	// 振動停止
	CManager::GetInputJoypad()->StopVibration();

	return pScene;
}