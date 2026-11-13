//==========================================
// 
// 箱アクション処理[action_box.cpp]
// Author : Kajino Hiyori _Create2026/11/09~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "action_box.h"
#include "player.h"
#include "manager.h"
#include "debugproc.h"

//=============================
// マクロ定義
//=============================

//=============================
// 静的メンバ変数宣言
//=============================
int CBox::m_nNumAll = 0;
const char* CBox::m_pModelName = "data\\MODEL\\action\\action000.x";

//=============================
// コンストラクタ
//=============================
CBox::CBox(CObject::PRIORITY priority) : CAction(priority)
{
	m_nNumAll++;
}

//=============================
// デストラクタ
//=============================
CBox::~CBox()
{

}

//=============================
// アイテム生成処理
//=============================
CBox* CBox::Create(D3DXVECTOR3 pos)
{
	CBox* pBox = NULL;

	if (pBox == NULL)
	{
		pBox = new CBox;	// インスタンス生成

		if (pBox != NULL)
		{ // インスタンス生成に成功
			// アイテムの初期化処理
			pBox->Init(pos);

			return pBox;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CBox::Init(D3DXVECTOR3 pos)
{
	// オブジェクトにタイプを指定
	CAction::Init(pos, CAction::TYPE_BOX, m_pModelName);
	return S_OK;
}

//=============================
// 終了処理
//=============================
void CBox::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// Xオブジェクトの終了処理
	CAction::Uninit();
}

//=============================
// 更新処理
//=============================
void CBox::Update(void)
{
	// アイテムの更新処理
	CAction::Update();

	if (CAction::GetState() != CAction::STATE_ACT) return;

	// アクション処理
	UpdateAction();

	CManager::GetDebugProc()->Print("BoxPos %f %f %f\n", GetPosition().x, GetPosition().y, GetPosition().z);
}

//=============================
// 描画処理
//=============================
void CBox::Draw(void)
{
	// Xオブジェクトの描画処理
	CAction::Draw();
}

#define BOX_THROWING	(D3DXVECTOR3(0.0f, 0.0f, 100.0f))	// 箱の投擲
//=============================
// アクション処理
//=============================
void CBox::UpdateAction(void)
{
	// 他のオブジェクトとの当たり判定
	if (CollisionHitter() == true)
	{// 自身を破棄
		Release();
		return;
	}
	
	// playerからactionになった際に移動量を設定
	if (CAction::GetState() == CAction::GetStateOld())return;
	CAction::SetMove(BOX_THROWING);
}