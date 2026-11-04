//==========================================
// 
// ランキングデータ処理[ranking_offline.h]
// Author : Kajino Hiyori _Create2026/07/30
// 
//==========================================
#ifndef _RANKING_OFFLINE_H_
#define	_RANKING_OFFLINE_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"
#include "protocol_ranking.h"

//=============================
// マクロ定義
//=============================

//=============================
// 前方宣言
//=============================

//=============================
// ランキングデータクラス
//=============================
class CRankingOffline
{
public:
	CRankingOffline(void);
	~CRankingOffline();
	static CRankingOffline* Create(void);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Set(int nScore);
	void Get(int* pData);

private:
	void SortScore(void);
	void SaveFile(void);
	void LoadFile(void);
	int m_aScore[DISP_RANKING];	// スコア格納
	int m_nLatest;	// 最新のスコア
};
#endif