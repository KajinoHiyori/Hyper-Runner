//==========================================
// 
// 減速アイテム処理[item_speeddown.h]
// Author : Kajino Hiyori _Create2026/11/09~
// 
//==========================================
#ifndef _ITEM_SPEEDDOWN_H_
#define _ITEM_SPEEDDOWN_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "item.h"
#include "loadscripts.h"

//=============================
// マクロ定義
//=============================
//=============================
// 前方宣言
//=============================
class CPlayer;	// プレイヤークラス

//=============================
// アイテムクラス
//=============================
class CSpeedDown : public CItem
{
public:
	CSpeedDown(CObject::PRIORITY priority = CObject::PRIORITY_EVENTS);
	~CSpeedDown();
	static CSpeedDown* Create(D3DXVECTOR3 pos);
	HRESULT Init(D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { CItem::SetPosition(pos); }
	D3DXVECTOR3 GetPosition(void) { return CItem::GetPosition(); }
	static int GetNumAll(void) { return m_nNumAll; }
	void Release(CPlayer* pPlayer);

private:
	static int m_nNumAll;			// アイテムの総数
	static const char* m_pModelName;	// モデル名
};

#endif#pragma once
