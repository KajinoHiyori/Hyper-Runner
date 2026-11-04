//==========================================
// 
// エフェクト処理[effect.h]
// Author : Kajino Hiyori _Create2026/05/18~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "effect.h"
#include "input.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "scene.h"
#include "game.h"
#include "field.h"

//=============================
// マクロ定義
//=============================
#define EFFECT_SIZE		(20.0f)		// エフェクトのサイズ
#define DIS_ALPHA		(0.05f)		// 透明度を上げる
#define CORRECTION		(0.7f)		// 補正係数
#define FLOATING		(20.0f)		// 浮遊高度
#define BASE_DISTANCE	(250.0f)	// 透明度を上げる範囲
#define SCORE_UP		(1)			// スコアアップ

//=============================
// 静的メンバ変数宣言
//=============================
const char* CEffect::m_apFilenameType[CEffect::TYPE_MAX] =
{
	"data\\TEXTURE\\effect\\effect000.jpg",	// テクスチャ000
	"data\\TEXTURE\\effect\\effect001.jpg",	// テクスチャ001
	"data\\TEXTURE\\effect\\effect002.jpg",	// テクスチャ002
};
int CEffect::m_nNumAll = 0;

//=============================
// コンストラクタ
//=============================
CEffect::CEffect(CObject::PRIORITY priority): CBillboard(priority)
{
	// メンバ変数の値をクリア
	m_move = INIT_D3DVEC3;	// 移動量
	m_posDest = INIT_D3DVEC3;	// 目的地[基点追従時のみ]
	m_fSpeed = 0.0f;		// 速度[基点追従時のみ]
	m_nLife = 0;			// エフェクトの寿命
	m_bGravity = false;		// 重力[true : 重力あり / false : 重力なし]
	m_nNumAll++;			// エフェクトの総数を加算
}

//=============================
// デストラクタ
//=============================
CEffect::~CEffect()
{

}

//=============================
// エフェクト生成処理
//=============================
CEffect* CEffect::Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, TYPE type, int nLife, bool bGravity, STATE state, float fSpeed)
{
	CEffect* pEffect = NULL;

	if (pEffect == NULL)
	{
		pEffect = new CEffect;	// インスタンス生成
		
		if (pEffect != NULL)
		{ // インスタンス生成に成功
			// エフェクトの初期化処理
			pEffect->Init(pos, type, col, state, fSpeed);

			// 移動量設定
			pEffect->m_move = move;
			pEffect->m_bGravity = bGravity;

			// エフェクトの寿命設定
			pEffect->m_nLife = nLife;

			return pEffect;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CEffect::Init(D3DXVECTOR3 pos, TYPE type, D3DXCOLOR col, STATE state, float fSpeed)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// テクスチャの読み込み
	int nIdx = pTexture->Register(m_apFilenameType[type]);

	// 2Dオブジェクトの初期化
	CBillboard::Init(pos, EFFECT_SIZE, EFFECT_SIZE);
	CBillboard::SetTextureIdx(nIdx);	// テクスチャ指定
	CBillboard::SetColor(col);			// 色指定

	// オブジェクトにタイプを指定
	CBillboard::SetType(TYPE_EFFECT);
	m_state = state;
	m_fSpeed = fSpeed;

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CEffect::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// billboardの終了処理
	CBillboard::Uninit();
}

//=============================
// 更新処理
//=============================
void CEffect::Update(void)
{
	switch (CManager::GetMode())
	{
	//case CScene::MODE_TITLE:	// タイトル
	//
	//	break;
	//
	//case CScene::MODE_TUTORIAL:	// チュートリアル
	//	// チュートリアル処理
	//	UpdateTutorial();
	//	break;

	case CScene::MODE_GAME:	// ゲーム
		// ゲーム処理
		UpdateGame();
		break;

	//case CScene::MODE_RESULT:	// リザルト
	//
	//	break;
	}
}

//=============================
// 描画処理
//=============================
void CEffect::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);	// Zテストの比較方法を変更(Zバッファの前後関係に関わらず描画する)
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);		// Zバッファ更新の有効/無効の設定

	// billboardの描画処理
	CBillboard::Draw();

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

//=============================
// チュートリアル処理
//=============================
void CEffect::UpdateTutorial(void)
{
}

//=============================
// ゲーム処理
//=============================
void CEffect::UpdateGame(void)
{
	// メッシュフィールド情報を取得
	CGame* pGame = static_cast<CGame*>(CManager::GetScene());
	//CField* pField = pGame->GetField();
	
	// 通常の更新処理
	//UpdateDefault(pField);
}

//=============================
// メッシュフィールドとの判定
//=============================
void CEffect::CollisionField(CField* pField)
{
	//// 位置情報取得
	//D3DXVECTOR3 pos = CBillboard::GetPosition();

	//if (pos.y < pField->GetHeight(pos))
	//{ // 地面に埋まった場合
	//	pos.y = CBillboard::GetPosition().y;
	//	m_move.y *= -1.0f;	// 跳ね返り処理
	//}

	//// 位置情報の更新
	//CBillboard::SetPosition(pos);
}

//=============================
// STATE_DEFAULTの更新
//=============================
void CEffect::UpdateDefault(CField* pField)
{
	// 寿命の更新
	m_nLife--;

	// 移動量の更新
	D3DXVECTOR3 pos = CBillboard::GetPosition();

	// 重力の有無
	if (m_bGravity == true)
	{
		m_move.y -= GRAVITY * 0.5f;
	}

	// フィールドとの判定
	CollisionField(pField);

	// 色の変更
	D3DXCOLOR col = CBillboard::GetColor();
	col.a -= DIS_ALPHA;
	SetColor(col);

	// 移動量更新
	pos += m_move;

	// 位置情報の更新
	CBillboard::SetPosition(pos);

	// 削除処理
	if (m_nLife <= 0 || col.a < 0.0f)
	{
		// 自身の破棄
		Release();
	}
}