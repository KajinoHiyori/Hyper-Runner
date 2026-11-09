//==========================================
// 
// アイテム処理[item.h]
// Author : Kajino Hiyori _Create2026/11/09~
// 
//==========================================
#ifndef _ITEM_H_
#define _ITEM_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "objectX.h"
#include "loadscripts.h"

//=============================
// マクロ定義
//=============================
#define CHANGE_SPEED	(2.0f)	// 加速度

//=============================
// 前方宣言
//=============================
class CField;	// フィールドクラス
class CPlayer;	// プレイヤークラス

//=============================
// アイテムクラス
//=============================
class CItem : public CObjectX
{
public:
	// アイテムの種類
	typedef enum
	{
		TYPE_NONE = -1,	// 初期化
		TYPE_SPEEDUP,	// 加速
		TYPE_SPEEDDOWN,	// 減速
		TYPE_MAX
	}TYPE;

	typedef enum
	{
		STATE_NONE = 0,		// 通常状態
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

	CItem(CObject::PRIORITY priority = CObject::PRIORITY_EVENTS);
	~CItem();
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, TYPE type, const char* pFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { CObjectX::SetPosition(pos); }
	D3DXVECTOR3 GetPosition(void) { return CObjectX::GetPosition(); }
	bool GetJump(void) { return m_bJump; }
	void SetState(STATE state) { m_state = state; m_nStateCounter = 0; }
	STATE GetState(void) { return m_state; }
	static CItem* Create(D3DXVECTOR3 pos, int Type);
	static int GetNumAll(void) { return m_nNumAll; }
	static void LoadInfo(void);
	static void LoadSetting(const char* pFileName);
	CPlayer* CollisionPlayer(void);
	void Release(void) { CObjectX::Release();}

private:
	void UpdateGame(void);
	void CollisionField(CField* pField);
	void CollisionBlock(void);
	void Move(void);
	static void Setter(D3DXVECTOR3 pos, int type);
	TYPE m_type;					// 種類
	D3DXVECTOR3	m_posOld;			// モデルの1フレーム前の位置
	D3DXVECTOR3 m_posDest;			// 目的値
	D3DXVECTOR3	m_move;				// モデルの移動量
	STATE m_state;					// 状態
	float m_fSize;					// 値判定サイズ
	int m_nStateCounter;			// ステートカウンター
	bool m_bJump;					// ジャンプフラグ
	static int m_nNumAll;			// アイテムの総数
};

#endif