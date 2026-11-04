//========================================================
// 
// オブジェクトX処理[objectX.h]
// Author : Kajino Hiyori _Create2026/06/01
//
//========================================================
#ifndef _OBJECTX_H_
#define	_OBJECTX_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// マクロ定義
//=============================
#define ENABLE_FILEX	// ファイルの一括読み込みを使用
//#undef	ENABLE_FILEX	// ENABLE_FILEXを無効化
#define MAX_MAT	(16)	// マテリアルの最大数

//=============================
// オブジェクトXクラス
//=============================
class CObjectX : public CObject
{
public:
	// モデルの色状態
	typedef enum
	{
		COLORTYPE_DEFAULT = 0,	// デフォルトの色を使用
		COLORTYPE_CHANGE,		// 色が変更されている
		COLORTYPE_ALPHA,		// アルファ値のみ変更
		COLORTYPE_MAX
	}COLORTYPE;

	CObjectX(CObject::PRIORITY priority = CObject::PRIORITY_EVENTS);
	~CObjectX();
	static CObjectX* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, const char* pFileName, bool bScale = false, D3DXVECTOR3 scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, const char* pFileName, bool bScale = false, D3DXVECTOR3 scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax, D3DXVECTOR3 scl, bool bScale, LPD3DXBUFFER pBuffMat, DWORD dwNumMat, LPD3DXMESH pMesh, int* pIdxTexture, const char* pFileName);
	void Uninit(void);
	void Uninit(bool bEnable);
	void Update(void);
	void Draw(void);
	void Draw(D3DXMATRIX mtx);
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotation(D3DXVECTOR3 rot) { m_rot = rot; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetVtxMin(void) { return m_vtxMin; }
	D3DXVECTOR3 GetVtxMax(void) { return m_vtxMax; }
	void SetScale(D3DXVECTOR3 scl) { m_scl = scl; m_bScale = true; }
	D3DXVECTOR3 GetScale(void) { return m_scl; }
	bool GetEnableScale(void) { return m_bScale; }
#ifdef ENABLE_FILEX
	int m_nIdx;	// Xファイルインデックス
#else
	D3DXMATERIAL* GetBufferPointer(void) { return (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer(); }
	int GetNumMat(void) { return (int)m_dwNumMat; }
	DWORD GetDWNumMat(void) { return m_dwNumMat; }
	LPD3DXMESH GetMesh(void) { return m_pMesh; }
	LPD3DXMESH* GetMeshPointer(void) { return &m_pMesh; }
	LPD3DXBUFFER GetBuffMat(void) { return m_pBuffMat; }
	LPD3DXBUFFER* GetBuffMatPointer(void) { return &m_pBuffMat; }
	int GetIdxTexture(int nIdx) { return m_aIdxTexture[nIdx]; }
#endif
	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; }
	static int GetNumAll(void) { return m_nNumAll; }
	void SetColor(COLORTYPE colType, D3DXCOLOR col = COLOR_WHITE) { m_colType = colType; m_col = col; }
	D3DXCOLOR GetColor(void) { return m_col; }

private:
	static int m_nNumAll;	// 総数
	COLORTYPE m_colType;	// モデルの色状態
	D3DXMATRIX	m_mtxWorld;	// ワールドマトリックス
	D3DXVECTOR3	m_pos;		// モデルの位置
	D3DXVECTOR3	m_rot;		// モデルの向き
	D3DXCOLOR m_col;		// モデルの色
	D3DXVECTOR3	m_vtxMin;	// モデルの大きさ(最小)
	D3DXVECTOR3	m_vtxMax;	// モデルの大きさ(最大)
	bool m_bScale;	// スケール変更を行うかを管理
	D3DXVECTOR3 m_scl;	// 拡大率
#ifdef ENABLE_FILEX

#else
	LPD3DXMESH			m_pMesh;				// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER		m_pBuffMat;				// マテリアルへのポインタ
	DWORD				m_dwNumMat;				// マテリアルの数
	int m_aIdxTexture[MAX_MAT];	// テクスチャのインデックスを格納
#endif // ENABLE_FILEX

};

#endif