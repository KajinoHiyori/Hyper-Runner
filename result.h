//==========================================
// 
// リザルト処理[result.h]
// Author : Kajino Hiyori _Create2026/06/29
// 
//==========================================
#ifndef _RESULT_H_
#define _RESULT_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "manager.h"

//=============================
// マクロ定義
//=============================

//=============================
// 前方宣言
//=============================


//=============================
// リザルトクラス
//=============================
class CResult : public CScene
{
public:
	// リザルトの状態
	typedef enum 
	{
		STATE_NONE = 0,	// 初期化
		STATE_STAGING,	// 演出
		STATE_DISP,		// 表示
		STATE_MAX
	}STATE;

	CResult(MODE mode);
	~CResult();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetState(STATE state) { m_state = state; }
	STATE GetState(void) { return m_state; }

private:
	
	STATE m_state;					// 状態管理
};
#endif