//========================================================
// 
// Player処理[leftplayer.cpp]
// Author : Kajino Hiyori _Create2026/06/02
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "player.h"
#include "camera.h"
#include "input.h"
#include "color.h"
#include "debugproc.h"
#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "model.h"
#include "shadow.h"
#include "meshfield.h"
#include "scene.h"
//#include "title.h"
//#include "tutorial.h"
#include "game.h"
//#include "result.h"
//#include "ranking.h"
//#include "credit.h"
//#include "tutorialUI.h"
#include "particle.h"
#include "sound.h"
#include "loadscripts.h"

//=============================
// マクロ定義
//=============================
#define FILE_NAME		("data\\SCRIPTS\\Player.txt")	// ファイル名
#define PRESSANIM_NAME	("data\\TEXTURE\\pressAnim_L.png")	// 有効攻撃表示テクスチャ名

//=============================
// コンストラクタ
//=============================
CPlayer::CPlayer(CObject::PRIORITY priority) : CObject(priority)
{
	// 値のクリア
	m_pos = INIT_D3DVEC3;		// モデルの位置
	m_posOld = INIT_D3DVEC3;	// モデルの1フレーム前の位置
	m_move = INIT_D3DVEC3;		// モデルの移動量
	m_rot = INIT_D3DVEC3;		// モデルの向き
	m_rotDest = INIT_D3DVEC3;	// モデルの目的の向き
	memset(&m_apModel[0], NULL, sizeof m_apModel);	// モデルのポインタ
	m_nNumModel = 0;			// モデル総数
	m_pMotion = NULL;			// モーション情報
	m_pShadow = NULL;			// 影情報
}

//=============================
// デストラクタ
//=============================
CPlayer::~CPlayer()
{

}

//=============================
// Player生成処理
//=============================
CPlayer* CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CPlayer* pPlayer = NULL;	// オブジェクトへのポインタを用意
	if (pPlayer == NULL)
	{ // NULLが設定できている場合
		pPlayer = new CPlayer;
		if (pPlayer != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pPlayer->Init(pos, rot);

			// 種類設定
			pPlayer->SetType(TYPE_PLAYER);

			return pPlayer;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CPlayer::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// 初期化
	m_pos = pos;
	m_posOld = pos;
	m_move = INIT_D3DVEC3;
	m_rot = rot;
	m_rotDest = rot;

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CPlayer::Uninit(void)
{
	// モデルの終了処理
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (m_apModel[nCntModel] == NULL) continue;

		// 使用されていた場合
		m_apModel[nCntModel]->Uninit();
		delete m_apModel[nCntModel];
		m_apModel[nCntModel] = NULL;
	}

	// モーションの破棄
	if (m_pMotion != NULL)
	{ // モーションが用意されていた場合
		delete m_pMotion;
		m_pMotion = NULL;
	}

	// 影の破棄
	m_pShadow = NULL;
}

//=============================
// 更新処理
//=============================
void CPlayer::Update(void)
{
	// シーン情報
	CScene::MODE mode = CManager::GetMode();

	// メッシュフィールド情報
	CMeshField* pMeshField = NULL;

	// 過去の位置を保存
	m_posOld = m_pos;

	// シーンごとに処理を分割
	switch (mode)
	{
	//case CScene::MODE_TITLE:	// タイトル
	//	// タイトル処理
	//	UpdateTitle();
	//	pMeshField = static_cast<CTitle*>(CManager::GetScene())->GetMeshField();
	//	break;

	//case CScene::MODE_TUTORIAL:	// チュートリアル
	//	// チュートリアル処理
	//	UpdateTutorial();
	//	pMeshField = static_cast<CTutorial*>(CManager::GetScene())->GetMeshField();
	//	break;

	case CScene::MODE_GAME:		// ゲーム
		// ゲーム処理
		UpdateGame();
		//pMeshField = static_cast<CGame*>(CManager::GetScene())->GetMeshField();
		break;

	//case CScene::MODE_RESULT:	// リザルト
	//	// リザルト処理
	//	UpdateResult();
	//	pMeshField = static_cast<CResult*>(CManager::GetScene())->GetMeshField();
	//	break;

	//case CScene::MODE_RANKING:	// ランキング
	//	pMeshField = static_cast<CRanking*>(CManager::GetScene())->GetMeshField();
	//	break;

	//case CScene::MODE_CREDIT:	// クレジット
	//	pMeshField = static_cast<CCredit*>(CManager::GetScene())->GetMeshField();
	//	break;
	}

	// モーションの更新
	m_pMotion->Update();

	// デバック表示=================================================================================
	CDebugProc* pDebugProc = CManager::GetDebugProc();
	pDebugProc->Print("Playerの位置 : (%.2f, %.2f, %.2f)\n", m_pos.x, m_pos.y, m_pos.z);
	pDebugProc->Print("Player向き : (%.2f, %.2f, %.2f)\n", m_rot.x, m_rot.y, m_rot.z);
	pDebugProc->Print("Player移動量 : (%.2f, %.2f, %.2f)\n", m_move.x, m_move.y, m_move.z);
}

//=============================
// 描画処理
//=============================
void CPlayer::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxGauge;	// ゲージ用マトリックス

	// ワールドマトリックスの初期化(デフォルトの値にする)
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ゲージ用のマトリックスを格納
	mtxGauge = m_mtxWorld;

	// モデルごとに描画
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (m_apModel[nCntModel] == NULL)continue;
		 // 使用されていた場合
		// モデルの描画
		m_apModel[nCntModel]->Draw();
		
	}

#ifndef ENABLE_MULTITARGETRENDERING
	CCamera* pCamera = CManager::GetCamera();
	LPDIRECT3DSURFACE9 pRenderDef, pZBuffDef;
	D3DVIEWPORT9 viewportDef;
	D3DXMATRIX mtxView, mtxProjectionDef;

	// 現在の状態を保存
	pDevice->GetRenderTarget(0, &pRenderDef);	// レンダリングターゲット
	pDevice->GetDepthStencilSurface(&pZBuffDef);	// Zバッファ
	pDevice->GetViewport(&viewportDef);				// ビューポート
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);	// ビューマトリックス
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjectionDef);	// プロジェクションマトリックス

	// レンダリングターゲットの変更
	pRenderer->ChangeTarget(pCamera->GetPosV(), pCamera->GetPosR(), pCamera->GetVecU());
	
	// ワールドマトリックスの初期化(デフォルトの値にする)
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// モデルごとに描画
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (m_apModel[nCntModel] != NULL)
		{ // 使用されていた場合
			// モデルの描画
			m_apModel[nCntModel]->Draw();
		}
	}

	// レンダリング状態を元に戻す
	pDevice->SetRenderTarget(0, pRenderDef);	// レンダリングターゲット
	pDevice->SetDepthStencilSurface(pZBuffDef);	// Zバッファ
	pDevice->SetViewport(&viewportDef);				// ビューポート
	pDevice->SetTransform(D3DTS_VIEW, &mtxView);	// ビューマトリックス
	pDevice->SetTransform(D3DTS_PROJECTION, &mtxProjectionDef);	// プロジェクションマトリックス

#endif
}

//=============================
// タイトル処理
//=============================
void CPlayer::UpdateTitle(void)
{

}

//=============================
// チュートリアル処理
//=============================
void CPlayer::UpdateTutorial(void)
{
	
}

//=============================
// ゲーム処理
//=============================
void CPlayer::UpdateGame(void)
{
	
}

//=============================
// リザルト処理
//=============================
void CPlayer::UpdateResult(void)
{
	
}

//=============================
// 移動処理
//=============================
void CPlayer::Move(void)
{
	// カメラ取得
	CCamera* pCamera = CManager::GetCamera();
	D3DXVECTOR3 rotCamera = pCamera->GetRotation();

	// スティックの角度を取得
	float fRotDiffKey = 0.0f;	// キー入力時の角度補正計算用
	float fRotDiffPad = 0.0f;	// ジョイパッド入力時の角度補正計算用

	// 移動管理
	D3DXVECTOR3 moveDirection = INIT_D3DVEC3;	// 移動方向の設定
	float fMoveDirection = 0.0f;	// 移動方向の管理

	float fAngle = 0.0f;	// 角度管理

	// 入力情報取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	
}