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

//=============================
// 前方宣言
//=============================

//=============================
// フィールドクラス
//=============================
class CField : public CObject3D
{
public:
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
	static int GetNumAll(void) { return m_nNumAll; }
	D3DXVECTOR3 GetPosition(void) { return CObject3D::GetPosition(); }

private:
	static int m_nNumAll;	// フィールドの総数
};
#endif