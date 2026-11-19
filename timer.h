//==========================================
// 
// タイマー処理[timer.h]
// Author : Kajino Hiyori _Create2026/05/18~
// 
//==========================================
#ifndef _TIMER_H_
#define _TIMER_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// マクロ定義
//=============================
#define TIMER_DIGIT	(3)	// タイマーの桁数

//=============================
// 前方宣言
//=============================
class CNumber2D;	// 数字[2D]クラス

//=============================
// タイマークラス
//=============================
class CTimer : public CObject
{
public:
	CTimer(CObject::PRIORITY priority = CObject::PRIORITY_UI);
	~CTimer();
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(D3DXVECTOR3 pos, int nTimer);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos = INIT_D3DVEC3) {}
	bool GetOperate(void) { return m_bOperate; }
	void SetOperate(bool bOperate) { m_bOperate = bOperate; }
	static CTimer* Create(D3DXVECTOR3 pos, int nTimer);

private:
	void SetTexU(void);
	void UpdateTutorial(void);
	void UpdateGame(void);
	CNumber2D* m_apNumber[TIMER_DIGIT];	// 数字表示へのポインタ
	int m_nFrame;	// フレーム数をカウント
	int m_nTimer;	// タイマーの値を管理
	int m_aTecU[TIMER_DIGIT];	// タイマーのUV座標を設定
	bool m_bOperate;	// 稼働状態を管理[true : 稼働中 / false : 停止中]
};

#endif // !_PLAYER_H_

#pragma once
