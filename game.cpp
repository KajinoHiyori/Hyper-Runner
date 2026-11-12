//==========================================
// 
// ゲーム処理[game.h]
// Author : Kajino Hiyori _Create2026/06/29
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "object2D.h"
#include "game.h"
#include "manager.h"
#include "input.h"
#include "sound.h"
#include "field.h"
#include "loadscripts.h"
#include "particle.h"
#include "meshcylinder.h"
#include "debugproc.h"
#include "player.h"
#include "field.h"
#include "block.h"
#include "item.h"
#include "meshskin_flat.h"
#include "meshskin_cylinder.h"

//=============================
// 静的メンバ変数宣言
//=============================
CPlayer* CGame::m_pPlayer = NULL;	// プレイヤーへのポインタ
CField* CGame::m_apField[MAX_FIELDLINE] = {};	// フィールドへのポインタ
//CWall* CGame::m_apWall[MAX_GAMEWALL] = {};	// 壁のポインタ
//CMachine* CGame::m_pMachine = NULL;	// マシンへのポインタ
//CTimer* CGame::m_pTimer = NULL;		// タイマーへのポインタ
//CPause* CGame::m_pPause = NULL;		// ポーズへのポインタ
//CGameUI* CGame::m_pGameUI = NULL;	// UIへのポインタ

//=============================
// マクロ定義
//=============================
#define GAME_TIME		(60)	// ゲーム時間
#define FIELD_WIDTH		(100.0f)	// フィールド幅
#define FIELD_DEPTH		(5000.0f)	// フィールド奥行き
#define FIELD_SPACE		(200.0f)		// フィールド間の隙間
#define FIELD_POS(x)	(D3DXVECTOR3(FIELD_SPACE * 2.0f * (float)(x - 1), 0.0f, 0.0f))	// フィールド位置
// ファイルパス
#define BLOCK_SETTINGPATH	("data\\SCRIPTS\\block_game.txt")	// ゲーム内でのブロック設置情報
#define ITEM_SETTINGPATH	("data\\SCRIPTS\\item_game.txt")	// ゲーム内でのアイテム設置情報
#define MESHSKIN_PATH		("data\\SCRIPTS\\meshskin_flat.txt")	// メッシュスキンの設置情報

//=============================
// コンストラクタ
//=============================
CGame::CGame(MODE mode) : CScene(mode)
{
	// 値のクリア
	m_state = STATE_NONE;
}

//=============================
// デストラクタ
//=============================
CGame::~CGame()
{
}

//=============================
// 初期化処理
//=============================
HRESULT CGame::Init(void)
{
	// ステート初期化
	m_state = STATE_START;

	// プレイヤー生成
	if (m_pPlayer != NULL)return E_FAIL;
	// 使用されていない場合
	m_pPlayer = CPlayer::Create(INIT_D3DVEC3, D3DXVECTOR3(0.0f, D3DX_PI, 0.0f));
	//m_pPlayer = CPlayer::Create(INIT_D3DVEC3, D3DXVECTOR3(0.0f, HALF_D3DXPI, 0.0f));

	CManager::GetSound()->Play(CSound::BGM_GAME);

	// フィールド生成
	for (int nCntLine = 0; nCntLine < MAX_FIELDLINE; nCntLine++)
	{
		if (m_apField[nCntLine] != NULL) return E_FAIL;
		// 使用されていない場合
		m_apField[nCntLine] = CField::Create(FIELD_POS(nCntLine), INIT_D3DVEC3, FIELD_WIDTH, FIELD_DEPTH);
	}

	//// 壁の総数を読み取り
	//int nNumWall = CWall::LoadNumAll(GAMESCRIPTS_PATH);

	//for (int nCntWall = 0; nCntWall < nNumWall; nCntWall++)
	//{ // 壁を生成
	//	m_apWall[nCntWall] = NULL;
	//	m_apWall[nCntWall] = CWall::Create(GAMESCRIPTS_PATH, nCntWall);
	//}

	//// タイマーを生成
	//if (m_pTimer != NULL) return E_FAIL;
	//m_pTimer = CTimer::Create(TIMER_POS, GAME_TIME);

	//// 回収マシンを生成
	//if (m_pMachine != NULL) return E_FAIL;
	//m_pMachine = CMachine::Create();

	//// ポーズを生成
	//if (m_pPause != NULL) return E_FAIL;
	//m_pPause = CPause::Create();

	//// UIを生成
	//if (m_pGameUI != NULL) return E_FAIL;
	//m_pGameUI = CGameUI::Create();

	//// トウモロコシ情報を読み込み
	//CCorn::LoadScripts(GAMESCRIPTS_PATH);
	//
	//// ポップコーン情報読み込み
	//CPopcorn::LoadScripts(GAMESCRIPTS_PATH);

	//// ブロック情報読み込み
	//CBlock::LoadScripts(GAMESCRIPTS_PATH);

	//// 茎情報読み込み
	//float fWidth = m_pField->GetWidth();
	//float fDepth = m_pField->GetDepth();
	//CStem::LoadScripts(GAMESCRIPTS_PATH, fWidth, fDepth);

	//// 飾りを生成
	//CDecoration::Create();

	//// ポップコーン管理のクリア
	//CManager::GetPopcornManager()->Reset();

	// ブロックを生成
	CBlock::LoadSetting(BLOCK_SETTINGPATH);

	// アイテムを生成
	CItem::LoadSetting(ITEM_SETTINGPATH);

	// メッシュスキン生成[平面]
	//CMeshSkinFlat::Create(MESHSKIN_PATH);

	// メッシュスキン生成[円柱]
	//CMeshSkinCylinder::Create(D3DXVECTOR3(0.0f, 0.0f, 300.0f), INIT_D3DVEC3, COLOR_WHITE, 100.0f, 100.0f);
	return S_OK;
}

//=============================
// 終了処理
//=============================
void CGame::Uninit(void)
{
	// 各ポインタのクリア
	m_pPlayer = NULL;	// プレイヤーへのポインタ
	for (int nCntLine = 0; nCntLine < MAX_FIELDLINE; nCntLine++)
	{
		m_apField[nCntLine] = NULL;
	}
	//m_pField = NULL;		// フィールドへのポインタ
	//m_pMachine = NULL;		// マシンへのポインタ
	//m_pTimer = NULL;		// タイマーのクリア
	//m_pPause = NULL;		// ポーズへのポインタ
	//m_pGameUI = NULL;		// UIへのポインタ
	//for (int nCntWall = 0; nCntWall < MAX_GAMEWALL; nCntWall++)
	//{
	//	if (m_apWall[nCntWall] != NULL)
	//	{
	//		m_apWall[nCntWall] = NULL;
	//	}
	//}
}

//=============================
// 更新処理
//=============================
void CGame::Update(void)
{
	// 入力情報取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();
	
	if ((pKeyboard->GetTrigger(DIK_RETURN) == true || pJoypad->GetAnyButton() == true) /* && CManager::GetFadeState() == CFade::STATE_FADENONE && m_state == STATE_END*/)
	{ // リザルトに遷移
		CManager::GetFade()->SetFade(CFade::TYPE_ALPHA, SCREEN_CENTER, COLOR_WHITE, CScene::MODE_RESULT);
	}
}

//=============================
// 描画処理
//=============================
void CGame::Draw(void)
{
}