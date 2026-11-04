//==========================================
// 
// マネージャー処理[manager.h]
// Author : Kajino Hiyori _Create2026/05/12~
// 
//==========================================
#ifndef _MANAGER_H_
#define _MANAGER_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "scene.h"
#include "fade.h"

//=============================
// マクロ定義
//=============================
#define ENABLE_ONLINE	// オンラインランキングを利用
#undef	ENABLE_ONLINE	// ENABLE_ONLINEを無効化

//=============================
// クラスの前方宣言
//=============================
class CRenderer;		// レンダラークラス
class CInputKeyboard;	// キーボード入力クラス
class CInputMouse;		// マウス入力クラス
class CInputJoypad;		// ジョイパッド入力クラス
class CTexture;			// テクスチャクラス
class CFileX;			// Xファイルクラス
class CDebugProc;		// デバッグ表示クラス
class CSound;			// サウンドクラス
class CCamera;			// カメラクラス
class CLight;			// ライトクラス
class CRankingOffline;		// ランキングデータクラス

//=============================
// マネージャークラス定義
//=============================
class CManager
{
public:
	CManager();
	~CManager();
	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static void SetMode(CScene::MODE mode, bool bClear = false);
	static CScene::MODE GetMode(void) { return m_pScene->GetMode(); }
	static CRenderer* GetRenderer(void) { return m_pRenderer; }
	static CInputKeyboard* GetInputKeyboard(void) { return m_pKeyboard; }
	static CInputMouse* GetInputMouse(void) { return m_pMouse; }
	static CInputJoypad* GetInputJoypad(void) { return m_pJoypad; }
	static CTexture* GetTexture(void) { return m_pTexture; }
	static CFileX* GetFileX(void) { return m_pFileX; }
	static CDebugProc* GetDebugProc(void) { return m_pDebugProc; }
	static CSound* GetSound(void) { return m_pSound; }
	static CCamera* GetCamera(void) { return m_pCamera; }
	static CLight* GetLight(void) { return m_pLight; }
	static CScene* GetScene(void) { return m_pScene; }
	static CFade* GetFade(void) { return m_pFade; }
	static CFade::STATE GetFadeState(void) { return m_pFade->GetState(); }
	static CRankingOffline* GetRankingOffline(void) { return m_pRankingOffline; }
	static bool GetPause(void) { return m_bPause; }
	static bool GetEnableObline(void) { return m_bOnline; }

private:
	void Reload(void);
	static CRenderer* m_pRenderer;				// レンダラーへのポインタ
	static CInputKeyboard* m_pKeyboard;		// キーボード入力へのポインタ
	static CInputMouse* m_pMouse;			// マウス入力へのポインタ
	static CInputJoypad* m_pJoypad;		// ジョイパッド入力へのポインタ
	static CTexture* m_pTexture;				// テクスチャへのポインタ
	static CFileX* m_pFileX;					// Xファイルへのポインタ
	static CDebugProc* m_pDebugProc;			// デバッグ表示へのポインタ
	static CSound* m_pSound;	// サウンドへのポインタ
	static CCamera* m_pCamera;	// カメラへのポインタ
	static CLight* m_pLight;	// ライトへのポインタ
	static CScene* m_pScene;	// シーンへのポインタ
	static CFade* m_pFade;		// フェードへのポインタ
	static CRankingOffline* m_pRankingOffline;	// ランキングデータへのポインタ
	static bool m_bPause;		// ポーズ状態の管理
	static bool m_bOnline;		// オンライン状態管理
};

#endif // !_MANAGER_H_

