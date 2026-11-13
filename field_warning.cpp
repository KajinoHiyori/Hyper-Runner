//==========================================
// 
// フィールド処理[field.h]
// Author : Kajino Hiyori _Create2026/09/28~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "input.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "scene.h"
#include "game.h"
#include "object3D.h"
#include "field.h"
#include "camera.h"
#include "debugproc.h"
#include "loadscripts.h"

//=============================
// マクロ定義
//=============================
#define FIELDSET		("FIELDSET")		// FIELDSET
#define END_FIELDSET	("END_FIELDSET")	// END_FIELDSET
#define TEXTURENAME		("TEXTURENAME")		// TEXTURENAME
#define POS				("POS")				// POS
#define ROT				("ROT")				// ROT
#define WIDTH			("WIDTH")			// WIDTH
#define DEPTH			("DEPTH")			// DEPTH
#define DIVISION		("DIVISION")		// DIVISION
#define MAXHEIGHT		("MAXHEIGHT")		// MAXHEIGHT
#define HEIGHTVALUE		("HEIGHTVALUE")		// HEIGHTVALUE
#define TEXTURERANGE	("TEXTURERANGE")	// TEXTURERANGE
#define FIELDTYPE		("FIELDTYPE")		// FIELDTYPE

//=============================
// 静的メンバ変数宣言
//=============================
int CField::m_nNumAll = 0;
const char* CField::m_pFilePath = "data\\TEXTURE\\field.jpg";

//=============================
// コンストラクタ
//=============================
CField::CField(CObject::PRIORITY priority) : CObject3D(priority)
{
	m_nNumAll++;
}

//=============================
// デストラクタ
//=============================
CField::~CField()
{

}

//=============================
// フィールド生成処理
//=============================
CField* CField::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth)
{
	CField* pField = NULL;

	if (pField == NULL)
	{
		pField = new CField;	// インスタンス生成

		if (pField != NULL)
		{ // インスタンス生成に成功
			// フィールドの初期化処理
			pField->Init(pos, rot, fWidth, fDepth);

			return pField;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CField::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth)
{
	// オブジェクト初期化
	CObject3D::Init(pos, rot, COLOR_WHITE, fWidth, fDepth, CObject3D::CENTER_BUTTON);

	// テクスチャ指定
	CObject3D::SetTextureIdx(CManager::GetTexture()->Register(m_pFilePath));
	CObject3D::SetTextureRange(0.5f, fDepth / fWidth * 0.5f);
	
	// オブジェクトにタイプを指定
	CObject3D::SetType(CObject::TYPE_FIELD);

	// インデックスを指定
	m_nIdx = m_nNumAll - 1;

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CField::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// 3Dオブジェクトの終了処理
	CObject3D::Uninit();
}

//=============================
// 更新処理
//=============================
void CField::Update(void)
{
	CManager::GetDebugProc()->Print("pos %f %f %f\n", CObject3D::GetPosition().x, CObject3D::GetPosition().y, CObject3D::GetPosition().z);
}

//=============================
// 描画処理
//=============================
void CField::Draw(void)
{
	// 3Dオブジェクトの描画処理
	CObject3D::Draw();
}

//=============================
// 高度取得
//=============================
float CField::GetHeight(D3DXVECTOR3 pos)
{
	return CObject3D::GetPosition().y;
}

//=============================
// 判定面のインデックスを獲得
//=============================
int CField::GetIndex(D3DXVECTOR3 pos)
{
	for (int nCntPriority = 0; nCntPriority < MAX_PRIORITYS; nCntPriority++)
	{
		CObject* pObject = GetTop(nCntPriority);	// 先頭アドレスを取得

		while (pObject != NULL)
		{ // 次のオブジェクトがある限りループ
			CObject* pObjectNext = pObject->GetNext();	// 次のオブジェクトへのポインタを設定
			bool bOneCollision = false;

			// オブジェクトとの当たり判定
			if (pObject->GetType() != CObject::TYPE_FIELD)
			{
				pObject = pObjectNext;	// NULL判定を行う
				continue;
			}
			// アイテムの場合
			CField* pField = static_cast<CField*>(pObject);

			// 当たり判定処理を実行
			if (pField->GetPosition().x - pField->GetWidth() <= pos.x && pField->GetPosition().x + pField->GetWidth() >= pos.x)
			{
				return pField->m_nIdx;
			}
			pObject = pObjectNext;	// NULL判定を行う
		}
	}
	return -1;
}
