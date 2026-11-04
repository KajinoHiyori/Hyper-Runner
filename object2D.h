//==========================================
// 
// 2Dオブジェクト処理[object2D.h]
// Author : Kajino Hiyori _Create2026/05/08~
// 
//==========================================
#ifndef _OBJECT2D_H_
#define _OBJECT2D_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// 2Dオブジェクトクラス
//=============================
class CObject2D : public CObject
{
public:
	CObject2D(CObject::PRIORITY priority = CObject::PRIORITY_UI);
	~CObject2D();
	static CObject2D* Create(D3DXVECTOR3 pos, float fHeight, float fWidth);
	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos, float fHeight, float fWidth);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetTexture(const char* pFileName);
	void BindTexture(LPDIRECT3DTEXTURE9 pTexture) { m_pTexture = pTexture; }
	void SetPosition(D3DXVECTOR3 pos);
	void SetRotation(D3DXVECTOR3 rot);
	void SetColor(D3DXCOLOR col);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	float GetLength(void) { return m_fLength; }
	float GetAngle(void) { return m_fAngle; }
	D3DXCOLOR GetColor(void) { return m_col; }
private:
	void SetVtx(void);
	D3DXVECTOR3 m_pos;			// 表示位置
	D3DXVECTOR3 m_rot;			// 向き
	D3DXCOLOR m_col;			// 色
	float m_fWidth;				// 横幅
	float m_fHeight;			// 縦幅
	float m_fLength;			// 対角線の長さ
	float m_fAngle;				// 対角線の長さ
	int m_nIdxTexture;			// テクスチャインデックス
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファ	
	LPDIRECT3DTEXTURE9 m_pTexture;			// テクスチャへのポインタ
};

#endif
