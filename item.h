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
#define MAX_ITEM	(16)	// アイテムの種類の最大数

//=============================
// 前方宣言
//=============================
class CField;		// フィールドクラス
class CWall;	// 壁クラス

//=============================
// アイテムクラス
//=============================
class CItem : public CObjectX
{
public:
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
	HRESULT Init(D3DXVECTOR3 pos, int Type);
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
	void Release(void) { CObjectX::Release();}

private:
	void UpdateGame(void);
	void CollisionField(CField* pField);
	void CollisionBlock(void);
	void Move(void);
	D3DXVECTOR3	m_posOld;			// モデルの1フレーム前の位置
	D3DXVECTOR3 m_posDest;			// 目的値
	D3DXVECTOR3	m_move;				// モデルの移動量
	STATE m_state;					// 状態
	float m_fSize;					// 値判定サイズ
	int m_nStateCounter;			// ステートカウンター
	bool m_bJump;					// ジャンプフラグ
	static int m_nNumAll;			// アイテムの総数
	static char m_apModelName[MAX_ITEM][MAX_STRING];	// モデル名
};

#endif