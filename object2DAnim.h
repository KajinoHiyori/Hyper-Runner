//==========================================
// 
// 2Dテクスチャアニメーション処理[object2D.h]
// Author : Kajino Hiyori _Create2026/05/08~
// 
//==========================================
#ifndef _OBJECT2DANIM_H_
#define _OBJECT2DANIM_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// 2Dアニメーションオブジェクトクラス
//=============================
class CObject2DAnim : public CObject
{
public:
	static CObject2DAnim* Create(D3DXVECTOR3 pos, D3DXCOLOR col);
	CObject2DAnim(CObject::PRIORITY priority = CObject::PRIORITY_UI);
	~CObject2DAnim();
	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos, D3DXCOLOR col);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetAnimInfo(float fWidth, float fHeight, int nFrame, int nMaxAnim, float fTexW, float fTexH, bool bLoop);
	void BindTexture(LPDIRECT3DTEXTURE9 pTexture) { m_pTexture = pTexture; }
	bool GetEnd(void) { return m_bEnd; }

private:
	void SetPosition(D3DXVECTOR3 pos);
	void SetVtx(void);
	D3DXVECTOR3 m_pos;			// 表示位置
	D3DXVECTOR3 m_rot;			// 向き
	D3DXCOLOR m_col;			// 色
	float m_fWidth;				// 横幅
	float m_fHeight;			// 縦幅
	float m_fLength;			// 対角線の長さ
	float m_fAngle;				// 対角線の長さ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファ	
	LPDIRECT3DTEXTURE9 m_pTexture;			// テクスチャへのポインタ

	// アニメーション管理
	int m_nCounterAnim;	// アニメーションカウンター
	int m_nPatternAnim;	// パターンナンバー
	int m_nFrame;		// パターンナンバーを加算するまでのフレーム数
	int m_nMaxAnim;		// 最大コマ数
	float m_fTexHStart;	// テクスチャ読み込み開始位置[上下段がある場合]
	float m_fTexWidth;	// テクスチャの読み込み幅
	float m_fTexHeight;	// テクスチャの読み込み高さ
	bool m_bLoop;		// ループ管理
	bool m_bEnd;		// アニメーションの終了状態
};

#endif
