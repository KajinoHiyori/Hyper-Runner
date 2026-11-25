//==========================================
// 
// 雲梯処理[monkybar.h]
// Author : Kajino Hiyori _Create2026/11/25
// 
//==========================================
#ifndef _MONKY_BAR_H_
#define _MONKY_BAR_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "objectX.h"
#include "loadscripts.h"
#include "shadow.h"

//=============================
// マクロ定義
//=============================

//=============================
// 前方宣言
//=============================
class CPlayer;	// プレイヤークラス

//=============================
// 雲梯クラス
//=============================
class CMonkyBar : public CObjectX
{
public:
	// 状態管理
	typedef enum
	{
		STATE_NONE = 0,		// 通常状態
		STATE_PLAYER_LEFT,	// 左手がくっついている
		STATE_PLAYER_RIGHT,	// 右手がくっついている
		STATE_MAX
	}STATE;

	// 色の変更[出現時のみ適応]
	typedef enum
	{
		COLORCHANGER_NONE = -1,
		ADDBLUE,	// B++
		DISRED,		// R--
		ADDGREEN,	// G++
		DISBLUE,	// B--
		ADDRED,		// R++
		DISGREEN,	// G--
		COLORCHANGER_MAX
	}COLORCHANGER;

	CMonkyBar(CObject::PRIORITY priority = CObject::PRIORITY_EVENTS);
	~CMonkyBar();
	static CMonkyBar* Create(D3DXVECTOR3 pos);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { CObjectX::SetPosition(pos); }
	D3DXVECTOR3 GetPosition(void) { return CObjectX::GetPosition(); }
	bool GetJump(void) { return m_bJump; }
	void SetState(STATE state) { m_state = state; m_nStateCounter = 0; }
	STATE GetState(void) { return m_state; }
	static int GetNumAll(void) { return m_nNumAll; }
	static void LoadInfo(void);
	static void LoadSetting(const char* pFileName);
	bool CollisionPlayer(D3DXVECTOR3 pos);
	void Release(void) { CObjectX::Release(); if (m_pShadow != NULL) { m_pShadow->Release(); } }

private:
	void UpdateGame(void);
	CShadow* m_pShadow;			// 影情報
	TYPE m_type;					// 種類
	D3DXVECTOR3	m_posOld;			// モデルの1フレーム前の位置
	D3DXVECTOR3 m_posDest;			// 目的値
	D3DXVECTOR3	m_move;				// モデルの移動量
	STATE m_state;					// 状態
	float m_fSize;					// 値判定サイズ
	int m_nStateCounter;			// ステートカウンター
	bool m_bJump;					// ジャンプフラグ
	static int m_nNumAll;			// 雲梯の総数
	static const char* m_pFileName;	// ファイル名
};

#endif