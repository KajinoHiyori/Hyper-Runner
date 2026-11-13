//==========================================
// 
// アクション処理[action.h]
// Author : Kajino Hiyori _Create2026/11/13~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "action.h"
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
//アクション情報=============================
#include "action_box.h"

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
#define ACTIONSET				("ACTIONSET")		// ACTIONSET
#define END_ACTIONSET			("END_ACTIONSET")	// END_ACTIONSET
#define ACTIONINFO			("ACTIONINFO")		// ACTIONINFO
#define END_ACTIONINFO		("END_ACTIONINFO")	// END_ACTIONINFO
#define NUMACTION				("NUMACTION")			// NUMACTION
// ファイルパス
#define ACTIONINFO_PATH		("data\\SCRIPTS\\action_info.txt")	// アクション情報

//=============================
// 静的メンバ変数宣言
//=============================
int CAction::m_nNumAll = 0;

//=============================
// コンストラクタ
//=============================
CAction::CAction(CObject::PRIORITY priority): CObjectX(priority)
{
	m_posOld = INIT_D3DVEC3;	// モデルの1フレーム前の位置
	m_move = INIT_D3DVEC3;		// モデルの移動量
	m_bJump = false;			// ジャンプフラグ
	m_state = STATE_NONE;		// ステート管理
	m_nStateCounter = 0;		// ステート管理カウンター
	m_fSize = 0.0f;				// サイズ
	m_posOffset = INIT_D3DVEC3;	// オフセット
	m_nNumAll++;
}

//=============================
// デストラクタ
//=============================
CAction::~CAction()
{

}

//=============================
// アクション生成処理
//=============================
CAction* CAction::Create(D3DXVECTOR3 pos, int Type)
{
	CAction* pAction = NULL;

	if (pAction == NULL)
	{
		pAction = new CAction;	// インスタンス生成
		
		if (pAction != NULL)
		{ // インスタンス生成に成功
			// アクションの初期化処理
			pAction->Init();

			return pAction;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CAction::Init(D3DXVECTOR3 pos, TYPE type, const char* pFileName)
{
	// オブジェクトにタイプを指定
	CObjectX::Init(pos, INIT_D3DVEC3, pFileName);
	CObjectX::SetType(CObject::TYPE_ACTION);
	
	m_posOld = pos;	// モデルの1フレーム前の位置

	// 状態管理
	m_state = STATE_NONE;
	m_stateOld = STATE_NONE;
	m_move = INIT_D3DVEC3;

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CAction::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// Xオブジェクトの終了処理
	CObjectX::Uninit();
}

//=============================
// 更新処理
//=============================
void CAction::Update(void)
{
	// 過去の情報を保存
	m_posOld = CObjectX::GetPosition();
	m_stateOld = m_state;

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
void CAction::Draw(void)
{
	// Xオブジェクトの描画処理
	CObjectX::Draw();
}

//=============================
// アクション情報読み取り
//=============================
void CAction::LoadInfo(void)
{

}

//=============================
// アクション配置情報読み取り
//=============================
void CAction::LoadSetting(const char* pFileName)
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

		if (strstr(&aStringCopy[0], ACTIONSET) != NULL)
		{ // ACTIONSETを読み込んだ
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
					CAction::Setter(pos, nType);
				}
				if (strstr(&aStringCopy[0], END_ACTIONSET) != NULL)
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
void CAction::CollisionPlayer(CPlayer* pPlayer)
{
	if (pPlayer == NULL) return;
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();
	D3DXVECTOR3 posAction = CObjectX::GetPosition();
	float fDistance = sqrtf(((posAction.x - posPlayer.x) * (posAction.x - posPlayer.x)) + ((posAction.z - posPlayer.z) * (posAction.z - posPlayer.z)));
	float fHeight = posPlayer.y - posAction.y;

	if ((fDistance > m_fSize + pPlayer->GetRadius() || fHeight > 0.0f))return;	// 取得ナシ
	m_state = STATE_PLAYER;
}

//=============================
// ゲーム処理
//=============================
void CAction::UpdateGame(void)
{
	// フィールド情報を取得
	CGame* pGame = static_cast<CGame*>(CManager::GetScene());

	//if (static_cast<CGame*>(CManager::GetScene())->GetEnablePause() == true) return;

	CField* pField = pGame->GetField(CField::GetIndex(CObjectX::GetPosition()));
	CPlayer* pPlayer = pGame->GetPlayer();

	// 移動処理
	Move();

	// 地面との判定処理
	if (CField::GetIndex(CObjectX::GetPosition()) != -1)
	{
		CollisionField(pField);
	}

	// ブロックとの判定処理
	CollisionBlock();

	// ステート管理処理
	StateManager(pPlayer);
}

//=============================
// ステート管理処理
//=============================
void CAction::StateManager(CPlayer* pPlayer)
{
	switch (m_state)
	{
	case STATE_NONE:	// 待機
		UpdateStateNone(pPlayer);
		break;

	case STATE_PLAYER:	// プレイヤーが保持
		UpdateStatePlayer(pPlayer);
		break;
	}
}

//=============================
// 通常処理
//=============================
void CAction::UpdateStateNone(CPlayer* pPlayer)
{
	// プレイヤーとの判定処理
	CollisionPlayer(pPlayer);
}

//=============================
// プレイヤーが保持中の処理
//=============================
void CAction::UpdateStatePlayer(CPlayer* pPlayer)
{
	// 頂点データの算出
	D3DXVECTOR3 pos = INIT_D3DVEC3;
	D3DXMATRIX mtxRightarm = pPlayer->GetRightarmMtx();
	D3DXVec3TransformCoord(&pos, &m_posOffset, &mtxRightarm);
	CObjectX::SetPosition(pos);

	if (CManager::GetInputKeyboard()->GetTrigger(DIK_SPACE) == true
		|| CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_A) == true)
	{
		m_state = STATE_ACT;
	}
}

//=============================
// フィールドとの判定
//=============================
void CAction::CollisionField(CField* pField)
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
			if (m_state == STATE_ACT)
			{
				m_state = STATE_NONE;
			}
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
void CAction::CollisionBlock(void)
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
			if (pObject->GetType() != CObject::TYPE_ACTION)
			{
				pObject = pObjectNext;	// NULL判定を行う
				continue;
			}
			// アクションの場合
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
void CAction::Move(void)
{
	D3DXVECTOR3 pos = CObjectX::GetPosition();

	// 重力	
	m_move.y -= GRAVITY;

	// 位置の更新
	pos += m_move;

	// 位置更新
	CObjectX::SetPosition(pos);

	// 移動量の更新
	m_move.x += (0.0f - m_move.x) * CORRECTION_MOVE;
	m_move.y += (0.0f - m_move.y) * CORRECTION_MOVE;
	m_move.z += (0.0f - m_move.z) * CORRECTION_MOVE;

	CManager::GetDebugProc()->Print("Boxmove %f %f %f\n", m_move.x, m_move.y, m_move.z);
}

//=============================
// 他のオブジェクトとの当たり判定
//=============================
bool CAction::CollisionHitter(void)
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
			if (pObject->GetType() != CObject::TYPE_BLOCK)
			{
				pObject = pObjectNext;	// NULL判定を行う
				continue;
			}
			// アクションの場合
			CBlock* pBlock = static_cast<CBlock*>(pObject);

			// 当たり判定処理を実行
			D3DXVECTOR3 posBlock = pBlock->GetPosition();
			D3DXVECTOR3 posAction = CObjectX::GetPosition();
			float fDistance = sqrtf(((posAction.x - posBlock.x) * (posAction.x - posBlock.x)) + ((posAction.z - posBlock.z) * (posAction.z - posBlock.z)));
			float fDistanceHeight = sqrtf((posAction.y - posBlock.y) * (posAction.y - posBlock.y));

			if ((fDistance < m_fSize + pBlock->GetSize() && fDistanceHeight < m_fSize + pBlock->GetVtxMax().y))return true;	// 取得
			pObject = pObjectNext;	// NULL判定を行う
		}
	}

	CObjectX::SetPosition(pos);

	return false;
}

#define BOX_SIZE	(20.0f)	// 箱の取得サイズ
#define BOX_OFFSET	(D3DXVECTOR3(12.0f, -20.0f, 0.0f))	// 箱のオフセット
//=============================
// アクション生成処理
//=============================
void CAction::Setter(D3DXVECTOR3 pos, int type)
{
	CAction* pAct = NULL;
	switch (type)
	{
	case TYPE_BOX:	// 箱の用意
		pAct = CBox::Create(pos);
		pAct->m_fSize = BOX_SIZE;
		pAct->m_posOffset = BOX_OFFSET;
		break;
	}
}