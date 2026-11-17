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
#include "polygonField.h"
#include "field.h"
#include "camera.h"
#include "debugproc.h"
#include "loadscripts.h"
#include "effect.h"

//=============================
// マクロ定義
//=============================
#define WARNING_Y		(1.0f)				// 注意表示UIの高度
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
const char* CField::m_apFilePath[NUM_FIELDTEXTURE] = 
{
	"data\\TEXTURE\\field\\field.jpg",	// 通常時の地面
	"data\\TEXTURE\\field\\field_warning.jpg",	// 注意表示の地面
};

//=============================
// コンストラクタ
//=============================
CField::CField(CObject::PRIORITY priority) : CObject3D(priority)
{
	m_pWarning = NULL;			// 注意用ポリゴン
	m_state = STATE_NONE;		// 現在の状態
	m_warning = WARNING_ADD;	// 現在の状態
	m_nIdx = -1;				// インデックス
	m_nStateCounter = 0;		// ステート管理
	m_fColorAlpha = 0.0f;		// 現在の透明度
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
	CObject3D::SetTextureIdx(CManager::GetTexture()->Register(m_apFilePath[0]));
	CObject3D::SetTextureRange(0.25f, fDepth / fWidth * 0.25f);

	// 注意表示ポリゴン生成
	m_pWarning = CPolygonField::Create(D3DXVECTOR3(pos.x, pos.y + WARNING_Y, pos.z), rot, COLOR_WHITE, fWidth, fWidth * 2.0f, CPolygonField::CENTER_BUTTON);
	m_pWarning->SetTextureIdx(CManager::GetTexture()->Register(m_apFilePath[1]));
	m_pWarning->SetColor(COLOR_WHITE_ZERO);
	
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

	// 注意表示ポリゴンの破棄
	SAFE_UNINIT(m_pWarning);
}

//=============================
// 更新処理
//=============================
void CField::Update(void)
{
	if (CManager::GetInputKeyboard()->GetTrigger(DIK_0) == true)
	{
		SetState(STATE_WARNING);
	}

	if (m_state != STATE_WARNING) return;
	// 注意喚起更新処理
	UpdateWarning();
}

//=============================
// 描画処理
//=============================
void CField::Draw(void)
{
	// 3Dオブジェクトの描画処理
	CObject3D::Draw();

	if (m_pWarning == NULL || m_state == STATE_NONE) return;
	m_pWarning->Draw();
}

//=============================
// 高度取得
//=============================
float CField::GetHeight(D3DXVECTOR3 pos)
{
	return CObject3D::GetPosition().y;
}

//=============================
// ステート更新処理
//=============================
void CField::SetState(STATE state)
{
	m_state = state; 
	m_nStateCounter = 0; 
	m_pWarning->SetPosition(CObject3D::GetPosition());
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
			if (pField->GetPosition().x - pField->GetWidth() <= pos.x
				&& pField->GetPosition().x + pField->GetWidth() >= pos.x
				&& pField->GetPosition().z <= pos.z 
				&& pField->GetPosition().z + pField->GetDepth() >= pos.z)
			{
				return pField->m_nIdx;
			}
			pObject = pObjectNext;	// NULL判定を行う
		}
	}
	return -1;
}

#define CHANGE_COLOR	(0.05f)	// 注意喚起の色変更
#define BREAK_TIME		(300)	// 注意喚起が出てから壊れるまでの時間
#define SHAKE_TIME		(1.0f)	// 画面振動幅
#define SHAKE_TIME		(120)	// 振動時間
//=============================
// 注意喚起更新処理
//=============================
void CField::UpdateWarning(void)
{
	// カウンターを加算して状態を管理
	m_nStateCounter++;
	if (m_nStateCounter >= BREAK_TIME)
	{ // 破壊状態に遷移
		// 位置を更新
		D3DXVECTOR3 pos = CObject3D::GetPosition();
		pos.z += m_pWarning->GetDepth();
		CObject3D::SetPosition(pos);

		// 奥行きを更新
		float fDepth = CObject3D::GetDepth();
		fDepth -= m_pWarning->GetDepth();
		CObject3D::SetDepth(fDepth);

		// 通常状態に変更
		m_state = STATE_NONE;

		// テクスチャ読み込み幅を変更
		CObject3D::SetTextureRange(0.25f, fDepth / CObject3D::GetWidth() * 0.25f);

		CManager::GetRenderer()->SetShake(SHAKE_TIME, SHAKE_TIME);
	}

	// 注意喚起UIの更新処理
	if (m_pWarning == NULL) return;
	D3DXCOLOR col = m_pWarning->GetColor();
	switch (m_warning)
	{
	case WARNING_ADD:	// 不透明化
		col.a += CHANGE_COLOR;
		if (col.a >= 1.0f)
		{
			col.a = 1.0f;
			m_warning = WARNING_DIS;
		}
		break;

	case WARNING_DIS:	// 透明化
		col.a -= CHANGE_COLOR;
		if (col.a <= 0.0f)
		{
			col.a = 0.0f;
			m_warning = WARNING_ADD;
		}
		break;
	}
	m_pWarning->SetColor(col);
}