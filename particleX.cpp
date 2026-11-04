//==========================================
// 
// パーティクルモデル処理[particleX.h]
// Author : Kajino Hiyori _Create2026/09/28~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "input.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "scene.h"
//#include "title.h"
//#include "tutorial.h"
#include "game.h"
//#include "result.h"
#include "ranking.h"
#include "field.h"
#include "particleX.h"
#include "camera.h"
//#include "wall.h"
#include "particle.h"
#include "debugproc.h"
#include "effect.h"

//=============================
// マクロ定義
//=============================
#define SIZE			(20.0f)	// モデルの基本サイズ
#define FIELD_NORMAL	(D3DXVECTOR3(0.0f, 1.0f, 0.0f))	// フィールド情報
#define CORRECTION_ROT	(0.1f)				// 回転の減衰係数
#define CORRECTION_MOVE	(0.1f)				// 移動の減衰係数
#define NOT_JUMP		(1.35f)	// ジャンプの反転を停止
#define TRANSPOTATION	(10.0f)	// 運搬時の上昇値
#define BUMP_DELETE		(3.0f)	// 一定高度以上で削除
#define GRAVITY_RATE	(0.5f)	// 重力管理
#define LIFE			(120)	// 寿命
#define DIS_SCALE		(0.1f)	// 拡大率を変更
#define RANDOM_XZ	((float)(rand() % 1000 - 500))	// ランダムな生成XZ
#define RANDOM_Y	((float)(rand() % 200) + 100)	// ランダムな生成Y
#define RANDOM_POS	(D3DXVECTOR3(RANDOM_XZ, RANDOM_Y, RANDOM_XZ))	// ランダムな生成位置
#define SPEED_EXPLOSION	(1.5f)	// 爆発速度
#define MOVE_PARTICLE	(10)					// パーティクル移動量のランダム数
#define MOVE_HALF		(MOVE_PARTICLE / 2)		// パーティクル移動量のランダム数
#define MOVE_ADJUST		(1.0f)		// 移動量の値を指定
#define MOVE_MIN		(4.0f)		// 最低限の移動量
#define RAND_MODELSCALE	((float)(rand() % 10) * 0.5f)	// 出現サイズのランダム化

//=============================
// 静的メンバ変数宣言
//=============================
int CParticleX::m_nNumAll = 0;
const char* CParticleX::m_apModelName[MAX_PARTICLE_X] =
{
	"data\\MODEL\\particle\\particle000.x",
	"data\\MODEL\\particle\\particle001.x",
	"data\\MODEL\\particle\\particle002.x",
};

//=============================
// コンストラクタ
//=============================
CParticleX::CParticleX(CObject::PRIORITY priority): CObjectX(priority)
{
	m_posOld = INIT_D3DVEC3;	// モデルの1フレーム前の位置
	m_move = INIT_D3DVEC3;		// モデルの移動量
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);	// 拡大率
	m_bJump = false;			// ジャンプフラグ
	m_fSize = 0.0f;				// サイズ
	m_nLife = 0;				// 寿命
	m_col = COLOR_WHITE;		// 色
	m_type = TYPE_000;			// 種類
	m_nNumAll++;
}

//=============================
// デストラクタ
//=============================
CParticleX::~CParticleX()
{

}

//=============================
// パーティクルモデル生成処理
//=============================
CParticleX* CParticleX::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXVECTOR3 scl, TYPE Type, D3DXCOLOR col)
{
	CParticleX* pParticleX = NULL;

	if (pParticleX == NULL)
	{
		pParticleX = new CParticleX;	// インスタンス生成
		
		if (pParticleX != NULL)
		{ // インスタンス生成に成功
			// パーティクルモデルの初期化処理
			pParticleX->Init(pos, rot, move, scl, Type, col);

			return pParticleX;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CParticleX::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXVECTOR3 scl, TYPE Type, D3DXCOLOR col)
{
	// オブジェクトにタイプを指定
	CObjectX::Init(pos, rot, m_apModelName[Type], true, scl);
	CObjectX::SetType(CObject::TYPE_PARTICLE);
	
	m_posOld = pos;	// モデルの1フレーム前の位置
	// サイズ情報取得
	m_fSize = scl.x * (CObjectX::GetVtxMax().y);
	m_nLife = LIFE;
	// 状態管理
	m_move = move;
	m_col = col;
	m_type = Type;
	if (m_type == TYPE_000)
	{
		CObjectX::SetColor(CObjectX::COLORTYPE_CHANGE, m_col);
	}
	return S_OK;
}

//=============================
// 終了処理
//=============================
void CParticleX::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// Xオブジェクトの終了処理
	CObjectX::Uninit();
}

//=============================
// 更新処理
//=============================
void CParticleX::Update(void)
{
	// 過去の情報を保存
	m_posOld = CObjectX::GetPosition();
	
	switch (CManager::GetMode())
	{
	//case CScene::MODE_TITLE:	// タイトル
	//	// タイトル処理
	//	UpdateTitle();
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
	//	// リザルト処理
	//	UpdateResult();
	//	break;

	//case CScene::MODE_RANKING:	// ランキング
	//	UpdateRanking();
	//	break;
	}

	D3DXVECTOR3 rot = CObjectX::GetRotation();
	rot.y += 0.05f;
	CObjectX::SetRotation(rot);
	if (m_type == TYPE_000)
	{
		CObjectX::SetColor(CObjectX::COLORTYPE_CHANGE, m_col);
	}
}

//=============================
// 描画処理
//=============================
void CParticleX::Draw(void)
{
	// Xオブジェクトの描画処理
	CObjectX::Draw();
}

//=============================
// タイトル処理
//=============================
void CParticleX::UpdateTitle(void)
{
	//// フィールド情報を取得
	//CTitle* pTitle = static_cast<CTitle*>(CManager::GetScene());
	//CField* pField = pTitle->GetField();

	//// 移動処理
	//Move();

	//// 縮小処理
	//if (DisScale() == false)
	//{ // 自身を破棄済み
	//	return;
	//}

	//// 地面との判定処理
	//CollisionField(pField);

	//// 寿命を減らして管理
	//m_nLife--;
	//if (m_nLife < 0)
	//{ // 自身を破棄
	//	Release();
	//	return;
	//}
}

//=============================
// チュートリアル処理
//=============================
void CParticleX::UpdateTutorial(void)
{
	//// フィールド情報を取得
	//CTutorial* pTutorial = static_cast<CTutorial*>(CManager::GetScene());
	//CField* pField = pTutorial->GetField();

	//// 移動処理
	//Move();

	//// 縮小処理
	//if (DisScale() == false)
	//{ // 自身を破棄済み
	//	return;
	//}

	//// 地面との判定処理
	//CollisionField(pField);

	//// 寿命を減らして管理
	//m_nLife--;
	//if (m_nLife < 0)
	//{ // 自身を破棄
	//	Release();
	//	return;
	//}
}

//=============================
// ゲーム処理
//=============================
void CParticleX::UpdateGame(void)
{
	//// フィールド情報を取得
	//CGame* pGame = static_cast<CGame*>(CManager::GetScene());
	//CField* pField = pGame->GetField();

	//if (pGame->GetEnablePause() == true) return;

	//// 移動処理
	//Move();

	//// 縮小処理
	//if (DisScale() == false)
	//{ // 自身を破棄済み
	//	return;
	//}

	//// 地面との判定処理
	//CollisionField(pField);

	//// 寿命を減らして管理
	//m_nLife--;
	//if (m_nLife < 0)
	//{ // 自身を破棄
	//	Release();
	//	return;
	//}
}

//=============================
// リザルト処理
//=============================
void CParticleX::UpdateResult(void)
{
	//// フィールド情報を取得
	//CResult* pResult = static_cast<CResult*>(CManager::GetScene());
	//CField* pField = pResult->GetField();

	//// 移動処理
	//Move();

	//// 縮小処理
	//if (DisScale() == false)
	//{ // 自身を破棄済み
	//	return;
	//}

	//// 地面との判定処理
	//CollisionField(pField);

	//// 寿命を減らして管理
	//m_nLife--;
	//if (m_nLife < 0)
	//{ // 自身を破棄
	//	Release();
	//	return;
	//}
}

//=============================
// ランキング処理
//=============================
void CParticleX::UpdateRanking(void)
{
	//// フィールド情報を取得
	//CRanking* pRanking = static_cast<CRanking*>(CManager::GetScene());
	//CField* pField = pRanking->GetField();

	//// 移動処理
	//Move();

	//// 縮小処理
	//if (DisScale() == false)
	//{ // 自身を破棄済み
	//	return;
	//}

	//// 地面との判定処理
	//CollisionField(pField);

	//// 寿命を減らして管理
	//m_nLife--;
	//if (m_nLife < 0)
	//{ // 自身を破棄
	//	Release();
	//	return;
	//}
}

//=============================
// メッシュフィールドとの判定
//=============================
void CParticleX::CollisionField(CField* pField)
{
	//D3DXVECTOR3 pos = CObjectX::GetPosition();
	//float fHeight = pField->GetHeight(D3DXVECTOR3(pos.x, pos.y + m_fSize, pos.z));
	//
	//if (pos.y - m_fSize <= fHeight)
	//{ // 地面に埋まった場合
	//	pos.y = fHeight + m_fSize;
	//	m_move.x *= -1.0f;	// 地面に跳ね返る
	//	m_move.y *= -2.0f;	// 地面に跳ね返る
	//	m_move.x *= -1.0f;	// 地面に跳ね返る
	//}

	//// フィールド上で行動範囲を制限[X]
	//if (pField->GetPosition().x - pField->GetWidth() >= pos.x - m_fSize)
	//{
	//	pos.x = pField->GetPosition().x - pField->GetWidth() + m_fSize;
	//}
	//else if (pField->GetPosition().x + pField->GetWidth() <= pos.x + m_fSize)
	//{
	//	pos.x = pField->GetPosition().x + pField->GetWidth() - m_fSize;
	//}
	//// フィールド上で行動範囲を制限[Z]
	//if (pField->GetPosition().z - pField->GetDepth() >= pos.z - m_fSize)
	//{
	//	pos.z = pField->GetPosition().z - pField->GetDepth() + m_fSize;
	//}
	//else if (pField->GetPosition().z + pField->GetDepth() <= pos.z + m_fSize)
	//{
	//	pos.z = pField->GetPosition().z + pField->GetDepth() - m_fSize;
	//}

	//// 位置更新
	//CObjectX::SetPosition(pos);
}

//=============================
// 移動処理
//=============================
void CParticleX::Move(void)
{
	D3DXVECTOR3 pos = CObjectX::GetPosition();

	// カメラ取得
	CCamera* pCamera = CManager::GetCamera();
	D3DXVECTOR3 rotCamera = pCamera->GetRotation();

	// 重力
	m_move.y -= GRAVITY* GRAVITY_RATE;
	// 移動量の更新
	m_move.y += (0.0f - m_move.y) * CORRECTION_MOVE;
	// 位置の更新
	pos += m_move;

	// 移動量の更新
	m_move.x += (0.0f - m_move.x) * CORRECTION_MOVE;
	m_move.z += (0.0f - m_move.z) * CORRECTION_MOVE;

	// 位置更新
	CObjectX::SetPosition(pos);
}

//=============================
// 縮小処理
//=============================
bool CParticleX::DisScale(void)
{
	D3DXVECTOR3 scl = CObjectX::GetScale();
	scl.x -= DIS_SCALE;
	scl.y = scl.x;
	scl.z = scl.x;

	m_fSize = scl.x * (CObjectX::GetVtxMax().y);	// サイズ変更
	CObjectX::SetScale(scl);

	if (scl.x <= 0.0f)
	{ // 0を下回ったら破棄
		// 自身を破棄
		Release();
		return false;
	}

	return true;
}

//=============================
// ランダムでポップコーン生成
//=============================
void CParticleX::RandomPopcorn(void)
{
	D3DXVECTOR3 move = INIT_D3DVEC3;
	move.x = (float)(rand() % MOVE_PARTICLE - MOVE_HALF) * SPEED_EXPLOSION;
	move.y = (float)(rand() % MOVE_HALF) * SPEED_EXPLOSION;
	move.z = (float)(rand() % MOVE_PARTICLE - MOVE_HALF) * SPEED_EXPLOSION;
	D3DXVECTOR3 scl = INIT_D3DVEC3;
	scl.x = RAND_MODELSCALE;
	scl.y = scl.x;
	scl.z = scl.x;
	CParticleX::Create(RANDOM_POS, D3DXVECTOR3(RAND_ROT, RAND_ROT, RAND_ROT), move, scl, CParticleX::TYPE_002, COLOR_WHITE);
}
