//==========================================
// 
// パーティクルモデル処理[particleX.h]
// Author : Kajino Hiyori _Create2026/09/28~
// 
//==========================================
#ifndef _PARTICLE_X_H_
#define _PARTICLE_X_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "objectX.h"

//=============================
// マクロ定義
//=============================
#define MAX_PARTICLE_X	(TYPE_MAX)	// パーティクルモデルの種類の最大数

//=============================
// 前方宣言
//=============================
class CField;		// フィールドクラス
class CWall;	// 壁クラス

//=============================
// パーティクルモデルクラス
//=============================
class CParticleX : public CObjectX
{
public:
	typedef enum
	{
		TYPE_000 = 0,	// シンプルな玉型
		TYPE_001,		// 葉型
		TYPE_002,		// ポップコーン
		TYPE_MAX
	}TYPE;

	CParticleX(CObject::PRIORITY priority = CObject::PRIORITY_EVENTS);
	~CParticleX();
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXVECTOR3 scl, TYPE Type, D3DXCOLOR col);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { CObjectX::SetPosition(pos); }
	D3DXVECTOR3 GetPosition(void) { return CObjectX::GetPosition(); }
	static CParticleX* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXVECTOR3 scl, TYPE Type, D3DXCOLOR col);
	static int GetNumAll(void) { return m_nNumAll; }
	static void RandomPopcorn(void);

private:
	void UpdateTitle(void);
	void UpdateTutorial(void);
	void UpdateGame(void);
	void UpdateResult(void);
	void UpdateRanking(void);
	void CollisionField(CField* pField);
	void Move(void);
	bool DisScale(void);
	D3DXVECTOR3	m_posOld;	// モデルの1フレーム前の位置
	D3DXVECTOR3	m_move;		// モデルの移動量
	D3DXVECTOR3 m_scl;		// 拡大率
	D3DXCOLOR m_col;		// 色
	TYPE m_type;			// 種類
	bool m_bJump;			// ジャンプフラグ
	float m_fSize;			// モデルの大きさ
	int m_nLife;			// 寿命
	static int m_nNumAll;	// パーティクルモデルの総数
	static const char* m_apModelName[MAX_PARTICLE_X];	// モデル名
};

#endif