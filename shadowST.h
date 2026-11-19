//==========================================
// 
// ステンシルシャドウ処理[shadowST.h]
// Author : Kajino Hiyori _Create2026/11/09~
// 
//==========================================
#ifndef _SHADOWST_H_
#define _SHADOWST_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "objectX.h"
#include "loadscripts.h"

//=============================
// マクロ定義
//=============================

//=============================
// 前方宣言
//=============================
class CPolygon2D;	// ポリゴン2Dクラス

//=============================
// ステンシルシャドウクラス
//=============================
class CShadowST : public CObjectX
{
public:
	CShadowST(CObject::PRIORITY priority = CObject::PRIORITY_STENCIL);
	~CShadowST();
	static CShadowST* Create(D3DXVECTOR3 pos, float fRadius);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, float fRadius);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { CObjectX::SetPosition(pos); }
	static void UninitPolygon(void);
	static void DrawStencil(void);

private:
	float m_fRadius;	// 半径[拡大率]
	float m_fHeight;	// 高さ[拡大率]
	static CPolygon2D* m_pShadow;	// 画面全体に貼るポリゴン情報
	static int m_nNumAll;	// 総数
	static const char* m_pModelName;	// モデル名
};

#endif