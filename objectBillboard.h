//========================================================
// 
// ビルボード処理[objectBillboard.h]
// Author : Kajino Hiyori _Create2026/06/01
//
//========================================================
#ifndef _BILLBOARD_H_
#define	_BILLBOARD_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// ビルボードクラス
//=============================
class CBillboard : public CObject
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

	CBillboard(CObject::PRIORITY priority = CObject::PRIORITY_EVENTS);
	~CBillboard();
	static CBillboard* Create(D3DXVECTOR3 pos, float fWidth, float fHeight, TYPE type = CENTER_MIDDLE);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, float fWidth, float fHeight, TYPE type = CENTER_MIDDLE);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetTextureIdx(int nIdx);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; }
	D3DXCOLOR GetColor(void) { return m_col; }
	void SetColor(D3DXCOLOR col) { m_col = col; SetVtxBuff(); }
	static int GetNumAll(void) { return m_nNumAll; }
	void Release(void) { CObject::Release(); }
	void SetWidth(float fWidth) { m_fWidth = fWidth; SetVtxBuff(); }
	void SetHeight(float fHeight) { m_fHeight = fHeight; SetVtxBuff(); }

private:
	void SetVtxBuff(void);
	static int m_nNumAll;		// 総数
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 m_pos;			// ビルボードの位置
	D3DXCOLOR m_col;			// ビルボードの色
	int m_nIdxTexture;			// テクスチャのインデックス
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	float m_fWidth;	// 幅
	float m_fHeight;	// 高さ
	TYPE m_type;	// アンカーポイント位置
};
#endif