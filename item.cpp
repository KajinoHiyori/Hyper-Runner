//==========================================
// 
// アイテム処理[item.h]
// Author : Kajino Hiyori _Create2026/09/28~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "item.h"
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
#include "player.h"
//アイテム情報=============================
#include "item_speedup.h"
#include "item_speeddown.h"

//=============================
// マクロ定義
//=============================
#define SIZE				(10.0f)	// モデルの基本サイズ
#define FIELD_NORMAL		(D3DXVECTOR3(0.0f, 1.0f, 0.0f))	// フィールド情報
#define CORRECTION_ROT		(0.1f)				// 回転の減衰係数
#define CORRECTION_MOVE		(0.1f)				// 移動の減衰係数
#define NOT_JUMP			(1.35f)	// ジャンプの反転を停止
#define TRANSPOTATION		(10.0f)	// 運搬時の上昇値
#define APPEAR_JUMP			(D3DXVECTOR3(0.0f, 30.0f, 0.0f))	// 出現時の飛び跳ね処理
#define COLOR_SWITCH		(15)	// 点滅間隔
#define COLOR_SWITCH1		(10)	// 点滅間隔
#define APPEAR_TIME			(120)	// 出現時間
#define APPEAR_COLOR		(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f))	// 出現時の点滅色
#define BUMP_DELETE			(3.0f)	// 一定高度以上で削除
#define MARGINE				(0.1f)	// 余白
#define STARCOLOR_CHANGE	(0.05f)	// 色変更
#define DEFCOLOR			(2)			// デフォルト色の設定
#define FRAME				(60)	// 移動フレーム
#define ITEMSET				("ITEMSET")		// ITEMSET
#define END_ITEMSET			("END_ITEMSET")	// END_ITEMSET
#define ITEMINFO			("ITEMINFO")		// ITEMINFO
#define END_ITEMINFO		("END_ITEMINFO")	// END_ITEMINFO
#define NUMITEM				("NUMITEM")			// NUMITEM
// ファイルパス
#define ITEMINFO_PATH		("data\\SCRIPTS\\item_info.txt")	// アイテム情報

//=============================
// 静的メンバ変数宣言
//=============================
int CItem::m_nNumAll = 0;

//=============================
// コンストラクタ
//=============================
CItem::CItem(CObject::PRIORITY priority): CObjectX(priority)
{
	m_posOld = INIT_D3DVEC3;	// モデルの1フレーム前の位置
	m_move = INIT_D3DVEC3;		// モデルの移動量
	m_bJump = false;			// ジャンプフラグ
	m_state = STATE_NONE;		// ステート管理
	m_nStateCounter = 0;		// ステート管理カウンター
	m_fSize = 0.0f;				// サイズ
	m_nNumAll++;
}

//=============================
// デストラクタ
//=============================
CItem::~CItem()
{

}

//=============================
// アイテム生成処理
//=============================
CItem* CItem::Create(D3DXVECTOR3 pos, int Type)
{
	CItem* pItem = NULL;

	if (pItem == NULL)
	{
		pItem = new CItem;	// インスタンス生成
		
		if (pItem != NULL)
		{ // インスタンス生成に成功
			// アイテムの初期化処理
			pItem->Init();

			return pItem;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CItem::Init(D3DXVECTOR3 pos, TYPE type, const char* pFileName)
{
	// オブジェクトにタイプを指定
	CObjectX::Init(pos, INIT_D3DVEC3, pFileName);
	CObjectX::SetType(CObject::TYPE_ITEM);
	
	m_posOld = pos;	// モデルの1フレーム前の位置

	// 状態管理
	m_state = STATE_NONE;
	m_move = APPEAR_JUMP;

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CItem::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// Xオブジェクトの終了処理
	CObjectX::Uninit();
}

//=============================
// 更新処理
//=============================
void CItem::Update(void)
{
	// 過去の情報を保存
	m_posOld = CObjectX::GetPosition();
	CField* pField = NULL;
	CPlayer* pPlayer = NULL;
	
	switch (CManager::GetMode())
	{
	case CScene::MODE_TITLE:	// タイトル
	
		break;
	

	case CScene::MODE_GAME:	// ゲーム
		// ゲーム処理
		UpdateGame();
		break;

	case CScene::MODE_RESULT:	// リザルト
		
		break;
	}
}

//=============================
// 描画処理
//=============================
void CItem::Draw(void)
{
	// Xオブジェクトの描画処理
	CObjectX::Draw();
}

//=============================
// アイテム情報読み取り
//=============================
void CItem::LoadInfo(void)
{

}

//=============================
// アイテム配置情報読み取り
//=============================
void CItem::LoadSetting(const char* pFileName)
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

		if (strstr(&aStringCopy[0], ITEMSET) != NULL)
		{ // ITEMSETを読み込んだ
			while (1)
			{
				// 文字列のクリア
				memset(&aString[0], 0, sizeof aString);
				memset(&aStringCopy[0], 0, sizeof aStringCopy);

				// 読み込んだ行の文字列判定
				fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
				pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

				if (strstr(&aStringCopy[0], "TYPE") != NULL)
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
					CItem::Setter(pos, nType);
				}
				if (strstr(&aStringCopy[0], END_ITEMSET) != NULL)
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

//=============================
// プレイヤーとの判定処理
//=============================
CPlayer* CItem::CollisionPlayer(void)
{
	CPlayer* pPlayer = NULL;
	bool bFind = false;

	for (int nCntPriority = 0; nCntPriority < MAX_PRIORITYS; nCntPriority++)
	{
		CObject* pObject = GetTop(nCntPriority);	// 先頭アドレスを取得

		while (pObject != NULL)
		{ // 次のオブジェクトがある限りループ
			CObject* pObjectNext = pObject->GetNext();	// 次のオブジェクトへのポインタを設定
			bool bOneCollision = false;

			// オブジェクトとの当たり判定
			if (pObject->GetType() != CObject::TYPE_PLAYER)
			{
				pObject = pObjectNext;	// NULL判定を行う
				continue;
			}
			// アイテムの場合
			pPlayer = static_cast<CPlayer*>(pObject);
			bFind = true;
			break;
		}
		if (bFind == true) break;
	}
	if (pPlayer == NULL) return NULL;

	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();
	D3DXVECTOR3 posItem = CObjectX::GetPosition();
	float fDistance = sqrtf(((posItem.x - posPlayer.x) * (posItem.x - posPlayer.x)) + ((posItem.z - posPlayer.z) * (posItem.z - posPlayer.z)));
	float fHeight = posPlayer.y - posItem.y;

	if (fDistance > m_fSize + pPlayer->GetRadius() || fHeight > 0.0f)return NULL;	// 取得ナシ

	return pPlayer;
}

//=============================
// ゲーム処理
//=============================
void CItem::UpdateGame(void)
{
	// フィールド情報を取得
	CGame* pGame = static_cast<CGame*>(CManager::GetScene());

	//if (static_cast<CGame*>(CManager::GetScene())->GetEnablePause() == true) return;

	CField* pField = pGame->GetField(CField::GetIndex(CObjectX::GetPosition()));

	// 移動処理
	Move();

	// 地面との判定処理
	if (CField::GetIndex(CObjectX::GetPosition()) != -1)
	{
		CollisionField(pField);
	}

	// ブロックとの判定処理
	CollisionBlock();
}

//=============================
// フィールドとの判定
//=============================
void CItem::CollisionField(CField* pField)
{
	D3DXVECTOR3 pos = CObjectX::GetPosition();
	float fHeight = pField->GetHeight(D3DXVECTOR3(pos.x, pos.y + CObjectX::GetVtxMin().y * CObjectX::GetScale().y, pos.z));
	
	if (pos.y + CObjectX::GetVtxMin().y * CObjectX::GetScale().y <= fHeight)
	{ // 地面に埋まった場合
		pos.y = fHeight - CObjectX::GetVtxMin().y * CObjectX::GetScale().y;

		if (m_move.y >= -NOT_JUMP && m_move.y <= NOT_JUMP)
		{
			m_bJump = false;
			m_move.y = 0.0f;
		}
		else
		{
			m_move.y *= -1.0f;	// 地面に跳ね返る
		}
	}

	// フィールド上で行動範囲を制限[X]
	if (pField->GetPosition().x - pField->GetWidth() >= pos.x - m_fSize)
	{
		pos.x = pField->GetPosition().x - pField->GetWidth() + m_fSize;
	}
	else if (pField->GetPosition().x + pField->GetWidth() <= pos.x + m_fSize)
	{
		pos.x = pField->GetPosition().x + pField->GetWidth() - m_fSize;
	}
	// フィールド上で行動範囲を制限[Z]
	if (pField->GetPosition().z - pField->GetDepth() >= pos.z - m_fSize)
	{
		pos.z = pField->GetPosition().z - pField->GetDepth() + m_fSize;
	}
	else if (pField->GetPosition().z + pField->GetDepth() <= pos.z + m_fSize)
	{
		pos.z = pField->GetPosition().z + pField->GetDepth() - m_fSize;
	}

	// 位置更新
	CObjectX::SetPosition(pos);
}

//=============================
// ブロックとの判定
//=============================
void CItem::CollisionBlock(void)
{
	D3DXVECTOR3 pos = CObjectX::GetPosition();
	for (int nCntPriority = 0; nCntPriority < MAX_PRIORITYS; nCntPriority++)
	{
		CObject* pObject = GetTop(nCntPriority);	// 先頭アドレスを取得

		while (pObject != NULL)
		{ // 次のオブジェクトがある限りループ
			CObject* pObjectNext = pObject->GetNext();	// 次のオブジェクトへのポインタを設定
			bool bOneCollision = false;

			// オブジェクトとの当たり判定
			if (pObject->GetType() != CObject::TYPE_ITEM)
			{
				pObject = pObjectNext;	// NULL判定を行う
				continue;
			}
			// アイテムの場合
			CBlock* pBlock = static_cast<CBlock*>(pObject);

			// 当たり判定処理を実行
			pBlock->Collision(&pos, m_posOld, m_fSize, MARGINE);

			pObject = pObjectNext;	// NULL判定を行う
		}
	}

	CObjectX::SetPosition(pos);
}

//=============================
// 移動処理
//=============================
void CItem::Move(void)
{
	D3DXVECTOR3 pos = CObjectX::GetPosition();

	// カメラ取得
	CCamera* pCamera = CManager::GetCamera();
	D3DXVECTOR3 rotCamera = pCamera->GetRotation();

	// 重力	
	m_move.y -= GRAVITY;
	// 移動量の更新
	m_move.y += (0.0f - m_move.y) * CORRECTION_MOVE;
	// 位置の更新
	pos += m_move;

	// 位置更新
	CObjectX::SetPosition(pos);
}

//=============================
// アイテム生成処理
//=============================
void CItem::Setter(D3DXVECTOR3 pos, int type)
{
	switch (type)
	{
	case TYPE_SPEEDUP:	// 加速アイテム
		CSpeedUP::Create(pos);
		break;

	case TYPE_SPEEDDOWN:	// 減速アイテム
		CSpeedDown::Create(pos);
		break;
	}
}