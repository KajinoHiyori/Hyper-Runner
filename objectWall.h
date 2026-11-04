//========================================================
// 
// Wallポリゴン処理[objectWall.h]
// Author : Kajino Hiyori _Create2026/06/01
//
//========================================================
#ifndef _OBJECTWALL_H_
#define	_OBJECTWALL_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// マクロ定義
//=============================
#define NUM_VTXWALL	(4)	// 頂点数
#define NUM_LAYER	(4)	// マルチテクスチャレイヤー層

//=============================
// 3Dオブジェクトクラス
//=============================
class CObjectWall : public CObject
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

	CObjectWall(CObjectWall::PRIORITY priority = CObject::PRIORITY_CYLINDER);
	~CObjectWall();
	static CObjectWall* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float m_fHeight, TYPE type = CENTER_MIDDLE);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float fHeight, TYPE type = CENTER_MIDDLE);
	void SetTextureIdx(int nIdxTexture);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos;}
	void SetRotation(D3DXVECTOR3 rot) { m_rot = rot;}
	void SetColor(D3DXCOLOR col) { m_col = col; SetVtxColor(); }
	void SetScroll(int nIdx, float fSpeed) { m_aScroll[nIdx] = fSpeed; }
	float GetWidth(void) { return m_fWidth; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXCOLOR GetColor(void) { return m_col; }

private:
	void SetVtx(void);
	void SetVtxColor(void);
	void UpdateTexture(void);
	D3DXVECTOR3 m_pos;	// 表示位置
	D3DXVECTOR3 m_rot;	// 向き
	D3DXCOLOR m_col;	// 色
	float m_fWidth;		// 横幅
	float m_fHeight;		// 高さ
	float m_aScroll[NUM_LAYER];				// スクロール速度
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファ
	int m_nNumLayer;		// レイヤー数
	int m_aIdxTexture[NUM_LAYER];		// テクスチャのインデックスを格納
	D3DXVECTOR2 m_aTecU[NUM_LAYER][NUM_VTXWALL];		// テクスチャのインデックスを格納
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	TYPE m_type;		// アンカーポイントのタイプ
};
#endif