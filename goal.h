//========================================================
// 
// Goal処理[goal.h]
// Author : Kajino Hiyori
//
//========================================================
#ifndef _GOAL_H_
#define	_GOAL_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// マクロ定義
//=============================
//=============================
// 前方宣言
//=============================

//=============================
// ゴールクラス
//=============================
class CGoal : public CObject
{
public:
	// ステート管理
	typedef enum
	{
		STATE_NONE = 0,	// ゴールしていない
		STATE_GOAL,		// ゴール
		STATE_MAX
	}STATE;

	CGoal(CObject::PRIORITY priority = CObject::PRIORITY_EVENTS);
	~CGoal();
	static CGoal* Create(float fLine);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(float fLine);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) {}
	float GetLine(void) { return m_fLine; }
	void SetLine(float fLine) { m_fLine = fLine; }

private:
	float m_fLine;	// ゴールライン
};

#endif