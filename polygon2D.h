//==========================================
// 
// ポリゴン処理[polygon2D.h]
// Author : Kajino Hiyori _Create2026/05/19~
// 
//==========================================
#ifndef _POLYGON2D_H_
#define _POLYGON2D_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"

//=============================
// ポリゴンクラス
//=============================
class CPolygon2D
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
		ROTATE,			// 回転
		UPRIGHT,		// 上にすぼむ
		TYPE_MAX
	}TYPE;

	static CPolygon2D* Create(D3DXVECTOR3 pos, float fWidth, float fHeight, D3DXCOLOR col, TYPE type = CENTER_MIDDLE, float fWidthButton = 0.0f);
	CPolygon2D();
	~CPolygon2D();
	void SetTextureIdx(int nIdx) { m_nIdxTexture = nIdx; }
	HRESULT Init(D3DXVECTOR3 pos, float fWidth, float fHeight, D3DXCOLOR col, TYPE type, float fWidthButton);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; SetVtx(); }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	void SetRotation(D3DXVECTOR3 rot) { m_rot = rot; SetVtx(); }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	void SetColor(D3DXCOLOR col) { m_col = col; SetVtx(); }
	D3DXCOLOR GetColor(void) { return m_col; }
	void SetHeight(float fHeight) { m_fHeight = fHeight; SetVtx(); }
	float GetHeight(void) { return m_fHeight; }
	void SetWidth(float fWidth) { m_fWidth = fWidth;  SetVtx(); }
	float GetWidth(void) { return m_fWidth; }
	void SetTexMinWidth(float fWidth) { m_fTexMinX = fWidth; SetVtx();}
	void SetTexMinHeight(float fHeight) { m_fTexMinY = fHeight; SetVtx();}
	void SetTexMaxWidth(float fWidth) { m_fTexMaxX = fWidth; SetVtx();}
	void SetTexMaxHeight(float fHeight) { m_fTexMaxY = fHeight; SetVtx();}
	void SetWidthTop(float fWidthTop) { m_fWidthTop = fWidthTop; SetVtx(); }
	float GetWidthTop(void) { return m_fWidthTop; }
	void SetWidthButton(float fWidthButton) { m_fWidthButton = fWidthButton; SetVtx(); }
	float GetWidthButton(void) { return m_fWidthButton; }
	float GetTexMinWidth(void) { return m_fTexMinX; }
	float GetTexMaxWidth(void) { return m_fTexMaxX; }
	static int GetNumAll(void) { return m_nNumAll; }

private:
	void SetVtx(void);
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファ
	int m_nIdxTexture;	// テクスチャインデックス
	D3DXVECTOR3 m_pos;	// 表示位置
	D3DXVECTOR3 m_rot;	// 回転
	D3DXCOLOR m_col;	// 表示色
	float m_fWidth;		// 横幅
	float m_fHeight;	// 縦幅
	float m_fAngle;		// 対角線の角度
	float m_fLength;	// 対角線の長さ
	TYPE m_type;		// 中心座標設定
	float m_fTexMinX;		// テクスチャ読み込み幅[最大値]
	float m_fTexMinY;		// テクスチャ読み込み高さ[最小値]
	float m_fTexMaxX;		// テクスチャ読み込み幅[最大値]
	float m_fTexMaxY;		// テクスチャ読み込み高さ[最小値]
	float m_fWidthTop;		// 横幅[上端]
	float m_fWidthButton;	// 横幅[下端]
	static int m_nNumAll;	// 数字の総数
};

#endif

