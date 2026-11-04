//========================================================
// 
// ビルボード処理[objectBillboardAnim.h]
// Author : Kajino Hiyori _Create2026/08/24
//
//========================================================
#ifndef _BILLBOARDANIM_H_
#define	_BILLBOARDANIM_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// ビルボードクラス
//=============================
class CBillboardAnim : public CObject
{
public:
	CBillboardAnim(CObject::PRIORITY priority = CObject::PRIORITY_EVENTS);
	~CBillboardAnim();
	static CBillboardAnim* Create(D3DXVECTOR3 pos, float fWidth, float fHeight, bool bDisp = true);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, float fWidth, float fHeight, bool bDisp = true);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetTextureIdx(int nIdx);
	void SetAnimInfo(float fWidth, float fHeight, int nFrame, int nMaxAnim, float fTexW, float fTexH, bool bLoop);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; }
	D3DXCOLOR GetColor(void) { return m_col; }
	void SetColor(D3DXCOLOR col) { m_col = col; SetVtx(); }
	void Release(void) { CObject::Release(); }
	void SetDisp(bool bDisp) { m_bDisp = bDisp; }
	static int GetNumAll(void) { return m_nNumAll; }

private:
	void SetVtx(void);
	static int m_nNumAll;		// 総数
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 m_pos;			// ビルボードの位置
	D3DXCOLOR m_col;			// ビルボードの色
	int m_nIdxTexture;			// テクスチャのインデックス
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	float m_fWidth;	// 幅
	float m_fHeight;	// 高さ
	bool m_bDisp;		// 表示管理

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