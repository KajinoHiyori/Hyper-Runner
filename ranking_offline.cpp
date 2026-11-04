//==========================================
// 
// ランキングデータ処理[ranking_offline.h]
// Author : Kajino Hiyori _Create2026/07/30
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "ranking_offline.h"

//=============================
// マクロ定義
//=============================
#define RESULT_FILE		("data\\SCRIPTS\\ranking.bin")	// ランキングデータの出力先

//=============================
// コンストラクタ
//=============================
CRankingOffline::CRankingOffline()
{
	// 値のクリア
	memset(&m_aScore[0], 0, sizeof m_aScore);
	m_nLatest = 0;
}

//=============================
// デストラクタ
//=============================
CRankingOffline::~CRankingOffline()
{

}

//=============================
// ランキングデータ生成処理
//=============================
CRankingOffline* CRankingOffline::Create(void)
{
	CRankingOffline* pRankingOffline = NULL;
	if (pRankingOffline == NULL)
	{ // クリアに成功している場合
		pRankingOffline = new CRankingOffline;
		if (pRankingOffline != NULL)
		{ // 生成に成功
			pRankingOffline->Init();
			return pRankingOffline;
		}
	}

	return NULL;	// 生成に失敗
}

//=============================
// 初期化処理
//=============================
HRESULT CRankingOffline::Init()
{
	// 外部ファイル情報読み取り
	LoadFile();

	// ランキングを並び替える
	SortScore();

	// 外部ファイル書き出し
	SaveFile();

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CRankingOffline::Uninit(void)
{
	
}

//=============================
// 更新処理
//=============================
void CRankingOffline::Update(void)
{

}

//=============================
// 描画処理
//=============================
void CRankingOffline::Draw(void)
{
	
}

//=============================
// ランキング設定
//=============================
void CRankingOffline::Set(int nData)
{
	// 外部ファイル読み込み
	LoadFile();
	m_nLatest = nData;
	m_aScore[MAX_RANKING] = m_nLatest;

	// ソート
	SortScore();

	// 書き出し
	SaveFile();
}

//=============================
// ランキング取得
//=============================
void CRankingOffline::Get(int* pData)
{
	for (int nCntRanking = 0; nCntRanking < DISP_RANKING; nCntRanking++, pData++)
	{ // データ格納
		*pData = m_aScore[nCntRanking];
	}
}

//=============================
// データ書き出し
//=============================
void CRankingOffline::SaveFile(void)
{
	FILE* pFile;
	// ファイルを開く
	pFile = fopen(RESULT_FILE, "wb");

	if (pFile != NULL)
	{ // ファイルが開けた場合
		// 外部ファイルに情報を書き出す
		fwrite(&m_aScore[0], sizeof(int), MAX_RANKING, pFile);
		fclose(pFile);
	}
}

//=============================
// 外部ファイル読み込み
//=============================
void CRankingOffline::LoadFile(void)
{
	FILE* pFile;
	// ファイルを開く
	pFile = fopen(RESULT_FILE, "rb");
	if (pFile != NULL)
	{ // ファイルが開けた場合
		// 外部ファイルから読み込む
		fread(&m_aScore[0], sizeof(int), MAX_RANKING, pFile);
		fclose(pFile);
	}
	else
	{
		for (int nCntRanking = 0; nCntRanking < MAX_RANKING; nCntRanking++)
		{
			m_aScore[nCntRanking] = 0;
		}
	}
}

//=============================
// スコアを並び換える
//=============================
void CRankingOffline::SortScore(void)
{
	int nSub = 0; // 判定用仮代入

	for (int nCount = 0; nCount < MAX_RANKING + 1; nCount++)
	{
		for (int nCount1 = 0; nCount1 < MAX_RANKING + 1; nCount1++)
		{
			if (m_aScore[nCount] > m_aScore[nCount1])
			{ // 先頭要素が判定中の要素より大きい場合、入れ替える
				nSub = m_aScore[nCount];				// 仮代入に先頭要素を代入
				m_aScore[nCount] = m_aScore[nCount1];	// 判定した数字を先頭要素に代入する
				m_aScore[nCount1] = nSub;				// 数値を戻す
			}
		}
	}
	m_aScore[MAX_RANKING] = m_nLatest;

	// データ書き出し
	SaveFile();
}