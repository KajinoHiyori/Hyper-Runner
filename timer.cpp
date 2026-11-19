//==========================================
// 
// タイマー処理[timer.h]
// Author : Kajino Hiyori _Create2026/05/18~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "timer.h"
#include "input.h"
#include "manager.h"
#include "renderer.h"
#include "number2D.h"
#include "scene.h"
#include "title.h"
#include "game.h"
#include "result.h"

//=============================
// マクロ定義
//=============================
#define TIMER_SIZE		(20.0f)	// タイマーのサイズ
#define TIMER_FRAME		(60)	// タイマー処理のフレーム数
#define TIMER_ORANGE	(30)	// 残り時間に応じて色変更
#define TIMER_RED		(10)	// 残り時間に応じて色変更

//=============================
// コンストラクタ
//=============================
CTimer::CTimer(CObject::PRIORITY priority) : CObject(priority)
{
	// 値のクリア
	memset(&m_apNumber[0], NULL, sizeof m_apNumber);	// ポインタのクリア
	m_nTimer = 0;	// タイマー管理の値をクリア
	m_nFrame = 0;	// フレーム管理の値をクリア
	memset(&m_aTecU[0], 0, sizeof m_aTecU);	// タイマーのUV座標を管理
	m_bOperate = true;	// 稼働状態の初期化
}

//=============================
// デストラクタ
//=============================
CTimer::~CTimer()
{

}

//=============================
// タイマー生成処理
//=============================
CTimer* CTimer::Create(D3DXVECTOR3 pos, int nTimer)
{
	CTimer* pTimer = NULL;

	if (pTimer == NULL)
	{
		pTimer = new CTimer;
		if (pTimer != NULL)
		{ // インスタンス生成に成功
			// 弾の初期化処理
			pTimer->Init(pos, nTimer);

			return pTimer;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CTimer::Init(D3DXVECTOR3 pos, int nTimer)
{
	for (int nCntTimer = 0; nCntTimer < TIMER_DIGIT; nCntTimer++)
	{
		// 数字の用意
		if (m_apNumber[nCntTimer] == NULL)
		{ // 初期化が行われている場合
			m_apNumber[nCntTimer] = m_apNumber[nCntTimer]->Create(D3DXVECTOR3(pos.x + nCntTimer * NUMBER_WIDTH, pos.y, pos.z), 0);
		}
		else
		{ // 初期化に失敗
			return E_FAIL;
		}
		// オブジェクトにタイプを指定
		SetType(TYPE_TIMER);
		m_nTimer = nTimer;
		// 表示桁を管理
		SetTexU();
	}

	for (int nTimer = 0; nTimer < TIMER_DIGIT; nTimer++)
	{
		m_apNumber[nTimer]->SetNum(m_aTecU[nTimer]);
	}

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CTimer::Uninit(void)
{
	for (int nCntTimer = 0; nCntTimer < TIMER_DIGIT; nCntTimer++)
	{
		// number2Dの終了処理
		SAFE_UNINIT(m_apNumber[nCntTimer]);
	}
}

//=============================
// 更新処理
//=============================
void CTimer::Update(void)
{
	// シーン情報取得
	CScene::MODE mode = CManager::GetMode();

	switch (mode)
	{
	case CScene::MODE_TITLE:	// タイトル
		break;

	//case CScene::MODE_TUTORIAL:	// チュートリアル
	//	
	//	break;

	case CScene::MODE_GAME:		// ゲーム
		// ゲーム処理
		UpdateGame();
		break;

	case CScene::MODE_RESULT:	// リザルト
		break;

	case CScene::MODE_RANKING:	// ランキング
		break;

	//case CScene::MODE_CREDIT:	// クレジット
	//	break;
	}
}

//=============================
// 描画処理
//=============================
void CTimer::Draw(void)
{
	for (int nCntTimer = 0; nCntTimer < TIMER_DIGIT; nCntTimer++)
	{
		// number2Dの描画処理
		m_apNumber[nCntTimer]->Draw();
	}
}

//=============================
// 表示桁を管理
//=============================
void CTimer::SetTexU(void)
{
	for (int nCntTimer = 0; nCntTimer < TIMER_DIGIT; nCntTimer++)
	{
		if (nCntTimer == TIMER_DIGIT - 1)
		{
			m_aTecU[nCntTimer] = m_nTimer % 10 / 1;
		}
		else
		{
			float fNumX = powf(static_cast <float>(10), static_cast <float>(TIMER_DIGIT - nCntTimer));		// 剰余算
			float fNumY = powf(static_cast <float>(10), static_cast <float>(TIMER_DIGIT - nCntTimer - 1));	// 除算
			m_aTecU[nCntTimer] = m_nTimer % static_cast<int>(fNumX) / static_cast<int>(fNumY);
		}
	}
}

//=============================
// チュートリアル処理
//=============================
void CTimer::UpdateTutorial(void)
{
	// 時間の更新無し
}

//=============================
// ゲーム処理
//=============================
void CTimer::UpdateGame(void)
{
	CGame* pGame = static_cast<CGame*>(CManager::GetScene());
	//if (pGame->GetState() != CGame::STATE_GAMING)
	//{ // ゲーム中でない場合は早期リターン
	//	return;
	//}

	//if (pGame->GetEnablePause() == true || pGame->GetState() != CGame::STATE_GAMING) return;
	// タイマー稼働中&&ポーズ状態でない
	m_nFrame++;	// 処理を管理するフレームを加算

	if (m_nFrame > TIMER_FRAME)
	{ // タイマーの値を減少
		m_nFrame = 0;
		m_nTimer--;
	}

	if (m_nTimer <= 0)
	{ // タイマーが0
		m_nTimer = 0;
		// 終了設定
		m_bOperate = false;
		pGame->SetState(CGame::STATE_CLEAR);
	}

	// 1ポリゴンずつに表示する数字の桁を更新
	SetTexU();
	for (int nTimer = 0; nTimer < TIMER_DIGIT; nTimer++)
	{
		// 規定時間に達したら色を変更する
		if (m_nTimer <= TIMER_RED)
		{ // 赤時間に突入
			m_apNumber[nTimer]->SetColor(COLOR_RED);
		}
		else if (m_nTimer <= TIMER_ORANGE)
		{ // 橙時間に突入
			m_apNumber[nTimer]->SetColor(COLOR_ORANGE);
		}
		m_apNumber[nTimer]->SetNum(m_aTecU[nTimer]);
	}
	
}
