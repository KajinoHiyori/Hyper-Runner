//==========================================
// 
// アクション処理[action.h]
// Author : Kajino Hiyori _Create2026/11/13~
// 
//==========================================
#ifndef _ACTION_H_
#define _ACTION_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "objectX.h"
#include "loadscripts.h"

//=============================
// マクロ定義
//=============================

//=============================
// 前方宣言
//=============================
class CField;	// フィールドクラス
class CPlayer;	// プレイヤークラス

//=============================
// アクションクラス
//=============================
class CAction : public CObjectX
{
public:
	// アクションの種類
	typedef enum
	{
		TYPE_NONE = -1,	// 初期化
		TYPE_BOX,		// 箱
		TYPE_MAX
	}TYPE;

	// アクションステート
	typedef enum
	{
		STATE_NONE = 0,		// 通常状態
		STATE_PLAYER,		// プレイヤーが保持
		STATE_ACT,			// 任意の行動中
		STATE_MAX
	}STATE;

	CAction(CObject::PRIORITY priority = CObject::PRIORITY_EVENTS);
	~CAction();
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, TYPE type, const char* pFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { CObjectX::SetPosition(pos); }
	D3DXVECTOR3 GetPosition(void) { return CObjectX::GetPosition(); }
	void SetMove(D3DXVECTOR3 move) { m_move.x = move.x; m_move.z = move.z; }
	bool GetJump(void) { return m_bJump; }
	void SetState(STATE state) { m_state = state; m_nStateCounter = 0; }
	STATE GetState(void) { return m_state; }
	STATE GetStateOld(void) { return m_stateOld; }
	static CAction* Create(D3DXVECTOR3 pos, int Type);
	static int GetNumAll(void) { return m_nNumAll; }
	static void LoadInfo(void);
	static void LoadSetting(const char* pFileName);
	void CollisionPlayer(CPlayer* pPlayer);
	void Release(void) { CObjectX::Release();}
	bool CollisionHitter(void);
	static void Setter(D3DXVECTOR3 pos, int type);

private:
	void UpdateGame(void);
	void StateManager(CPlayer* pPlayer);
	void UpdateStateNone(CPlayer* pPlayer);
	void UpdateStatePlayer(CPlayer* pPlayer);
	void CollisionField(CField* pField);
	void CollisionBlock(void);
	void Move(void);
	TYPE m_type;					// 種類
	D3DXVECTOR3	m_posOld;			// モデルの1フレーム前の位置
	D3DXVECTOR3 m_posDest;			// 目的値
	D3DXVECTOR3 m_posOffset;		// オフセット
	D3DXVECTOR3	m_move;				// モデルの移動量
	STATE m_state;					// 状態
	STATE m_stateOld;				// 状態
	float m_fSize;					// 値判定サイズ
	int m_nStateCounter;			// ステートカウンター
	bool m_bJump;					// ジャンプフラグ
	static int m_nNumAll;			// アクションの総数
};

#endif