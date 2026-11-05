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
#include "object2D.h"
#include "result.h"
#include "manager.h"
#include "input.h"
#include "sound.h"
#include "player.h"
#include "field.h"
#include "loadscripts.h"
#include "ranking_offline.h"
#include "particle.h"
#include "meshcylinder.h"
#include "debugproc.h"

//=============================
// マクロ定義
//=============================
#define NEXT_MODE		(240)	//	次のモードに遷移するまでの間隔
#define VIBRATION		(3000)	// 振動強度
#define DECIDE_VIBTIME	(10)	// 選択時の振動時間

//=============================
// 静的メンバ変数宣言
//=============================

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

	return E_FAIL;
}

//=============================
// 終了処理
//=============================
void CRanking::Uninit(void)
{
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