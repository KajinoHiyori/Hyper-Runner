//==========================================
// 
// 減速アイテム処理[item_speeddown.cpp]
// Author : Kajino Hiyori _Create2026/11/09~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "item_speeddown.h"
#include "player.h"

//=============================
// マクロ定義
//=============================

//=============================
// 静的メンバ変数宣言
//=============================
int CSpeedDown::m_nNumAll = 0;
const char* CSpeedDown::m_pModelName = "data\\MODEL\\item\\item001.x";

//=============================
// コンストラクタ
//=============================
CSpeedDown::CSpeedDown(CObject::PRIORITY priority) : CItem(priority)
{
	m_nNumAll++;
}

//=============================
// デストラクタ
//=============================
CSpeedDown::~CSpeedDown()
{

}

//=============================
// アイテム生成処理
//=============================
CSpeedDown* CSpeedDown::Create(D3DXVECTOR3 pos)
{
	CSpeedDown* pSpeed = NULL;

	if (pSpeed == NULL)
	{
		pSpeed = new CSpeedDown;	// インスタンス生成

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
HRESULT CSpeedDown::Init(D3DXVECTOR3 pos)
{
	// オブジェクトにタイプを指定
	CItem::Init(pos, CItem::TYPE_SPEEDDOWN, m_pModelName);
	return S_OK;
}

//=============================
// 終了処理
//=============================
void CSpeedDown::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// Xオブジェクトの終了処理
	CItem::Uninit();
}

//=============================
// 更新処理
//=============================
void CSpeedDown::Update(void)
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
void CSpeedDown::Draw(void)
{
	// Xオブジェクトの描画処理
	CItem::Draw();
}

//=============================
// 描画処理
//=============================
void CSpeedDown::Release (CPlayer* pPlayer)
{
	pPlayer->ChangeSpeed(false);

	// アイテムを破棄
	CItem::Release();
}