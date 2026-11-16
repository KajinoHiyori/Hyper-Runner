//==========================================
// 
// マグマ処理[magma.h]
// Author : Kajino Hiyori _Create2026/11/16~
// 
//==========================================
#ifndef _MAGMA_H_
#define _MAGMA_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "objectField.h"

//=============================
// マクロ定義
//=============================

//=============================
// 前方宣言
//=============================

//=============================
// マグマクラス
//=============================
class CMagma : public CObjectField
{
public:
	// テクスチャの種類
	typedef enum
	{
		TYPE_000 = 0,	// テクスチャ000
		TYPE_001,		// テクスチャ001
		TYPE_MAX
	}TYPE;

	CMagma(CObject::PRIORITY priority = CObject::PRIORITY_MAGMA);
	~CMagma();
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) {}
	float GetHeight(D3DXVECTOR3 pos) { return CObjectField::GetHeight(pos); }
	static CMagma* Create(D3DXVECTOR3 pos);
	static int GetNumAll(void) { return m_nNumAll; }

private:
	static const char* m_apFilenameType[TYPE_MAX];		// テクスチャ名
	static int m_nNumAll;	// マグマの総数
};

#endif