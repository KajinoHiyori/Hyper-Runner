//==========================================
// 
// パーティクル処理[particle.h]
// Author : Kajino Hiyori _Create2026/05/18~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "particle.h"
#include "input.h"
#include "manager.h"
#include "renderer.h"
#include "effect.h"
#include "smoke.h"
#include "debugproc.h"

//=============================
// マクロ定義
//=============================
#define FIREFLOWER_MAX	(1)		// 1フレーム当たりの最大数[花火型]
#define DIGG_MAX		(3)		// 1フレーム当たりの最大数[掘り返し型]
#define FIREFLOWER_LIFE	(60)	// パーティクルの寿命[花火型]
#define FIREFLOWER_POS	(10.0f)	// 発生位置の設定[花火型]
#define EFFECT_LIFE		(30)	// エフェクトの寿命
#define SPEED_EXPLOSION	(1.5f)	// 爆発速度
#define MOVE_PARTICLE	(10)					// パーティクル移動量のランダム数
#define MOVE_HALF		(MOVE_PARTICLE / 2)		// パーティクル移動量のランダム数
#define MOVE_ADJUST		(1.0f)		// 移動量の値を指定
#define MOVE_MIN		(4.0f)		// 最低限の移動量
#define ANGLE_DOUBLE	(629)		// 角度の最大数(*2)
#define MAX_ANGLE		(314)		// 角度の最大数
#define ANGLE_ADJUST	(100.0f)	// 角度の値を指定
#define CRASH_SIZE		(5.0f)		// 衝突時のエフェクトサイズ
#define RAND_ROT		((rand() % 628 - 314) / 100.0f)	// 角度のランダム値
#define RANDOM_ROT		(D3DXVECTOR3(RAND_ROT, RAND_ROT, RAND_ROT))	// ランダムで角度を設定
#define RAND_MODELSCALE	((float)(rand() % 5) * 0.5f)	// 出現サイズのランダム化
#define SOIL_RANDOM		((float)(rand() % 20) + 5.0f)	// 土エフェクトのサイズ
#define DIGG_RANDOM		((float)(rand() % 10) * 0.1f + 0.5f)	// 掘り返しのサイズ

//=============================
// 静的メンバ変数宣言
//=============================
int CParticle::m_nNumAll = 0;	// 総数のクリア

//=============================
// コンストラクタ
//=============================
CParticle::CParticle(CObject::PRIORITY priority) : CObject(priority)
{
	// 値のクリア
	m_nLife = 0;	// パーティクル管理の値をクリア
	m_nMax = 0;  // 1フレーム当たりの最大数をクリア
	m_col = COLOR_WHITE;	// 色指定
	m_type = TYPE_FIREFLOWER;	// 種類
	m_bGravity = false;	// 重力判定
	m_modelType = CParticleX::TYPE_000;	// モデルの種類
	m_nNumAll++;	// 総数の加算
}

//=============================
// デストラクタ
//=============================
CParticle::~CParticle()
{

}

//=============================
// パーティクル生成処理
//=============================
CParticle* CParticle::Create(D3DXVECTOR3 pos, TYPE type, D3DXCOLOR col, int nLife, bool bGravity)
{
	CParticle* pParticle = NULL;

	if (pParticle == NULL)
	{
		pParticle = new CParticle;
		if (pParticle != NULL)
		{ // インスタンス生成に成功
			if (type == TYPE_DIGGER || type == TYPE_LEAF)
			{
				pParticle->m_nMax = DIGG_MAX;
			}
			else
			{
				pParticle->m_nMax = FIREFLOWER_MAX;	// 最大数設定
			}
			pParticle->m_nLife = nLife;	// パーティクルの寿命
			pParticle->m_col = col;	// 色指定

			// パーティクルの初期化処理
			pParticle->Init(pos, type);
			pParticle->m_bGravity = bGravity;

			return pParticle;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// パーティクル生成処理
//=============================
CParticle* CParticle::Create(D3DXVECTOR3 pos, CParticleX::TYPE modeltype, D3DXCOLOR col, int nLife)
{
	CParticle* pParticle = NULL;

	if (pParticle == NULL)
	{
		pParticle = new CParticle;
		pParticle->m_type = TYPE_MODEL;
		if (pParticle != NULL)
		{ // インスタンス生成に成功
			pParticle->m_nMax = FIREFLOWER_MAX;	// 最大数設定
			// パーティクルの初期化処理
			pParticle->Init(pos, modeltype, col, nLife);

			return pParticle;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}


//=============================
// 初期化処理
//=============================
HRESULT CParticle::Init(D3DXVECTOR3 pos, TYPE type)
{
	// 設定位置の保存
	m_pos = pos;

	D3DXVECTOR3 posLocal = INIT_D3DVEC3;	// ローカルで位置保存
	m_type = type;	// 種類格納

	SetType(TYPE_PARTICLE);

	return S_OK;
}

//=============================
// 初期化処理
//=============================
HRESULT CParticle::Init(D3DXVECTOR3 pos, CParticleX::TYPE modeltype, D3DXCOLOR col, int nLife)
{
	// 設定位置の保存
	m_pos = pos;
	m_modelType = modeltype;
	m_col = col;
	m_nLife = nLife;

	SetType(TYPE_PARTICLE);	

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CParticle::Uninit(void)
{
	m_nNumAll--;	// 総数の減算
}

//=============================
// 更新処理
//=============================
void CParticle::Update(void)
{
	m_nLife--;	// 寿命の減少

	CDebugProc* pDebugProc = CManager::GetDebugProc();

	if (m_nLife <= 0)
	{ // 寿命が尽きた
		// 終了処理
		Release();
		return;	// 早期リターン
	}

	D3DXVECTOR3 pos = INIT_D3DVEC3;	// 発生位置の設定
	// エフェクトの用意
	int nCntEffect = 0;	// エフェクト数の計算

	for (int nCntParticle = 0; nCntParticle < MAX_EFFECT; nCntParticle++)
	{
		D3DXCOLOR col = COLOR_WHITE;	// 色指定
		CEffect* pEffect = NULL;		// エフェクト指定

		// 初期化が行われている&&寿命が残っている
		// 位置、移動量設定
		pos = m_pos;
		D3DXVECTOR3 move = INIT_D3DVEC3;	// 移動量の設定
		move.x = (float)(rand() % MOVE_PARTICLE - MOVE_HALF) * SPEED_EXPLOSION;
		move.y = (float)(rand() % MOVE_HALF) * SPEED_EXPLOSION;
		move.z = (float)(rand() % MOVE_PARTICLE - MOVE_HALF) * SPEED_EXPLOSION;
		D3DXVECTOR3 scl = INIT_D3DVEC3;	// 拡大率の変更

		switch (m_type)
		{
		case TYPE_RANDCOL:	// 色ランダム
			col.r = RAND_COLOR;
			col.g = RAND_COLOR;
			col.b = RAND_COLOR;
			col.a = 1.0f;
			CEffect::Create(pos, move, col, CEffect::TYPE_001, EFFECT_LIFE, m_bGravity);
			break;

		case TYPE_EXPLOSION:	// 爆発
			col = m_col;
			CEffect::Create(pos, move, col, CEffect::TYPE_000, EFFECT_LIFE, m_bGravity);
			break;

		case TYPE_RECOVER:	// 回復
			col = m_col;
			CEffect::Create(pos, move, col, CEffect::TYPE_002, EFFECT_LIFE, m_bGravity);
			break;

		case TYPE_SMOKE:	// 煙
			// 色に応じた煙
			CSmoke::Create(m_pos, move*0.75f, m_col, SOIL_RANDOM);
			break;

		case TYPE_CRASH:	// 衝突
			col = m_col;
			pEffect = CEffect::Create(pos, move, col, CEffect::TYPE_002, EFFECT_LIFE, m_bGravity);
			pEffect->SetSize(CRASH_SIZE);
			break;

		case TYPE_SOIL:	// 土
			CSmoke::Create(m_pos, D3DXVECTOR3(move.x, 0.0f, move.z), m_col, SOIL_RANDOM);
			break;

		case TYPE_DIGGER:	// 掘り返し
			CSmoke::Create(m_pos, move, COLOR_LIGHTBROWN, DIGG_RANDOM);
			move.x = sinf((float)((rand() % 628) - 314) / 100.0f) * 10.0f;
			move.y = (rand() % 10 + 0.5f);
			move.z = cosf((rand() % 628 - 314) / 100.0f) * 10.0f;
			scl.x = RAND_MODELSCALE;
			scl.y = scl.x;
			scl.z = scl.x; 
			CParticleX::Create(pos, RANDOM_ROT, move, scl, m_modelType, COLOR_DARKBROWN);
			break;

		case TYPE_LEAF:	// 葉
			move.x = sinf((float)((rand() % 628) - 314) / 100.0f) * 10.0f;
			move.y = (rand() % 10 + 0.5f);
			move.z = cosf((rand() % 628 - 314) / 100.0f) * 10.0f;
			scl.x = RAND_MODELSCALE;
			scl.y = scl.x;
			scl.z = scl.x;
			CParticleX::Create(pos, RANDOM_ROT, move, scl, CParticleX::TYPE_001, COLOR_GREEN);
			break;

		case TYPE_MODEL:	// モデル
			scl.x = RAND_MODELSCALE;
			scl.y = scl.x;
			scl.z = scl.x;
			move = INIT_D3DVEC3;	// 移動量の設定
			move.x = sinf((float)((rand() % 628) - 314) / 100.0f) * 10.0f;
			move.y = (rand() % 10 + 0.5f);
			move.z = cosf((rand() % 628 - 314) / 100.0f) * 10.0f;
			CParticleX::Create(pos, RANDOM_ROT, move, scl, m_modelType, m_col);
			break;

		default:
			col = m_col;
			CEffect::Create(pos, move, col, CEffect::TYPE_000, EFFECT_LIFE, m_bGravity);
			break;
		}
		
		nCntEffect++;	// 1フレーム当たりの生成数をカウント

		if (nCntEffect > m_nMax)
		{ // 規定値を越えたら処理を抜ける
			break;
		}
	}
}

//=============================
// 描画処理
//=============================
void CParticle::Draw(void)
{

}
