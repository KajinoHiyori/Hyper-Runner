//==========================================
// 
// フィールド処理[field.h]
// Author : Kajino Hiyori _Create2026/09/28~
// 
//==========================================
#ifndef _FIELD_H_
#define _FIELD_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object3D.h"
#include "meshfield.h"

//=============================
// マクロ定義
//=============================
#define NUM_FIELDTEXTURE	(2)	// フィールド関連のテクスチャ数

//=============================
// 前方宣言
//=============================
class CPolygonField;	// 地面ポリゴン

//=============================
// フィールドクラス
//=============================
class CField : public CObject3D
{
public:
	// フィールドの状態
	typedef enum
	{
		STATE_NONE = 0,	// 通常
		STATE_WARNING,	// 注意喚起
		STATE_MAX
	}STATE;

	// 注意喚起UIの状態
	typedef enum
	{
		WARNING_ADD = 0,	// 不透明化
		WARNING_DIS,		// 透明化
		WARNING_MAX
	}WARNING;

	CField(CObject::PRIORITY priority = CObject::PRIORITY_FIELD);
	~CField();
	static CField* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { CObject3D::SetPosition(pos); }
	float GetHeight(D3DXVECTOR3 pos);
	float GetWidth(void) { return CObject3D::GetWidth(); }
	float GetDepth(void) { return CObject3D::GetDepth(); }
	void SetState(STATE state);
	STATE GetState(void) { return m_state; }
	static int GetNumAll(void) { return m_nNumAll; }
	static int GetIndex(D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosition(void) { return CObject3D::GetPosition(); }

private:
	void UpdateWarning(void);
	CPolygonField* m_pWarning;	// 注意用ポリゴン
	STATE m_state;	// 現在の状態
	WARNING m_warning;	// 注意喚起管理
	float m_fColorAlpha;	// 色
	int m_nStateCounter;	// ステート管理
	int m_nIdx;	// インデックス
	static int m_nNumAll;	// フィールドの総数
	static const char* m_apFilePath[NUM_FIELDTEXTURE];	// テクスチャパス
};
#endif