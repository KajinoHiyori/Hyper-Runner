//==========================================
// 
// オブジェクト処理[object.h]
// Author : Kajino Hiyori _Create2026/05/08
// 
//==========================================
#ifndef _OBJECT_H_
#define _OBJECT_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"

//=============================
// マクロ定義
//=============================
#define MAX_OBJECT		(1024)	// オブジェクト最大数
#define MAX_PRIORITYS	(PRIORITY_MAX)		// 優先順位最大数

//=============================
// オブジェクトクラス
//=============================
class CObject 
{
public:
	// オブジェクトの種類
	typedef enum
	{
		TYPE_NONE = 0,	// 初期化
		TYPE_PLAYER,	// プレイヤー
		TYPE_MACHINE,	// 機械
		TYPE_FIELD,		// 地面
		TYPE_BLOCK,		// ブロック
		TYPE_WALL,		// 壁
		TYPE_ITEM,	// アイテム
		TYPE_STEM,	// 茎
		TYPE_CORN,	// トウモロコシ
		TYPE_POPCORN,	// ポップコーン
		TYPE_TIMER,	// タイマー
		TYPE_PAUSE,	// ポーズ
		TYPE_SHADOW,	// 影
		TYPE_EFFECT,	// エフェクト
		TYPE_PARTICLE,	// パーティクル
		TYPE_MAX
	}TYPE;

	// 優先順位
	typedef enum
	{
		PRIORITY_SCENE = 0,		// シーンクラス
		PRIORITY_FIELD,			// 地面
		PRIORITY_EVENTS,		// イベント[item / obsculate]
		PRIORITY_CHARACTOR,		// キャラクター
		PRIORITY_SHADOW,		// 影
		PRIORITY_CYLINDER,		// シリンダー
		PRIORITY_MESH,			// メッシュ
		PRIORITY_UI,			// UI
		PRIORITY_TRIANGLE,		// 三角形
		PRIORITY_COUNTDOWN,		// countdown
		PRIOTITY_ADDALPHA,		// 加算合成専専用
		PRIOTITY_DISALPHA,		// 減算合成専専用
		PRIORITY_SMOKE,			// 煙
		PRIORITY_VISUALEFEFCT,	// 視覚効果
		PRIORITY_MANAGER,		// 管理[描画無し]
		PRIORITY_PAUSE,			// PAUSE
		PRIORITY_MAX
	}PRIORITY;

	CObject(PRIORITY priority = PRIORITY_EVENTS);
	virtual ~CObject();
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	static void ReleaseAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	static int GetNumAll(void) { return m_nNumAll; }
	virtual void SetPosition(D3DXVECTOR3 pos) = 0;
	void Release(void);
	void SetType(TYPE type) { m_type = type; }
	TYPE GetType(void) { return m_type; }
	static CObject* GetTop(int nIdx) { return m_apTop[nIdx]; }
	CObject* GetNext(void) { return m_pNext; }

private:
	static CObject* m_apTop[MAX_PRIORITYS];	// 先頭オブジェクトへのポインタ
	static CObject* m_apCur[MAX_PRIORITYS];	// 最後尾オブジェクトへのポインタ
	CObject* m_pPrev;		// 前のオブジェクトへのポインタ
	CObject* m_pNext;		//  次のオブジェクトへのポインタ
	bool m_bDeath;			// 死亡フラグ

	static int m_nNumAll;	// 総数を格納
	int m_nPriority;	// 自身の優先順位を格納
	TYPE m_type;		// オブジェクトの種類を格納
};

#endif