//==========================================
// 
// 加速アイテム処理[item_speedup.cpp]
// Author : Kajino Hiyori _Create2026/11/09~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "item_speedup.h"
#include "player.h"

//=============================
// マクロ定義
//=============================

//=============================
// 静的メンバ変数宣言
//=============================
int CSpeedUP::m_nNumAll = 0;
const char* CSpeedUP::m_pModelName = "data\\MODEL\\item\\item000.x";

//=============================
// コンストラクタ
//=============================
CSpeedUP::CSpeedUP(CObject::PRIORITY priority) : CItem(priority)
{
	m_nNumAll++;
}

//=============================
// デストラクタ
//=============================
CSpeedUP::~CSpeedUP()
{

}

//=============================
// アイテム生成処理
//=============================
CSpeedUP* CSpeedUP::Create(D3DXVECTOR3 pos)
{
	CSpeedUP* pSpeed = NULL;

	if (pSpeed == NULL)
	{
		pSpeed = new CSpeedUP;	// インスタンス生成

		if (pSpeed != NULL)
		{ // インスタンス生成に成功
			// アイテムの初期化処理
			pSpeed->Init(pos);

			return pSpeed;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CSpeedUP::Init(D3DXVECTOR3 pos)
{
	// オブジェクトにタイプを指定
	CItem::Init(pos, CItem::TYPE_SPEEDUP, m_pModelName);
	return S_OK;
}

//=============================
// 終了処理
//=============================
void CSpeedUP::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// Xオブジェクトの終了処理
	CItem::Uninit();
}

//=============================
// 更新処理
//=============================
void CSpeedUP::Update(void)
{
	// アイテムの更新処理
	CItem::Update();

	CPlayer* pPlayer = CItem::CollisionPlayer();

	if (pPlayer == NULL) return;
	// 自身を破棄
	Release(pPlayer);
}

//=============================
// 描画処理
//=============================
void CSpeedUP::Draw(void)
{
	// Xオブジェクトの描画処理
	CItem::Draw();
}

//=============================
// 描画処理
//=============================
void CSpeedUP::Release (CPlayer* pPlayer)
{
	pPlayer->ChangeSpeed(true);

	// アイテムを破棄
	CItem::Release();
}