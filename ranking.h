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
class CRankingUI;	// ランキングUIクラス
class CPlayer;		// プレイヤークラス
class CField;		// フィールドクラス
class CWall;		// 壁クラス

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
	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CField* GetField(void) { return m_pField; }
	static CWall* GetWall(int nIdx) { return m_apWall[nIdx]; }
	static CRankingUI* GetRankingUI(void) { return m_pRankingUI; }

private:
	static CPlayer* m_pPlayer;		// プレイヤークラスへのポインタ
	static CField* m_pField;		// フィールドへのポインタ
	static CWall* m_apWall[MAX_RANKINGWALL];		// 壁へのポインタ
	static CRankingUI* m_pRankingUI;	// ランキングUIへのポインタ
	int m_nCounter;		// 次に遷移するまでの間隔をカウント
};
#endif