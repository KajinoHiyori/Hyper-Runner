//==========================================
// 
// パーティクル処理[particle.h]
// Author : Kajino Hiyori _Create2026/05/18~
// 
//==========================================
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"
#include "particleX.h"

//=============================
// マクロ定義
//=============================
#define MAX_EFFECT	(512)	// パーティクルの桁数

//=============================
// 前方宣言
//=============================
class CEffect;		// エフェクトクラス
class CParticleX;	// パーティクルモデルクラス

//=============================
// パーティクルクラス
//=============================
class CParticle : public CObject
{
public:
	typedef enum
	{
		TYPE_FIREFLOWER = 0,	// 花火型
		TYPE_RANDCOL,			// 色ランダム[花火型]
		TYPE_EXPLOSION,			// 爆発
		TYPE_RECOVER,			// 回復
		TYPE_SMOKE,				// 煙
		TYPE_CRASH,				// 衝突
		TYPE_SOIL,				// 土
		TYPE_DIGGER,			// 掘り返し
		TYPE_MODEL,				// モデル
		TYPE_LEAF,				// 葉
		TYPE_MAX
	}TYPE;

	// レンダーステート
	typedef enum
	{
		RENDERSTATE_NORMAL = 0,	// 通常
		RENDERSTATE_ALPHA,		// 加算合成
		RENDERSTATE_MAX
	}RENDERSTATE;

	CParticle(CObject::PRIORITY priority = CObject::PRIORITY_VISUALEFEFCT);
	~CParticle();
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(D3DXVECTOR3 pos, TYPE type);
	HRESULT Init(D3DXVECTOR3 pos, CParticleX::TYPE modeltype, D3DXCOLOR col, int nLife);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) {}
	static CParticle* Create(D3DXVECTOR3 pos, TYPE type, D3DXCOLOR col, int nLife, bool bGravity = false);
	static CParticle* Create(D3DXVECTOR3 pos, CParticleX::TYPE modeltype, D3DXCOLOR col, int nLife);
	static int GetNumAll(void) { return m_nNumAll; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }

private:
	static int m_nNumAll;	// パーティクル総数
	D3DXVECTOR3 m_pos;	// 位置
	D3DXCOLOR m_col;	// 色
	TYPE m_type;		// 種類
	int m_nLife;	// 寿命
	int m_nMax;		// 1フレーム当たりの最大数
	bool m_bGravity;	// 重力
	CParticleX::TYPE m_modelType;	// モデルの種類
};

#endif // !_PLAYER_H_

#pragma once
