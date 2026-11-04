//==========================================
// 
// ゲーム処理[game.h]
// Author : Kajino Hiyori _Create2026/06/29
// 
//==========================================
#ifndef _GAME_H_
#define _GAME_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "manager.h"
#include "pause.h"

//=============================
// マクロ定義
//=============================
#define MAX_GAMEWALL	(128)	// 壁の最大数

//=============================
// 前方宣言
//=============================
//class CPlayer;		// プレイヤークラス
//class CField;		// フィールドクラス
//class CWall;		// 壁クラス
//class CMachine;		// マシンクラス
//class CTimer;		// タイマークラス
//class CGameUI;		// UIクラス

//=============================
// ゲームクラス
//=============================
class CGame : public CScene
{
public:
	// ゲームの状態
	typedef enum 
	{
		STATE_NONE = 0,	// 初期化
		STATE_START,	// 開始待ち
		STATE_GAMING,	// ゲーム中
		STATE_CLEAR,	// クリア
		STATE_LOSER,	// 失敗
		STATE_END,		// 遷移待機
		STATE_MAX
	}STATE;

	CGame(MODE mode);
	~CGame();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetState(STATE state) { m_state = state; }
	STATE GetState(void) { return m_state; }
	//static CPlayer* GetPlayer(void) { return m_pPlayer; }
	//static CField* GetField(void) { return m_pField; }
	//static CWall* GetWall(int nIdx){ return m_apWall[nIdx]; }
	//static CMachine* GetMachine(void) { return m_pMachine; }
	//static CTimer* GetTimer(void) { return m_pTimer; }
	//static CPause* GetPause(void) { return m_pPause; }
	//static CGameUI* GetGameUI(void) { return m_pGameUI; }
	//static bool GetEnablePause(void) { return m_pPause->GetPauseState(); }

private:
	//static CPlayer* m_pPlayer;		// プレイヤークラスへのポインタ
	//static CField* m_pField;		// フィールドへのポインタ
	//static CWall* m_apWall[MAX_GAMEWALL];		// 壁へのポインタ
	//static CMachine* m_pMachine;	// マシンクラスへのポインタ
	//static CTimer* m_pTimer;		// タイマーへのポインタ
	//static CPause* m_pPause;		// ポーズへのポインタ
	//static CGameUI* m_pGameUI;		// UIへのポインタ
	STATE m_state;					// 状態管理
};
#endif