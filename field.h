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
class CField : public CMeshField
{
public:
	CField(CObject::PRIORITY priority = CObject::PRIORITY_FIELD);
	~CField();
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { CMeshField::SetPosition(pos); }
	float GetHeight(D3DXVECTOR3 pos);
	D3DXVECTOR3 GetNormalize(D3DXVECTOR3 pos) { return CMeshField::GetNormalize(); }
	float GetWidth(void) { return CMeshField::GetWidth(); }
	float GetDepth(void) { return CMeshField::GetDepth(); }
	static CField* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	static int GetNumAll(void) { return m_nNumAll; }
	D3DXVECTOR3 GetPosition(void) { return CMeshField::GetPosition(); }
	void Dent(D3DXVECTOR3 pos, float fSize, float fDepth) { CMeshField::Dent(pos, fSize, fDepth); }
	void Bump(D3DXVECTOR3 pos, float fSize, float fPeak) { CMeshField::Bump(pos, fSize, fPeak); }

private:
	HRESULT LoadScripts(const char* pFileName);
	static int m_nNumAll;	// フィールドの総数
};
#endif