//==========================================
// 
// メッシュリング処理[meshring.h]
// Create : 2026/10/21~
// 
//==========================================
#ifndef _MESHRING_H_
#define	_MESHRING_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// メッシュリングクラス
//=============================
class CMeshRing : public CObject
{
public:
	// リングの種類
	typedef enum
	{
		RINGTYPE_NONE = 0,	// 何もしていない状態
		RINGTYPE_LANDING,	// 着地
		RINGTYPE_MAX
	}RINGTYPE;

	CMeshRing(CObject::PRIORITY priority = CObject::PRIOTITY_ADDALPHA);
	~CMeshRing();
	static CMeshRing* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, RINGTYPE type, float fInside, float fOutside, int nLife);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, RINGTYPE type, float fInside, float fOutside, int nLife);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) {}

private:
	void SetVtx(void);
	void SetIdx(void);
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;	// インデックスバッファへのポインタ
	D3DXMATRIX m_mtxWorld;				// ワールドマトリックス
	D3DXVECTOR3 m_pos;					// メッシュフィールドの位置
	D3DXVECTOR3 m_rot;					// メッシュフィールドの向き
	D3DXCOLOR m_colInside;				// 内側の色
	D3DXCOLOR m_colOutside;				// 外側の色
	RINGTYPE m_type;						// 種類
	float m_fInside;					// 大きさ(内側)
	float m_fOutside;					// 大きさ(外側)
	int m_nLife;							// 寿命
};
#endif
