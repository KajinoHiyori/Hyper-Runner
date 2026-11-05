//==========================================
// 
// タイトル処理[title.h]
// Author : Kajino Hiyori _Create2026/07/30
// 
//==========================================
#ifndef _TITLE_H_
#define _TITLE_H_

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
// タイトルクラス
//=============================
class CTitle : public CScene
{
public:
	CTitle(MODE mode);
	~CTitle();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	int m_nCounter;		// 次に遷移するまでの間隔をカウント
};
#endif