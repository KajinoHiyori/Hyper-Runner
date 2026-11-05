//==========================================
// 
// ランキング処理[ranking.h]
// Author : Kajino Hiyori _Create2026/07/30
// 
//==========================================
#ifndef _RANKING_H_
#define _RANKING_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "manager.h"

//=============================
// マクロ定義
//=============================
#define MAX_RANKINGWALL	(128)	// 壁の最大数

//=============================
// 前方宣言
//=============================

//=============================
// ランキングクラス
//=============================
class CRanking : public CScene
{
public:
	CRanking(MODE mode);
	~CRanking();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	int m_nCounter;		// 次に遷移するまでの間隔をカウント
};
#endif