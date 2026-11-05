//==========================================
// 
// ゲーム処理[result.h]
// Author : Kajino Hiyori _Create2026/06/29
// 
//==========================================
//=============================
// インクルードファイル
//=============================
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
// 静的メンバ変数宣言
//=============================

//=============================
// マクロ定義
//=============================

//=============================
// コンストラクタ
//=============================
CResult::CResult(MODE mode) : CScene(mode)
{
	// 値のクリア
	m_state = STATE_NONE;
}

//=============================
// デストラクタ
//=============================
CResult::~CResult()
{
}

//=============================
// 初期化処理
//=============================
HRESULT CResult::Init(void)
{
	CManager::GetSound()->Play(CSound::BGM_RESULT);

	// ステート初期化
	m_state = STATE_STAGING;

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CResult::Uninit(void)
{
	
}

//=============================
// 更新処理
//=============================
void CResult::Update(void)
{
	// 入力情報取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();
	
	if ((pKeyboard->GetAnyTrigger() == true || pJoypad->GetAnyButton() == true) && CManager::GetFadeState() == CFade::STATE_FADENONE)
	{ // ランキングに遷移
		CManager::GetFade()->SetFade(CFade::TYPE_ALPHA, SCREEN_CENTER, COLOR_WHITE, CScene::MODE_RANKING);
		CManager::GetSound()->Play(CSound::SE_ENTER);
	}
}

//=============================
// 描画処理
//=============================
void CResult::Draw(void)
{
}