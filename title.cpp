//==========================================
// 
// タイトル処理[title.cpp]
// Author : Kajino Hiyori _Create2026/06/29
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "object2D.h"
#include "title.h"
#include "object2D.h"
#include "result.h"
#include "manager.h"
#include "input.h"
#include "sound.h"
#include "player.h"
#include "field.h"
#include "loadscripts.h"
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
CTitle::CTitle(MODE mode) : CScene(mode)
{
	// 値のクリア
	m_nCounter = 0;
}

//=============================
// デストラクタ
//=============================
CTitle::~CTitle()
{
}

//=============================
// 初期化処理
//=============================
HRESULT CTitle::Init(void)
{
	// BGMの再生
	CSound* pSound = CManager::GetSound();
	pSound->Play(CSound::BGM_TITLE);

	return E_FAIL;
}

//=============================
// 終了処理
//=============================
void CTitle::Uninit(void)
{
	
}

//=============================
// 更新処理
//=============================
void CTitle::Update(void)
{
	// 入力情報取得
	CInputKeyboard* pInpuutKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// フェード情報
	CFade* pFade = CManager::GetFade();

	// 遷移
	if ((pInpuutKeyboard->GetAnyTrigger() == true 
		|| CManager::GetInputJoypad()->GetAnyButton() == true
		|| CManager::GetInputMouse()->GetTrigger(CInputMouse::BUTTON_LEFT) == true))
	{
		pFade->SetFade(CFade::TYPE_ALPHA, SCREEN_CENTER, COLOR_BLACK, CScene::MODE_GAME);
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
void CTitle::Draw(void)
{
}