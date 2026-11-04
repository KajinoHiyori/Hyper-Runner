//==========================================
// 
// ポーズ処理[pause.h]
// Author : Kajino Hiyori _Create2026/07/16~
// 
//==========================================
#ifndef _PAUSE_H_
#define _PAUSE_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// 前方宣言
//=============================
class CPolygon2D;	// ポリゴン2Dクラス

//=============================
// ポーズクラス
//=============================
class CPause : public CObject
{
public:
	typedef enum
	{
		MENU_CONTINUE = 0,	// continue
		MENU_RETRY,			// retry
		MENU_QUIT,			// quit
		MENU_MAX
	}MENU;

	typedef enum
	{
		TYPE_BG = 0,	// 背景
		TYPE_GEAR1,		// 歯車1
		TYPE_GEAR2,		// 歯車2
		TYPE_GEAR3,		// 歯車3
		TYPE_GEAR4,		// 歯車4
		TYPE_PAUSE,		// pause
		TYPE_CONTINUE,	// continue
		TYPE_RETRY,		// retry
		TYPE_QUIT,		// quit
		TYPE_MAX
	}TYPE;

	CPause(CObject::PRIORITY priority = PRIORITY_PAUSE);
	~CPause();
	static CPause* Create(void);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool GetPauseState(void) { return m_bPause; }
	void SetPauseState(bool bPause) { m_bPause = bPause; }
	void SetPosition(D3DXVECTOR3 pos) {}

private:
	void MouseSelect(void);
	void StickSelect(void);
	void NormalSelect(void);
	void DecisionMenu(void); 
	D3DXVECTOR3 m_mousePos;	// マウスの現在座標
	D3DXVECTOR3 m_mousePosOld;	// マウスの過去座標
	MENU m_menu;	// 現在選択されているメニュー
	bool m_bPause;	// ポーズ状態[true : ポーズ中 / false : ポーズ中でない]
	bool m_bDisp;	// 表示状態
	CPolygon2D* m_apPolygon[CPause::TYPE_MAX];	// ポリゴン情報[object継承無]
	static const char* m_apFilename[CPause::TYPE_MAX];	// テクスチャファイル名
};

#endif // !_PAUSE_H_