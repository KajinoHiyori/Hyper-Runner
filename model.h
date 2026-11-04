//========================================================
// 
// モデル処理[model.h]
// Author : Kajino Hiyori _Create2026/06/02
//
//========================================================
#ifndef _MODEL_H_
#define	_MODEL_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// マクロ定義
//=============================
#define MAX_MAT	(16)	// マテリアルの最大数
#define MAX_MODEL	(16)	// モデルの最大数

//=============================
// モデルクラス
//=============================
class CModel
{
public:
	// モデルの色状態
	typedef enum
	{
		COLORTYPE_DEFAULT = 0,	// デフォルトの色を使用
		COLORTYPE_CHANGE,		// 色が変更されている
		COLORTYPE_MAX
	}COLORTYPE;

	CModel();
	~CModel();
	static CModel* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, const char* pFileName);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, const char* pFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Draw(CObject::TYPE type, D3DXCOLOR col);
	void SetParent(CModel* pModel) { m_pParent = pModel; }
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotation(D3DXVECTOR3 rot) { m_rot = rot; }
	D3DXVECTOR3 GetPosOffset(void) { return m_posOffset; }
	D3DXVECTOR3 GetRotOffset(void) { return m_rotOffset; }
	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; }
	void SetColor(COLORTYPE colType, D3DXCOLOR col = COLOR_WHITE) { m_colType = colType; m_col = col; }

private:
	D3DXMATRIX	m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3	m_pos;			// モデルの位置
	D3DXVECTOR3	m_posOffset;	// モデルの位置[オフセット]
	D3DXVECTOR3 m_rot;			// モデルの向き
	D3DXVECTOR3 m_rotOffset;	// モデルの向き[オフセット]
	D3DXVECTOR3	m_vtxMin;	// モデルの大きさ(最小)
	D3DXVECTOR3	m_vtxMax;	// モデルの大きさ(最大)
	CModel* m_pParent;		// 親モデルへのポインタ
	LPD3DXMESH			m_pMesh;		// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER		m_pBuffMat;		// マテリアルへのポインタ
	DWORD				m_dwNumMat;		// マテリアルの数
	int m_aIdxTexture[MAX_MAT];	// テクスチャのインデックスを格納
	COLORTYPE m_colType;	// モデルの色状態
	D3DXCOLOR m_col;		// モデルの色
};

#endif