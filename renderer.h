//==========================================
// 
// レンダラー処理[renderer.h]
// Author : Kajino Hiyori _Create2026/05/08~
// 
//==========================================
#ifndef _RENDERER_H_
#define _RENDERER_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"

//=============================
// マクロ定義
//=============================
#define ENABLE_MULTITARGETRENDERING	// マルチターゲットレンダリング処理
//#undef ENABLE_MULTITARGETRENDERING	// ENABLE_MULTITARGETRENDERINGを無効化
#define NUM_FEEDBACK	(2)	// フィードバック数

//=============================
// レンダラークラス
//=============================
class CRenderer
{
public:
	CRenderer();
	~CRenderer();
	HRESULT Init(const HWND hWnd, const BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	LPDIRECT3DDEVICE9 GetDevice(void) { return m_pD3DDevice; }
#ifdef ENABLE_MULTITARGETRENDERING
	void ChangeTarget(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 vecU);
	//LPDIRECT3DTEXTURE9 GetTextureMT(void) { return m_pTextureMT; }
	void SetClear(bool bClear) { m_bClearMT = bClear; }
#endif // ENABLE_MULTITARGETRENDERING


private:
	LPDIRECT3D9	 m_pD3D;			// Direct3Dオブジェクトへのポインタ
	LPDIRECT3DDEVICE9 m_pD3DDevice;	// Direct3Dデバイスへのポインタ
#ifdef ENABLE_MULTITARGETRENDERING
	void DrawPastImages(void);
	void DrawCurrentImages(void);
	void ClearTextures(void);
	void SwapTexture(void);
	LPDIRECT3DTEXTURE9	m_apTextureMT[NUM_FEEDBACK];	// レンダリングターゲット用テクスチャ
	LPDIRECT3DSURFACE9	m_apRenderMT[NUM_FEEDBACK];	// テクスチャレンダリング用インターフェース
	LPDIRECT3DSURFACE9	m_pZBuffMT;		// テクスチャレンダリング用Zバッファ
	D3DVIEWPORT9		m_viewportMT;	// テクスチャレンダリング用ビューポート
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffMT;	// フィードバック用ポリゴンの頂点バッファ
	bool m_bClearMT;	// 画面クリア管理
	bool m_bClearTexture;	// フィードバックテクスチャのクリア
	bool m_bFeedBack;	// フィードバックの有無を管理
	bool m_bFeedBackDest;	// フィードバックの有無を管理
#endif
};

#endif