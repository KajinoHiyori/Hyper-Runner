//========================================================
// 
// アイテム処理[rightplayer.cpp]
// Author : Kajino Hiyori _Create2026/06/02
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "block.h"
#include "camera.h"
#include "input.h"
#include "color.h"
#include "debugproc.h"
#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "ObjectX.h"
#include "field.h"
#include "scene.h"
#include "game.h"
#include "ranking.h"
#include "effect.h"

//=============================
// マクロ定義
//=============================
#define PARTICLE_LIFE		(20)	// パーティクル寿命
#define EXPLOSION_DAMAGE	(360)	// 爆発によるダメージ
#define NOT_JUMP			(0.1f)	// ジャンプ停止
#define BLOCKINFO			("BLOCKINFO")		// BLOCKINFO
#define END_BLOCKINFO		("END_BLOCKINFO")	// END_BLOCKINFO
#define NUMBLOCK			("NUMBLOCK")		// NUMBLOCK
#define BLOCKSET			("BLOCKSET")	// BLOCKSET
#define END_BLOCKSET		("END_BLOCKSET")	// END_BLOCKSET
#define TYPE				("TYPE")				// TYPE
// ファイルパス
#define BLOCKINFO_PATH		("data\\SCRIPTS\\block_info.txt")	// モデル名情報

//=============================
// 静的メンバ変数宣言
//=============================
int CBlock::m_nNumAll = 0;
char CBlock::m_apModelName[MAX_BLOCK][MAX_STRING] = {};

//=============================
// コンストラクタ
//=============================
CBlock::CBlock(CObject::PRIORITY priority) : CObjectX(priority)
{
	// 値のクリア
	m_move = INIT_D3DVEC3;	// 移動量
	m_fLength = 0.0f;	// 対角線の長さ
	m_fAngle = 0.0f;	// 対角線の角度
	m_fWidth = 0.0f;	// 横幅
	m_fDepth = 0.0f;	// 奥行き
	m_nType = 0;		// 種類
	m_nNumAll++;
}

//=============================
// デストラクタ
//=============================
CBlock::~CBlock()
{

}

//=============================
// アイテム生成処理
//=============================
CBlock* CBlock::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int type)
{
	CBlock* pBlock = NULL;	// オブジェクトへのポインタを用意
	if (pBlock == NULL)
	{ // NULLが設定できている場合
		pBlock = new CBlock;
		if (pBlock != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pBlock->Init(pos, rot, type);

			return pBlock;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CBlock::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int type)
{
	m_nType = type;
	// モデルを用意
	CObjectX::Init(pos, rot, m_apModelName[type]);
	// 種類設定
	SetType(TYPE_BLOCK);
	D3DXVECTOR3 vtxMin = CObjectX::GetVtxMin();	// 最小値
	D3DXVECTOR3 vtxMax = CObjectX::GetVtxMax();	// 最大値
	m_fWidth = vtxMax.x - vtxMin.x;	// 横幅
	m_fDepth = vtxMax.z - vtxMin.z;	// 奥行き
	m_fLength = sqrtf(m_fWidth * m_fWidth + m_fDepth * m_fDepth) * 0.5f;	// 対角線の長さ
	m_fAngle = atan2f(m_fWidth, m_fDepth);				// 対角線の長さ
	return S_OK;
}

//=============================
// 終了処理
//=============================
void CBlock::Uninit(void)
{
	// オブジェクト破棄
	CObjectX::Uninit();
	m_nNumAll--;
}

//=============================
// 更新処理
//=============================
void CBlock::Update(void)
{
	//if (m_nType == 0) return;	// フェンスの場合早期リターン

	// シーン情報を取得
	CScene::MODE mode = CManager::GetMode();

	// シーンごとに処理を分割
	switch (mode)
	{
	//case CScene::MODE_TITLE:	// タイトル
	//
	//	break;
	//
	//case CScene::MODE_TUTORIAL:	// チュートリアル
	//	// チュートリアル処理
	//	UpdateTutorial();
	//	break;

	case CScene::MODE_GAME:		// ゲーム
		// ゲーム処理
		UpdateGame();
		break;

	//case CScene::MODE_RESULT:	// リザルト
	//
	//	break;
	//
	//case CScene::MODE_RANKING:	// ランキング
	//	break;
	//
	//case CScene::MODE_CREDIT:	// クレジット
	//	break;
	}
}

//=============================
// 描画処理
//=============================
void CBlock::Draw(void)
{
	// アイテムの描画
	CObjectX::Draw();
}

//=============================
// 当たり判定[XZ]
//=============================
bool CBlock::Collision(D3DXVECTOR3* pPos, D3DXVECTOR3 posOld, float fRadius, float fMargin)
{
	D3DXVECTOR3 posLocal = *pPos;
	D3DXVECTOR3 posLocal1 = *pPos;

	bool bCollision = false;	// 衝突判定

	// オブジェクト情報の獲得
	D3DXVECTOR3 posObj = CObjectX::GetPosition();	// 位置
	D3DXVECTOR3 rotObj = CObjectX::GetRotation();	// 向き
	D3DXVECTOR3 vtxMin = CObjectX::GetVtxMin();		// 最小値
	D3DXVECTOR3 vtxMax = CObjectX::GetVtxMax();		// 最大値

	// 各頂点の座標計算
	D3DXVECTOR3 posLeftBack = INIT_D3DVEC3;		// モデル左奥の座標
	posLeftBack.x = posObj.x - sinf(-rotObj.y + m_fAngle) * (m_fLength + fRadius);
	posLeftBack.y = 0.0f;
	posLeftBack.z = posObj.z + cosf(-rotObj.y + m_fAngle) * (m_fLength + fRadius);
	
	D3DXVECTOR3 posRightBack = INIT_D3DVEC3;	// モデル右奥の座標
	posRightBack.x = posObj.x + sinf(rotObj.y + m_fAngle) * (m_fLength + fRadius);
	posRightBack.y = 0.0f;
	posRightBack.z = posObj.z + cosf(rotObj.y + m_fAngle) * (m_fLength + fRadius);
	
	D3DXVECTOR3 posLeftFront = INIT_D3DVEC3;	// モデル左手前の座標
	posLeftFront.x = posObj.x - sinf(rotObj.y + m_fAngle) * (m_fLength + fRadius);
	posLeftFront.y = 0.0f;
	posLeftFront.z = posObj.z - cosf(rotObj.y + m_fAngle) * (m_fLength + fRadius);
	
	D3DXVECTOR3 posRightFront = INIT_D3DVEC3;	// モデル右手前の座標
	posRightFront.x = posObj.x + sinf(-rotObj.y + m_fAngle) * (m_fLength + fRadius);
	posRightFront.y = 0.0f;
	posRightFront.z = posObj.z - cosf(-rotObj.y + m_fAngle) * (m_fLength + fRadius);

	D3DXVECTOR3 vecLineLBRB = posRightBack - posLeftBack;	// 左奥→右奥
	D3DXVec3Normalize(&vecLineLBRB, &vecLineLBRB);
	D3DXVECTOR3 vecLineRBRF = posRightFront - posRightBack;	// 右奥→右手前
	D3DXVec3Normalize(&vecLineRBRF, &vecLineRBRF);
	D3DXVECTOR3 vecLineRFLF = posLeftFront - posRightFront;	// 右手前→左手前
	D3DXVec3Normalize(&vecLineRFLF, &vecLineRFLF);
	D3DXVECTOR3 vecLineLFLB = posLeftBack - posLeftFront;	// 左手前→左奥
	D3DXVec3Normalize(&vecLineLFLB, &vecLineLFLB);

	D3DXVECTOR3 vecMove = posLocal - posOld;	// 移動ベクトル
	D3DXVec3Normalize(&vecMove, &vecMove);
	D3DXVECTOR3 vecToPos = INIT_D3DVEC3;		// 対象ベクトル
	float fRate = 0.0f;	// 全体比率
	float fAllArea = 0.0f;	// 全体面積
	float fRateArea = 0.0f;	// 比率面積
	D3DXVECTOR3 posSet = INIT_D3DVEC3;

	// オブジェクトとの当たり判定[XZ]
	if ((CrossCollisionLeft(posLocal, posLeftBack, vecLineLBRB) == true)		// 左奥基準
		&& (CrossCollisionLeft(posLocal, posRightFront, vecLineRFLF) == true)	// 右手前基準
		&& (CrossCollisionLeft(posLocal, posLeftFront, vecLineLFLB) == true)	// 左手前基準
		&& (CrossCollisionLeft(posLocal, posRightBack, vecLineRBRF) == true))	// 右奥基準
	{ // オブジェクトにめり込んでいる
		// どこから侵入したかの判定
		if (CrossCollisionLeft(posOld, posLeftBack, vecLineLBRB) == false)
		{ // 左奥から侵入
			vecToPos = posLocal - posLeftBack;	// 対象ベクトル
			fAllArea = (vecLineLBRB.z * vecMove.x) - (vecLineLBRB.x * vecMove.z);	// 全体面積
			fRateArea = (vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z);	// 比率面積
			fRate = fRateArea / fAllArea;	// 比率を求める

			// 余白分を含めて計算
			posLeftBack.x = posObj.x - sinf(-rotObj.y + m_fAngle) * (m_fLength + fRadius + fMargin);
			posLeftBack.y = 0.0f;
			posLeftBack.z = posObj.z + cosf(-rotObj.y + m_fAngle) * (m_fLength + fRadius + fMargin);

			// 位置更新
			posLocal.x = vecLineLBRB.x * fRate + posLeftBack.x;
			posLocal.z = vecLineLBRB.z * fRate + posLeftBack.z;
		}
		else if (CrossCollisionLeft(posOld, posRightBack, vecLineRBRF) == false)
		{ // 右奥から侵入
			vecToPos = posLocal - posRightBack;	// 対象ベクトル
			fAllArea = (vecLineRBRF.z * vecMove.x) - (vecLineRBRF.x * vecMove.z);	// 全体面積
			fRateArea = (vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z);	// 比率面積
			fRate = fRateArea / fAllArea;	// 比率を求める

			// 余白分を含めて計算
			posRightBack.x = posObj.x + sinf(rotObj.y + m_fAngle) * (m_fLength + fRadius + fMargin);
			posRightBack.y = 0.0f;
			posRightBack.z = posObj.z + cosf(rotObj.y + m_fAngle) * (m_fLength + fRadius + fMargin);

			// 位置更新
			posLocal.x = vecLineRBRF.x * fRate + posRightBack.x;
			posLocal.z = vecLineRBRF.z * fRate + posRightBack.z;
		}
		else if (CrossCollisionLeft(posOld, posLeftFront, vecLineLFLB) == false)
		{ // 左手前から侵入
			vecToPos = posLocal - posLeftFront;	// 対象ベクトル
			fAllArea = (vecLineLFLB.z * vecMove.x) - (vecLineLFLB.x * vecMove.z);	// 全体面積
			fRateArea = (vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z);	// 比率面積
			fRate = fRateArea / fAllArea;	// 比率を求める

			// 余白分を含めて計算
			posLeftFront.x = posObj.x - sinf(rotObj.y + m_fAngle) * (m_fLength + fRadius + fMargin);
			posLeftFront.y = 0.0f;
			posLeftFront.z = posObj.z - cosf(rotObj.y + m_fAngle) * (m_fLength + fRadius + fMargin);

			// 位置更新
			posLocal.x = vecLineLFLB.x * fRate + posLeftFront.x;
			posLocal.z = vecLineLFLB.z * fRate + posLeftFront.z;
		}
		else if (CrossCollisionLeft(posOld, posRightFront, vecLineRFLF) == false)
		{ // 右手前から侵入
			vecToPos = posLocal - posRightFront;	// 対象ベクトル
			fAllArea = (vecLineRFLF.z * vecMove.x) - (vecLineRFLF.x * vecMove.z);	// 全体面積
			fRateArea = (vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z);	// 比率面積
			fRate = fRateArea / fAllArea;	// 比率を求める

			// 余白分を含めて計算
			posRightFront.x = posObj.x + sinf(-rotObj.y + m_fAngle) * (m_fLength + fRadius + fMargin);
			posRightFront.y = 0.0f;
			posRightFront.z = posObj.z - cosf(-rotObj.y + m_fAngle) * (m_fLength + fRadius + fMargin);

			// 位置更新
			posLocal.x = vecLineRFLF.x * fRate + posRightFront.x;
			posLocal.z = vecLineRFLF.z * fRate + posRightFront.z;
		}
		bCollision = true;
		*pPos = posLocal;
		return bCollision;
	}

	return bCollision;
}

//=============================
// 当たり判定[Y]
//=============================
bool CBlock::CollisionHeight(D3DXVECTOR3 pos, D3DXVECTOR3 posOld, float fHeight, float fMargi, bool bCollisionn)
{
	D3DXVECTOR3 posBlock = CObjectX::GetPosition();
	D3DXVECTOR3 vtxMax = CObjectX::GetVtxMax(); 
	if (posOld.y < pos.y || pos.y <= posBlock.y)return false;
	
	if (pos.y < posBlock.y + vtxMax.y)
	{
		return true;
	}
	return false;
}

//=============================
// 設定用の当たり判定
//=============================
bool CBlock::SetCollision(D3DXVECTOR3* pPos, float fRadius)
{
	D3DXVECTOR3 posLocal = *pPos;
	D3DXVECTOR3 posLocal1 = *pPos;

	// オブジェクト情報の獲得
	D3DXVECTOR3 posObj = CObjectX::GetPosition();	// 位置
	D3DXVECTOR3 rotObj = CObjectX::GetRotation();	// 向き
	D3DXVECTOR3 vtxMin = CObjectX::GetVtxMin();		// 最小値
	D3DXVECTOR3 vtxMax = CObjectX::GetVtxMax();		// 最大値

	// 各頂点の座標計算
	D3DXVECTOR3 posLeftBack = INIT_D3DVEC3;		// モデル左奥の座標
	posLeftBack.x = posObj.x - sinf(-rotObj.y + m_fAngle) * (m_fLength + fRadius);
	posLeftBack.y = 0.0f;
	posLeftBack.z = posObj.z + cosf(-rotObj.y + m_fAngle) * (m_fLength + fRadius);
	D3DXVECTOR3 posRightBack = INIT_D3DVEC3;	// モデル右奥の座標
	posRightBack.x = posObj.x + sinf(rotObj.y + m_fAngle) * (m_fLength + fRadius);
	posRightBack.y = 0.0f;
	posRightBack.z = posObj.z + cosf(rotObj.y + m_fAngle) * (m_fLength + fRadius);
	D3DXVECTOR3 posLeftFront = INIT_D3DVEC3;	// モデル左手前の座標
	posLeftFront.x = posObj.x - sinf(rotObj.y + m_fAngle) * (m_fLength + fRadius);
	posLeftFront.y = 0.0f;
	posLeftFront.z = posObj.z - cosf(rotObj.y + m_fAngle) * (m_fLength + fRadius);
	D3DXVECTOR3 posRightFront = INIT_D3DVEC3;	// モデル右手前の座標
	posRightFront.x = posObj.x + sinf(-rotObj.y + m_fAngle) * (m_fLength + fRadius);
	posRightFront.y = 0.0f;
	posRightFront.z = posObj.z - cosf(-rotObj.y + m_fAngle) * (m_fLength + fRadius);

	D3DXVECTOR3 vecLineLBRB = posRightBack - posLeftBack;	// 左奥→右奥
	D3DXVec3Normalize(&vecLineLBRB, &vecLineLBRB);
	D3DXVECTOR3 vecLineRBRF = posRightFront - posRightBack;	// 右奥→右手前
	D3DXVec3Normalize(&vecLineRBRF, &vecLineRBRF);
	D3DXVECTOR3 vecLineRFLF = posLeftFront - posRightFront;	// 右手前→左手前
	D3DXVec3Normalize(&vecLineRFLF, &vecLineRFLF);
	D3DXVECTOR3 vecLineLFLB = posLeftBack - posLeftFront;	// 左手前→左奥
	D3DXVec3Normalize(&vecLineLFLB, &vecLineLFLB);

	// オブジェクトとの当たり判定[XZ]
	if ((CrossCollisionLeft(posLocal, posLeftBack, vecLineLBRB) == true)		// 左奥基準
		&& (CrossCollisionLeft(posLocal, posRightFront, vecLineRFLF) == true)	// 右手前基準
		&& (CrossCollisionLeft(posLocal, posLeftFront, vecLineLFLB) == true)	// 左手前基準
		&& (CrossCollisionLeft(posLocal, posRightBack, vecLineRBRF) == true))	// 右奥基準
	{ // オブジェクトにめり込んでいる
		return true;
	}

	return false;
}

//=============================
// 地面との判定処理処理
//=============================
void CBlock::CollisionField(CField* pField)
{
	D3DXVECTOR3 pos = CObjectX::GetPosition();

	float fHeight = pField->GetHeight(D3DXVECTOR3(pos.x, pos.y, pos.z));
	if (pos.y <= fHeight)
	{ // 地面に埋まった場合
		pos.y = fHeight;
		m_move.y = 0.0f;
		SetPosition(pos);
	}
}

//=============================
// ゲーム処理
//=============================
void CBlock::UpdateGame(void)
{
	if (m_nType == 0) return;	// 柵のみ早期リターン

	// オブジェクトの情報を取得	
	D3DXVECTOR3 pos = CObjectX::GetPosition();	// 現在地の取得
	D3DXVECTOR3 rot = CObjectX::GetRotation();

	// シーン情報を取得
	CScene* pScene = CManager::GetScene();
	CGame* pGame = static_cast<CGame*>(pScene);

	// デバッグ情報を取得
	CDebugProc* pDebug = CManager::GetDebugProc();

	// 重力
	m_move.y -= GRAVITY;

	// 位置更新
	pos += m_move;

	// 移動量の更新
	m_move.x += (0.0f - m_move.x) * CORRECTION_MOVE;
	m_move.y += (0.0f - m_move.y) * CORRECTION_MOVE;
	m_move.z += (0.0f - m_move.z) * CORRECTION_MOVE;

	SetPosition(pos);
	SetRotation(rot);

	// 地面との当たり判定処理
	CollisionField(pGame->GetField(0));
}

//=============================
// ブロック情報読み取り
//=============================
void CBlock::LoadInfo(void)
{
	// ファイル名のクリア
	for (int nBlock = 0; nBlock < MAX_BLOCK; nBlock++)
	{
		m_apModelName[nBlock][0] = {};
	}

	// ファイルを開く
	FILE* pFile = fopen(BLOCKINFO_PATH, "r");

	if (pFile == NULL)
	{ // 読み込みに失敗
		return;
	}

	// スクリプト読み込み処理インスタンス生成
	CLoadScripts* pLoadScripts = NULL;
	pLoadScripts = new CLoadScripts;

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]
	char aFileName[MAX_BLOCK][MAX_STRING];	// ローカルで文字列を格納[コピー]
	memset(&aFileName[0], 0, sizeof aFileName);
	D3DXVECTOR3 pos;	// 位置
	int nType = 0;	// 種類
	int nNumType = 0;

	do
	{ // 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

		if (strstr(&aStringCopy[0], BLOCKINFO) != NULL)
		{ // BLOCKINFOを読み込んだ
			while (1)
			{
				// 文字列のクリア
				memset(&aString[0], 0, sizeof aString);
				memset(&aStringCopy[0], 0, sizeof aStringCopy);

				// 読み込んだ行の文字列判定
				fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
				pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

				if (strstr(&aStringCopy[0], NUMBLOCK) != NULL)
				{ // NUMBLOCKを読み込んだ
					nNumType = pLoadScripts->LoadInt(&aStringCopy[0]);
				}
				if (strstr(&aStringCopy[0], MODELNAME) != NULL)
				{ // MODELNAMEを読み込んだ
					pLoadScripts->LoadFileName(&aStringCopy[0]);
					memcpy(&m_apModelName[nType][0], &aStringCopy[0], sizeof aStringCopy);
					nType++;
				}
				if (strstr(&aStringCopy[0], END_BLOCKINFO) != NULL && nType == nNumType)
				{ // END_BLOCKINFOを読み込んだ
					bLoadEnd = true;
					break;
				}
			}
		}
	} while (bLoadEnd == false);

	// ファイルを閉じる
	fclose(pFile);

	// スクリプト破棄
	SAFE_DELETE_SINGULAR(pLoadScripts);
}

//=============================
// ブロック配置情報読み取り
//=============================
void CBlock::LoadSetting(const char* pFileName)
{
	// スクリプト読み込み処理インスタンス生成
	CLoadScripts* pLoadScripts = NULL;
	pLoadScripts = new CLoadScripts;

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]
	char aFileName[MAX_STRING];	// ローカルで文字列を格納[コピー]
	memset(&aFileName[0], 0, sizeof aFileName);
	D3DXVECTOR3 pos;	// 位置
	D3DXVECTOR3 rot;	// 向き
	int nType = 0;	// 種類

	// ファイルを開く
	FILE* pFile = fopen(pFileName, "r");

	if (pFile == NULL)
	{ // 読み込みに失敗
		return;
	}

	do
	{ // 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

		if (strstr(&aStringCopy[0], BLOCKSET) != NULL)
		{ // BLOCKSETを読み込んだ
			while (1)
			{
				// 文字列のクリア
				memset(&aString[0], 0, sizeof aString);
				memset(&aStringCopy[0], 0, sizeof aStringCopy);

				// 読み込んだ行の文字列判定
				fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
				pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

				if (strstr(&aStringCopy[0], TYPE) != NULL)
				{ // TYPEを読み込んだ
					nType = pLoadScripts->LoadInt(&aStringCopy[0]);
				}
				if (strstr(&aStringCopy[0], POS) != NULL)
				{ // POSを読み込んだ
					pos = pLoadScripts->LoadVector3(&aStringCopy[0]);
				}
				if (strstr(&aStringCopy[0], ROT) != NULL)
				{ // ROTを読み込んだ
					rot = pLoadScripts->LoadVector3(&aStringCopy[0]);
					CBlock::Create(pos, rot, nType);
				}
				if (strstr(&aStringCopy[0], END_BLOCKSET) != NULL)
				{ // END_PLAYERSETを読み込んだ
					bLoadEnd = true;
					break;
				}
			}
		}
	} while (bLoadEnd == false);

	// ファイルを閉じる
	fclose(pFile);

	// スクリプト破棄
	SAFE_DELETE_SINGULAR(pLoadScripts);
}