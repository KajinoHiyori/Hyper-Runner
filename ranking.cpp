//==========================================
// 
// タイトル処理[ranking.cpp]
// Author : Kajino Hiyori _Create2026/06/29
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "object2D.h"
#include "ranking.h"
#include "ranking_ui.h"
#include "object2D.h"
#include "result.h"
#include "manager.h"
#include "input.h"
#include "sound.h"
#include "player.h"
#include "field.h"
#include "wall.h"
#include "stem.h"
#include "corn.h"
#include "popcorn.h"
#include "loadscripts.h"
#include "ranking_online.h"
#include "ranking_offline.h"
#include "block.h"
#include "machine.h"
#include "timer.h"
#include "particle.h"
#include "meshcylinder.h"
#include "decoration.h"
#include "debugproc.h"
#include "popcornmanager.h"

//=============================
// マクロ定義
//=============================
#define NEXT_MODE		(240)	//	次のモードに遷移するまでの間隔
#define VIBRATION		(3000)	// 振動強度
#define DECIDE_VIBTIME	(10)	// 選択時の振動時間

//=============================
// 静的メンバ変数宣言
//=============================
CPlayer* CRanking::m_pPlayer = NULL;	// プレイヤーへのポインタ
CField* CRanking::m_pField = NULL;	// フィールドへのポインタ
CWall* CRanking::m_apWall[MAX_RANKINGWALL] = {};	// 壁のポインタ
CRankingUI* CRanking::m_pRankingUI = NULL;	// ランキングUIへのポインタ

//=============================
// コンストラクタ
//=============================
CRanking::CRanking(MODE mode) : CScene(mode)
{
	// 値のクリア
	m_nCounter = 0;
}

//=============================
// デストラクタ
//=============================
CRanking::~CRanking()
{
}

//=============================
// 初期化処理
//=============================
HRESULT CRanking::Init(void)
{
	// BGMの再生
	CSound* pSound = CManager::GetSound();
	pSound->Play(CSound::BGM_RANKING);

	// プレイヤー生成
	if (m_pPlayer != NULL)return E_FAIL;
	// 使用されていない場合
	m_pPlayer = CPlayer::Create(RANKINGSCRIPTS_PATH);

	// フィールド生成
	if (m_pField != NULL) return E_FAIL;
	// 使用されていない場合
	m_pField = CField::Create(INIT_D3DVEC3, INIT_D3DVEC3);

	// 壁の総数を読み取り
	int nNumWall = CWall::LoadNumAll(RANKINGSCRIPTS_PATH);

	for (int nCntWall = 0; nCntWall < nNumWall; nCntWall++)
	{ // 壁を生成
		m_apWall[nCntWall] = NULL;
		m_apWall[nCntWall] = CWall::Create(RANKINGSCRIPTS_PATH, nCntWall);
	}

	// ブロック情報読み込み
	CBlock::LoadScripts(RESULTSCRIPTS_PATH);

	// 茎の生成
	CStem::Reset(m_pField->GetWidth(), m_pField->GetDepth());
	CManager::GetSound()->Stop(CSound::SE_GROW);

	// 飾りを生成
	CDecoration::Create();

	// ランキングUIの表示
	if (m_pRankingUI != NULL) return E_FAIL;
	m_pRankingUI = CRankingUI::Create();
	
	return E_FAIL;
}

//=============================
// 終了処理
//=============================
void CRanking::Uninit(void)
{
	// 値のクリア
		// 各ポインタのクリア
	m_pPlayer = NULL;	// プレイヤーへのポインタ
	m_pField = NULL;		// フィールドへのポインタ
	for (int nCntWall = 0; nCntWall < MAX_RESULTWALL; nCntWall++)
	{
		if (m_apWall[nCntWall] != NULL)
		{
			m_apWall[nCntWall] = NULL;
		}
	}
	m_pRankingUI = NULL;
}

//=============================
// 更新処理
//=============================
void CRanking::Update(void)
{
	// 入力情報取得
	CInputKeyboard* pInpuutKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// フェード情報
	CFade* pFade = CManager::GetFade();

	// カウンター加算
	m_nCounter++;

	// 遷移
	if ((pInpuutKeyboard->GetAnyTrigger() == true 
		|| pJoypad->GetAnyButton() == true
		|| CManager::GetInputMouse()->GetTrigger(CInputMouse::BUTTON_LEFT) == true
		|| m_nCounter >= NEXT_MODE) 
		&& CManager::GetFadeState() == CFade::STATE_FADENONE)
	{
		pFade->SetFade(CFade::TYPE_ALPHA, SCREEN_CENTER, COLOR_BLACK, CScene::MODE_TITLE);
		CManager::GetSound()->Play(CSound::SE_ENTER);
		int nLeft = pJoypad->GetVibrationLeft();
		int nRight = pJoypad->GetVibrationRight();
		nLeft += VIBRATION;
		nRight += VIBRATION;
		pJoypad->SetVibration(nLeft, nRight, DECIDE_VIBTIME);
	}
}

//=============================
// 描画処理
//=============================
void CRanking::Draw(void)
{
}