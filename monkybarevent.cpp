//==========================================
// 
// 雲梯一括管理処理[monkybarevent.cpp]
// Author : Kajino Hiyori
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "monkybar.h"
#include "monkybarevent.h"

//=============================
// マクロ定義
//=============================

//=============================
// コンストラクタ
//=============================
CMonkyBarEvent::CMonkyBarEvent(CObject::PRIORITY priority) : CObject(priority)
{
	for (int nCntBar = 0; nCntBar < MAX_MONKYBAR; nCntBar++)
	{ // バーのクリア
		m_apBar[nCntBar] = NULL;
	}
}

//=============================
// デストラクタ
//=============================
CMonkyBarEvent::~CMonkyBarEvent()
{

}

//=============================
// 生成処理
//=============================
CMonkyBarEvent* CMonkyBarEvent::Create(D3DXVECTOR3 pos)
{
	CMonkyBarEvent* pMonkyBarEvent = NULL;

	if (pMonkyBarEvent == NULL)
	{
		pMonkyBarEvent = new CMonkyBarEvent;	// インスタンス生成

		if (pMonkyBarEvent != NULL)
		{ // インスタンス生成に成功
			// 雲梯の初期化処理
			pMonkyBarEvent->Init(pos);

			return pMonkyBarEvent;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

#define BAR_DEPTH	(10.0f)	// バー同士の間隔
//=============================
// 初期化処理
//=============================
HRESULT CMonkyBarEvent::Init(D3DXVECTOR3 pos)
{
	// バーの用意[手前からインデックスが小さくなる]
	for (int nCntBar = 0; nCntBar < MAX_MONKYBAR; nCntBar++)
	{
		if (m_apBar[nCntBar] != NULL) continue;
		m_apBar[nCntBar] = CMonkyBar::Create(D3DXVECTOR3(pos.x, pos.y, pos.x + nCntBar * BAR_DEPTH));
	}

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CMonkyBarEvent::Uninit(void)
{
	for (int nCntBar = 0; nCntBar < MAX_MONKYBAR; nCntBar++)
	{ // バーのクリア
		m_apBar[nCntBar] = NULL;
	}
}

//=============================
// 更新処理
//=============================
void CMonkyBarEvent::Update(void)
{

}

//=============================
// 描画処理
//=============================
void CMonkyBarEvent::Draw(void)
{

}

//=============================
// 破棄処理
//=============================
void  CMonkyBarEvent::Release(void)
{
	for (int nCntBar = 0; nCntBar < MAX_MONKYBAR; nCntBar++)
	{ // バーの破棄
		if (m_apBar[nCntBar] == NULL) continue;
		m_apBar[nCntBar]->Release();
	}

	// 自身の破棄
	CObject::Release();
}
