//========================================================
// 
// 3Dポリゴン処理[polygon3D.h]
// Author : Kajino Hiyori _Create2026/06/01
//
//========================================================
#ifndef _POLYGON3D_H_
#define	_POLYGON3D_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"

//=============================
// マクロ定義
//=============================
#define NUM_VTX3D	(4)	// 頂点数

//=============================
// 3Dポリゴンクラス
//=============================
class CPolygon3D
{
public:
	// アンカーポイントの位置設定
	typedef enum
	{
		LEFT_TOP = 0,	// 左揃え上寄せ
		LEFT_MIDDLE,	// 左揃え中央寄せ
		LEFT_BUTTON,	// 左揃え下寄せ
		CENTER_TOP,		// 中央揃え上寄せ
		CENTER_MIDDLE,	// 中央揃え中央寄せ
		CENTER_BUTTON,	// 中央揃え下寄せ
		RIGHT_TOP,		// 右揃え上寄せ
		RIGHT_MIDDLE,	// 右揃え中央寄せ
		RIGHT_BUTTON,	// 右揃え下寄せ
	}TYPE;

	CPolygon3D();
	~CPolygon3D();
	static CPolygon3D* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float fHeight, TYPE type = CENTER_MIDDLE);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float fHeight, TYPE type = CENTER_MIDDLE);
	void SetTextureIdx(int nIdx) { m_nIdxTexture = nIdx; }
	void Uninit(void);
	void Update(void);
	void Draw(D3DXMATRIX mtxParent);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; SetVtx(); }
	void SetRotation(D3DXVECTOR3 rot) { m_rot = rot; SetVtx(); }
	void SetWidth(float fWidth) { m_fWidth = fWidth; SetVtx(); }
	void SetHeight(float fHeight) { m_fHeight = fHeight; SetVtx(); }
	float GetHeight(void) { return m_fHeight; }
	void SetTexMinX(float fTexMinX) { m_fTexMinX = fTexMinX; SetVtx();}
	void SetTexMinY(float fTexMinY) { m_fTexMinY = fTexMinY; SetVtx();}
	void SetTexMaxX(float fTexMaxX) { m_fTexMaxX = fTexMaxX; SetVtx();}
	void SetTexMaxY(float fTexMaxY) { m_fTexMaxY = fTexMaxY; SetVtx();}
	void SetColor(D3DXCOLOR col) { m_col = col; SetVtxColor(); }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXCOLOR GetColor(void) { return m_col; }

private:
	void SetVtx(void);
	void SetVtxColor(void);
	D3DXVECTOR3 m_pos;	// 表示位置
	D3DXVECTOR3 m_rot;	// 向き
	D3DXCOLOR m_col;	// 色
	float m_fWidth;		// 横幅
	float m_fHeight;	// 縦幅
	float m_fTexMinX;		// テクスチャ読み込み幅[最大値]
	float m_fTexMinY;		// テクスチャ読み込み高さ[最小値]
	float m_fTexMaxX;		// テクスチャ読み込み幅[最大値]
	float m_fTexMaxY;		// テクスチャ読み込み高さ[最小値]
	TYPE m_type;		// アンカーポイントのタイプ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファ
	int m_nIdxTexture;		// テクスチャのインデックスを格納
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
};
#endif