//==========================================
// 
// マネージャー処理[manager.cpp]
// Author : Kajino Hiyori _Create2026/05/12
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "manager.h"
#include "renderer.h"
#include "object3D.h"
#include "input.h"
#include "debugproc.h"
#include "camera.h"
#include "light.h"
#include "sound.h"
#include "texture.h"
#include "ranking_offline.h"
#include "field.h"
#include "block.h"
#include "item.h"
#include "fileX.h"

//=============================
// 静的メンバ変数宣言
//=============================
CRenderer* CManager::m_pRenderer = NULL;				// レンダラーへのポインタ
CInputKeyboard* CManager::m_pKeyboard = NULL;			// キーボード入力へのポインタ
CInputMouse* CManager::m_pMouse = NULL;					// マウス入力へのポインタ
CInputJoypad* CManager::m_pJoypad = NULL;				// ジョイパッド入力へのポインタ
CTexture* CManager::m_pTexture = NULL;					// テクスチャへのポインタ
CFileX* CManager::m_pFileX = NULL;						// Xファイルへのポインタ
CDebugProc* CManager::m_pDebugProc = NULL;				// デバッグ表示へのポインタ
CSound* CManager::m_pSound = NULL;						// サウンドへのポインタ
CCamera* CManager::m_pCamera = NULL;					// カメラへのポインタ
CLight* CManager::m_pLight = NULL;						// ライトへのポインタ
CScene* CManager::m_pScene = NULL;						// シーンへのポインタ
CFade* CManager::m_pFade = NULL;						// フェードへのポインタ
CRankingOffline* CManager::m_pRankingOffline = NULL;			// ランキングデータへのポインタ
bool CManager::m_bPause = false;						// ポーズ状態管理[true : ポーズ中 / false : 通常]
bool CManager::m_bOnline = true;						// オンライン状態管理

//=============================
// マクロ定義
//=============================
#define GAME_TIME		(60)	// ゲームの制限時間

//=============================
// コンストラクタ
//=============================
CManager::CManager()
{
}

//=============================
// デストラクタ
//=============================
CManager::~CManager()
{
}

//=============================
// 初期化処理
//=============================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	int nID = 0;	// メッセージボックス管理

	// レンダラー生成処理======================================================
	if (m_pRenderer == NULL)
	{
		m_pRenderer = new CRenderer;		// レンダラー生成
		if (m_pRenderer == NULL)
		{ // レンダラー生成に失敗
			nID = MessageBox(hWnd, "レンダラー生成に失敗", WINDOW_NAME, MB_OK);
			return E_FAIL;
		}
	}
	else
	{ // レンダラーのクリアに失敗
		nID = MessageBox(hWnd, "レンダラー生成に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}
	// レンダラー初期化処理
	if (FAILED(m_pRenderer->Init(hWnd, bWindow)))
	{ // 初期化処理に失敗した場合
		nID = MessageBox(hWnd, "レンダラーの初期化に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}

	// キーボード入力の初期化==================================================
	if (m_pKeyboard == NULL)
	{
		m_pKeyboard = new CInputKeyboard;		// キーボード入力生成
		if (m_pKeyboard == NULL)
		{ // キーボード入力生成に失敗
			nID = MessageBox(hWnd, "キーボード入力生成に失敗", WINDOW_NAME, MB_OK);
			return E_FAIL;
		}
	}
	else
	{ // キーボード入力のクリアに失敗
		nID = MessageBox(hWnd, "キーボード入力生成に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}
	// キーボード入力初期化処理
	if (FAILED(m_pKeyboard->Init(hInstance, hWnd)))
	{ // 初期化処理に失敗した場合
		nID = MessageBox(hWnd, "キーボード入力の初期化に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}

	// マウス入力の初期化======================================================
	if (m_pMouse == NULL)
	{
		m_pMouse = new CInputMouse;		// マウス入力生成
		if (m_pMouse == NULL)
		{ // マウス入力生成に失敗
			nID = MessageBox(hWnd, "マウス入力生成に失敗", WINDOW_NAME, MB_OK);
			return E_FAIL;
		}
	}
	else
	{ // マウス入力のクリアに失敗
		nID = MessageBox(hWnd, "マウス入力生成に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}
	// マウス入力初期化処理
	if (FAILED(m_pMouse->Init(hInstance, hWnd)))
	{ // 初期化処理に失敗した場合
		nID = MessageBox(hWnd, "マウス入力の初期化に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}

	// ジョイパッド入力の初期化================================================
	if (m_pJoypad == NULL)
	{
		m_pJoypad = new CInputJoypad;		// ジョイパッド入力生成
		if (m_pJoypad == NULL)
		{ // ジョイパッド生成に失敗
			nID = MessageBox(hWnd, "ジョイパッド入力生成に失敗", WINDOW_NAME, MB_OK);
			return E_FAIL;
		}
	}
	else
	{ // ジョイパッド入力のクリアに失敗
		nID = MessageBox(hWnd, "ジョイパッド入力生成に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}
	// ジョイパッド入力初期化処理
	if (FAILED(m_pJoypad->Init(hInstance, hWnd)))
	{ // 初期化処理に失敗した場合
		nID = MessageBox(hWnd, "ジョイパッド入力の初期化に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}

	// テクスチャの初期化======================================================
	if (m_pTexture == NULL)
	{
		m_pTexture = new CTexture;	// テクスチャ生成
		if (m_pTexture == NULL)
		{ // テクスチャ生成に失敗
			nID = MessageBox(hWnd, "テクスチャ生成に失敗", WINDOW_NAME, MB_OK);
			return E_FAIL;
		}
	}
	else
	{ // テクスチャのクリアに失敗
		nID = MessageBox(hWnd, "テクスチャ生成に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}
	// テクスチャ読み込み処理
	if (FAILED(m_pTexture->Load()))
	{ // 初期化処理に失敗した場合
		nID = MessageBox(hWnd, "テクスチャの初期化に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}

	// Xファイルの初期化======================================================
	if (m_pFileX == NULL)
	{
		m_pFileX = new CFileX;	// Xファイル生成
		if (m_pFileX == NULL)
		{ // Xファイル生成に失敗
			nID = MessageBox(hWnd, "Xファイル生成に失敗", WINDOW_NAME, MB_OK);
			return E_FAIL;
		}
	}
	else
	{ // テクスチャのクリアに失敗
		nID = MessageBox(hWnd, "Xファイル生成に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}
	// テクスチャ読み込み処理
	if (FAILED(m_pFileX->Load()))
	{ // 初期化処理に失敗した場合
		nID = MessageBox(hWnd, "Xファイルの初期化に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}

	// カメラの初期化=========================================================
	if (m_pCamera == NULL)
	{
		m_pCamera = new CCamera;	// カメラ生成
		if (m_pCamera == NULL)
		{// カメラ生成に失敗
			nID = MessageBox(hWnd, "カメラ生成に失敗", WINDOW_NAME, MB_OK);
			return E_FAIL;
		}
	}
	else
	{ // カメラのクリアに失敗
		nID = MessageBox(hWnd, "カメラ生成に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}
	// カメラ入力初期化処理
	if (FAILED(m_pCamera->Init()))
	{ // 初期化処理に失敗した場合
		nID = MessageBox(hWnd, "カメラ入力の初期化に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}

	// ライトの初期化=========================================================
	if (m_pLight == NULL)
	{
		m_pLight = new CLight;	// ライト生成
		if (m_pLight == NULL)
		{// ライト生成に失敗
			nID = MessageBox(hWnd, "ライト生成に失敗", WINDOW_NAME, MB_OK);
			return E_FAIL;
		}
	}
	else
	{ // ライトのクリアに失敗
		nID = MessageBox(hWnd, "ライト生成に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}
	// ライト入力初期化処理
	if (FAILED(m_pLight->Init()))
	{ // 初期化処理に失敗した場合
		nID = MessageBox(hWnd, "ライト入力の初期化に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}

	// デバッグ表示入力の初期化================================================
	if (m_pDebugProc == NULL)
	{
		m_pDebugProc = new CDebugProc;		// デバッグ表示入力生成
		if (m_pDebugProc == NULL)
		{ // デバッグ表示生成に失敗
			nID = MessageBox(hWnd, "デバッグ表示入力生成に失敗", WINDOW_NAME, MB_OK);
			return E_FAIL;
		}
	}
	else
	{ // デバッグ表示入力のクリアに失敗
		nID = MessageBox(hWnd, "デバッグ表示入力生成に失敗", WINDOW_NAME, MB_OK);
		return E_FAIL;
	}
	// デバッグ表示入力初期化処理
	m_pDebugProc->Init();

	// サウンドの初期化=======================================================
	if (m_pSound == NULL)
	{
		m_pSound = new CSound;	// サウンド生成
		if (m_pSound == NULL)
		{ // サウンド生成に失敗
			nID = MessageBox(hWnd, "サウンド生成に失敗", WINDOW_NAME, MB_OK);
			return E_FAIL;
		}
	}
	if (FAILED(m_pSound->Init(hWnd)))
	{ // 初期化に失敗
		return E_FAIL;
	}

	// フェードの初期化=======================================================
	if (m_pFade == NULL)
	{
		m_pFade = CFade::Create(CFade::TYPE_ALPHA, SCREEN_CENTER, COLOR_BLACK);	// フェード生成
		if (m_pFade == NULL)
		{ // フェード生成に失敗
			nID = MessageBox(hWnd, "フェード生成に失敗", WINDOW_NAME, MB_OK);
			return E_FAIL;
		}
	}

	// ブロックモデルの読み込み
	CBlock::LoadInfo();

	// アイテムモデルの読み込み
	CItem::LoadInfo();

	// モード設定
	SetMode(CScene::MODE_GAME);

#ifdef ENABLE_ONLINE
	// ランキングデータ[オンライン]の初期化=========================================================
	// 送受信設定
	WSADATA wsaData;	// Winsock	
	// Winsockの初期化関数を実行
	int nErr = WSAStartup(WINSOCK_VERSION, &wsaData);
	if (nErr != 0)
	{ // 初期化に失敗
		nID = MessageBox(hWnd, "Winsockの初期化に失敗", WINDOW_NAME, MB_OK);
		m_bOnline = false;
	}
	if (m_pRankingOnline == NULL)
	{
		m_pRankingOnline = new CRankingOnline;	// ランキングデータ生成
		if (m_pRankingOnline == NULL)
		{
			nID = MessageBox(hWnd, "ランキングデータ生成に失敗", WINDOW_NAME, MB_OK);
			m_bOnline = false;
		}
	}
	if (FAILED(m_pRankingOnline->Init()))
	{ // 初期化に失敗
		nID = MessageBox(hWnd, "接続に失敗", WINDOW_NAME, MB_OK);
		m_bOnline = false;
	}
	
	if (m_bOnline == false)
	{
		// オンラインランキング情報を破棄
		SAFE_UNINIT(m_pRankingOnline);

		// ランキングデータ[オフライン]の初期化=========================================================
		m_pRankingOffline = CRankingOffline::Create();
	}
#else
	m_bOnline = false;
	// ランキングデータ[オフライン]の初期化=========================================================
	m_pRankingOffline = CRankingOffline::Create();

#endif // ENABLE_ONLINE

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CManager::Uninit(void)
{
	// オブジェクトの終了処理
	CObject::ReleaseAll();
	
	 // オフラインランキングデータの終了処理
	SAFE_UNINIT(m_pRankingOffline);

	// フェードの破棄
	SAFE_UNINIT(m_pFade);

	// デバッグ表示の破棄
	SAFE_UNINIT(m_pDebugProc);

	// テクスチャの破棄
	SAFE_UNLOAD(m_pTexture);

	// モデルの破棄
	SAFE_UNLOAD(m_pFileX);

	// キーボード入力の破棄
	SAFE_UNINIT(m_pKeyboard);

	// マウス入力の破棄
	SAFE_UNINIT(m_pMouse);

	// ジョイパッド入力の破棄
	SAFE_UNINIT(m_pJoypad);

	// ライトの破棄
	SAFE_UNINIT(m_pLight);

	// カメラの破棄
	SAFE_UNINIT(m_pCamera);

	// 音声の停止
	m_pSound->Stop();
	
	// サウンドの終了処理
	SAFE_UNINIT(m_pSound);

	// レンダラーの破棄
	SAFE_UNINIT(m_pRenderer);
}

//=============================
// 更新処理
//=============================
void CManager::Update(void)
{
	// キーボード入力の更新処理
	m_pKeyboard->Update();

	// マウス入力の更新処理
	m_pMouse->Update();

	// ジョイパッド入力の更新処理
	m_pJoypad->Update();

	// カメラの更新処理
	m_pCamera->Update();

	// ライトの更新処理
	m_pLight->Update();

	// フェードの更新
	if (GetFadeState() != CFade::STATE_FADENONE)
	{
		m_pFade->Update();
	}

	// デバック表示
	m_pDebugProc->Print("オブジェクト数 : %d\n", CObject::GetNumAll());
	//m_pDebugProc->Print("フィールド数 : %d\n", CField::GetNumAll());

	// レンダラー更新処理
	m_pRenderer->Update();
	
	// デバッグ表示の更新処理
	m_pDebugProc->Update();
}

//=============================
// 描画処理
//=============================
void CManager::Draw(void)
{
	// レンダラー描画処理
	m_pRenderer->Draw();
}

//=============================
// 再読み込み時に再生成
//=============================
void CManager::SetMode(CScene::MODE mode, bool bClear)
{
	// 音声の停止
	m_pSound->Stop();

	// 全てのオブジェクトを破棄
	CObject::ReleaseAll();

	//新しいシーン生成
	m_pScene = NULL;
	m_pScene = CScene::Create(mode, bClear);

}

//=============================
// 再読み込み時に再生成
//=============================
void CManager::Reload(void)
{
	// ポーズ状態の初期化
	m_bPause = false;
}