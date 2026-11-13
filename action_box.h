//==========================================
// 
// 箱アクション処理[action_box.h]
// Author : Kajino Hiyori _Create2026/11/13~
// 
//==========================================
#ifndef _ACTION_BOX_H_
#define _ACTION_BOX_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "action.h"
#include "loadscripts.h"

//=============================
// マクロ定義
//=============================
//=============================
// 前方宣言
//=============================
class CPlayer;	// プレイヤークラス

//=============================
// アクションクラス
//=============================
class CBox : public CAction
{
public:
	CBox(CObject::PRIORITY priority = CObject::PRIORITY_EVENTS);
	~CBox();
	static CBox* Create(D3DXVECTOR3 pos);
	HRESULT Init(D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { CAction::SetPosition(pos); }
	D3DXVECTOR3 GetPosition(void) { return CAction::GetPosition(); }
	static int GetNumAll(void) { return m_nNumAll; }

private:
	void UpdateAction(void);
	static int m_nNumAll;			// アクションの総数
	static const char* m_pModelName;	// モデル名
};

#endif#pragma once
