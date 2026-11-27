//==========================================
// 
// 雲梯一括管理処理[monkybarevent.h]
// Author : Kajino Hiyori
// 
//==========================================
#ifndef _MONKYBAR_EVENT_H_
#define _MONKYBAR_EVENT_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// マクロ定義
//=============================
#define MAX_MONKYBAR	(10)	// 雲梯の本数

//=============================
// 前方宣言
//=============================
class CMonkyBar;	// 雲梯[1本]クラス

//=============================
// 雲梯一括管理クラス
//=============================
class CMonkyBarEvent : public CObject
{
public:
	CMonkyBarEvent(CObject::PRIORITY priority = CObject::PRIORITY_EVENTS);
	~CMonkyBarEvent();
	static CMonkyBarEvent* Create(D3DXVECTOR3 pos);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) {}
	void Release(void);

private:
	CMonkyBar* m_apBar[MAX_MONKYBAR];	// 雲梯の本数
	float m_fLeft;	// 左端
	float m_fRight;	// 右端
	float m_fDepth;	// 奥
	float m_fFront;	// 手前
};

#endif
