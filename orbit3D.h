//==========================================
// 
// オービット処理[orbit3D.h]
// Create : 2026/10/21~
// 
//==========================================
#ifndef _ORBIT3D_H_
#define _ORBIT3D_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// マクロ定義
//=============================
#define MAX_ORBIT	(100)	// オービットポイント最大数

//=============================
// オービットクラス
//=============================
class COrbit3D : public CObject
{
public:
	static COrbit3D* Create(D3DXVECTOR3 pos, D3DXCOLOR colTop, D3DXCOLOR colDown, D3DXVECTOR3 rot, float fAngle, float fLength);
	COrbit3D(CObject::PRIORITY priority = CObject::PRIOTITY_ADDALPHA);
	~COrbit3D();
	void SetIdxTexture(int nIdx) { m_nIdxTexture = nIdx; }
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXCOLOR colTop, D3DXCOLOR colDown, D3DXVECTOR3 rot, float fAngle, float fLength);
	void Uninit(void);
	void Update(void) {}
	void Update(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	void Draw(void);
	void SetColor(D3DXCOLOR colTop, D3DXCOLOR colDown);
	static int GetNumAll(void) { return m_nNumAll; }
	void SetPosition(D3DXVECTOR3 pos) {}

private:
	void SetVtx(void);
	void SetIdx(void);
	int m_nIdxTexture;			// 共有テクスチャ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファ
	LPDIRECT3DINDEXBUFFER9	m_pIdxBuff;		// インデックスバッファ
	D3DXVECTOR3 m_pos;						// 発生基準位置
	D3DXVECTOR3 m_posOffset[2];				// 表示位置
	D3DXVECTOR3 m_posPoint[MAX_ORBIT];		// オービット位置
	D3DXCOLOR m_col[2];						// 表示色
	D3DXCOLOR m_colPoint[MAX_ORBIT];		// 表示色
	static int m_nNumAll;					// 数字の総数

	// オフセットからの計算に必要
	D3DXVECTOR3 m_rot;	// 角度
	float m_fAngle;		// 対角線
	float m_fLength;	// 対角線の長さ
};

#endif

