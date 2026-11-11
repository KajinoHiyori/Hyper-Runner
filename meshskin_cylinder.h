//========================================================
// 
// メッシュスキン[円柱]処理[meshskin_cylinder.h]
// Author : Kajino Hiyori _Create2026/11/11
//
//========================================================
#ifndef _MESHSKIN_CYLINDER_H_
#define	_MESHSKIN_CYLINDER_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// マクロ定義
//=============================
#define MAX_CYLINDERBONE	(2)	// ボーン最大数
#define MAX_CYLINDERVERTEX		(512)	// 頂点の最大数

//=============================
// メッシュスキン[円柱]クラス
//=============================
class CMeshSkinCylinder : public CObject
{
public:
	// ボーン情報
	typedef struct
	{
		D3DXVECTOR3 pos;	// ボーンの位置
		D3DXVECTOR3 rot;	// ボーンの向き
	}BONE;

	CMeshSkinCylinder(CObject::PRIORITY priority = CObject::PRIORITY_MESH);
	~CMeshSkinCylinder();
	static CMeshSkinCylinder* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fRadius, float fHeight);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fRadius, float fHeight);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; }

private:
	void SetOffset(void);
	void SetVtx(void);
	void SetDrawVtxBuff(void);
	void SetIdx(void);
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;	// インデックスバッファへのポインタ
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	D3DXVECTOR3 m_pos;		// 位置
	D3DXVECTOR3 m_rot;		// 向き
	D3DXCOLOR m_col;		// 色
	float m_fRadius;		// 半径
	float m_fHeight;		// 高さ
	int m_nIdxTexture;		// テクスチャインデックス
	int m_nDivisionW;		// 横分割数
	int m_nDivisionH;		// 縦分割数
	int m_nWidthSize;		// 横幅の大きさ
	int m_nHeightSize;		// 高さの大きさ
	int m_nNumPrimitive;	// プリミティブ数
	int m_nNumIdx;			// インデックス数
	int m_nNumVertex;		// 頂点数
	int m_nNumBone;			// ボーン数
	BONE m_aBone[MAX_CYLINDERBONE];	// ボーン情報を格納
	D3DXVECTOR3 m_aPosVtx[MAX_CYLINDERVERTEX];	// 頂点座標
	D3DXVECTOR3 m_aPosOffset[MAX_CYLINDERBONE][MAX_CYLINDERVERTEX];	// 頂点座標
	float m_aWeight[MAX_CYLINDERBONE][MAX_CYLINDERVERTEX];	// ボーンごとの影響力
};
#endif