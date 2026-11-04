//========================================================
// 
// メッシュシリンダー処理[meshcylinder.h]
// Author : Kajino Hiyori _Create2026/07/21
//
//========================================================
#ifndef _MESHCYLINDER_H_
#define	_MESHCYLINDER_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// メッシュシリンダークラス
//=============================
class CMeshCylinder : public CObject
{
public:
	CMeshCylinder(CObject::PRIORITY priority = CObject::PRIOTITY_ADDALPHA);
	~CMeshCylinder();
	static CMeshCylinder* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fRadius, float fHeight, int nWave = 6);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fRadius, float fHeight, int nWave);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; }

private:
	void SetVtx(void);
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;	// インデックスバッファへのポインタ
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	D3DXVECTOR3 m_pos;		// 位置
	D3DXVECTOR3 m_rot;		// 向き
	D3DXCOLOR m_col;		// 色
	float m_fRadius;		// 半径
	float m_fHeight;		// 高さ
	float m_fCounter;		// 波打ちカウンター
	int m_nWave;				// 波の間隔
	int m_nIdxTexture;		// テクスチャインデックス
};
#endif