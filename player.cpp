//========================================================
// 
// Player処理[leftplayer.cpp]
// Author : Kajino Hiyori _Create2026/06/02
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "player.h"
#include "camera.h"
#include "input.h"
#include "color.h"
#include "debugproc.h"
#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "model.h"
#include "shadow.h"
#include "meshfield.h"
#include "scene.h"
#include "title.h"
#include "game.h"
#include "result.h"
#include "ranking.h"
#include "particle.h"
#include "sound.h"
#include "field.h"

//=============================
// マクロ定義
//=============================
#define FILE_NAME			("data\\SCRIPTS\\Player.txt")	// ファイル名
#define PRESSANIM_NAME		("data\\TEXTURE\\pressAnim_L.png")	// 有効攻撃表示テクスチャ名
#define SPEED				(1.0f)					// 移動速度
#define CORRECTION_MOVE		(0.1f)				// 移動の減衰係数

//=============================
// コンストラクタ
//=============================
CPlayer::CPlayer(CObject::PRIORITY priority) : CObject(priority)
{
	// 値のクリア
	m_pos = INIT_D3DVEC3;		// モデルの位置
	m_posOld = INIT_D3DVEC3;	// モデルの1フレーム前の位置
	m_move = INIT_D3DVEC3;		// モデルの移動量
	m_rot = INIT_D3DVEC3;		// モデルの向き
	m_rotDest = INIT_D3DVEC3;	// モデルの目的の向き
	memset(&m_apModel[0], NULL, sizeof m_apModel);	// モデルのポインタ
	m_nNumModel = 0;			// モデル総数
	m_pMotion = NULL;			// モーション情報
	m_pShadow = NULL;			// 影情報
	for (int nCntModel = 0; nCntModel < MODEL_MAX; nCntModel++)
	{
		memset(&m_aFileName[nCntModel][0], 0, sizeof m_aFileName[nCntModel]);
	}
	m_fSpeed = 0.0f;	// 速度
	m_nLine = LINE_CENTER;		// 移動しているライン
	m_bJump = false;	// ジャンプフラグ
}

//=============================
// デストラクタ
//=============================
CPlayer::~CPlayer()
{

}

//=============================
// Player生成処理
//=============================
CPlayer* CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CPlayer* pPlayer = NULL;	// オブジェクトへのポインタを用意
	if (pPlayer == NULL)
	{ // NULLが設定できている場合
		pPlayer = new CPlayer;
		if (pPlayer != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pPlayer->Init(pos, rot);

			// 種類設定
			pPlayer->SetType(TYPE_PLAYER);

			return pPlayer;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CPlayer::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// 初期化
	m_pos = pos;
	m_posOld = pos;
	m_fSpeed = SPEED;
	m_move = D3DXVECTOR3(0.0f, 0.0f, m_fSpeed);
	m_rot = rot;
	m_rotDest = rot;

	// モデル読み込み
	LoadScripts("data\\SCRIPTS\\player_setting.txt");

	if (m_pMotion != NULL)
	{
		m_pMotion->Set(MOTIONTYPE_MOVE, false, 0);
	}

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CPlayer::Uninit(void)
{
	// モデルの終了処理
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		SAFE_UNINIT(m_apModel[nCntModel]);
	}

	// モーションの破棄
	if (m_pMotion != NULL)
	{ // モーションが用意されていた場合
		SAFE_DELETE_SINGULAR(m_pMotion);
	}

	// 影の破棄
	m_pShadow = NULL;
}

//=============================
// 更新処理
//=============================
void CPlayer::Update(void)
{
	// シーン情報
	CScene::MODE mode = CManager::GetMode();

	// メッシュフィールド情報
	CMeshField* pMeshField = NULL;

	// 過去の位置を保存
	m_posOld = m_pos;

	// シーンごとに処理を分割
	switch (mode)
	{
	case CScene::MODE_TITLE:	// タイトル
		// タイトル処理
		UpdateTitle();
		break;

	case CScene::MODE_GAME:		// ゲーム
		// ゲーム処理
		UpdateGame();
		//pMeshField = static_cast<CGame*>(CManager::GetScene())->GetMeshField();
		break;

	case CScene::MODE_RESULT:	// リザルト
		// リザルト処理
		UpdateResult();
		break;

	case CScene::MODE_RANKING:	// ランキング
		break;
	}

	// モーションの更新
	if (m_pMotion != NULL)
	{
		m_pMotion->Update();
	}
	// デバック表示=================================================================================
	CDebugProc* pDebugProc = CManager::GetDebugProc();
	pDebugProc->Print("Playerの位置 : (%.2f, %.2f, %.2f)\n", m_pos.x, m_pos.y, m_pos.z);
	pDebugProc->Print("Player向き : (%.2f, %.2f, %.2f)\n", m_rot.x, m_rot.y, m_rot.z);
	pDebugProc->Print("Player移動量 : (%.2f, %.2f, %.2f)\n", m_move.x, m_move.y, m_move.z);
}

//=============================
// 描画処理
//=============================
void CPlayer::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxGauge;	// ゲージ用マトリックス

	// ワールドマトリックスの初期化(デフォルトの値にする)
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ゲージ用のマトリックスを格納
	mtxGauge = m_mtxWorld;

	// モデルごとに描画
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (m_apModel[nCntModel] == NULL)continue;
		 // 使用されていた場合
		// モデルの描画
		m_apModel[nCntModel]->Draw();
		
	}

#ifndef ENABLE_MULTITARGETRENDERING
	CCamera* pCamera = CManager::GetCamera();
	LPDIRECT3DSURFACE9 pRenderDef, pZBuffDef;
	D3DVIEWPORT9 viewportDef;
	D3DXMATRIX mtxView, mtxProjectionDef;

	// 現在の状態を保存
	pDevice->GetRenderTarget(0, &pRenderDef);	// レンダリングターゲット
	pDevice->GetDepthStencilSurface(&pZBuffDef);	// Zバッファ
	pDevice->GetViewport(&viewportDef);				// ビューポート
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);	// ビューマトリックス
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjectionDef);	// プロジェクションマトリックス

	// レンダリングターゲットの変更
	pRenderer->ChangeTarget(pCamera->GetPosV(), pCamera->GetPosR(), pCamera->GetVecU());
	
	// ワールドマトリックスの初期化(デフォルトの値にする)
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// モデルごとに描画
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (m_apModel[nCntModel] != NULL)
		{ // 使用されていた場合
			// モデルの描画
			m_apModel[nCntModel]->Draw();
		}
	}

	// レンダリング状態を元に戻す
	pDevice->SetRenderTarget(0, pRenderDef);	// レンダリングターゲット
	pDevice->SetDepthStencilSurface(pZBuffDef);	// Zバッファ
	pDevice->SetViewport(&viewportDef);				// ビューポート
	pDevice->SetTransform(D3DTS_VIEW, &mtxView);	// ビューマトリックス
	pDevice->SetTransform(D3DTS_PROJECTION, &mtxProjectionDef);	// プロジェクションマトリックス

#endif
}

//=============================
// タイトル処理
//=============================
void CPlayer::UpdateTitle(void)
{

}

//=============================
// チュートリアル処理
//=============================
void CPlayer::UpdateTutorial(void)
{
	
}

//=============================
// ゲーム処理
//=============================
void CPlayer::UpdateGame(void)
{
	// 移動処理
	Move();

	// 地面との判定
	CollisionField(static_cast<CGame*>(CManager::GetScene())->GetField(m_nLine));
}

//=============================
// リザルト処理
//=============================
void CPlayer::UpdateResult(void)
{
	
}

//=============================
// 移動処理
//=============================
void CPlayer::Move(void)
{
	// 入力情報取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();
	int nChangeDir = 0;	// 移動方向設定

	if ((pKeyboard->GetTrigger(DIK_D) == true
		|| pKeyboard->GetTrigger(DIK_RIGHTARROW) == true
		|| pJoypad->GetTrigger(CInputJoypad::JOYKEY_RIGHT) == true
		|| pJoypad->GetLeftStickTrigger(CInputJoypad::JOYKEY_LSTICK_RIGHT) == true)
		&& m_bJump == false)
	{ // 右のラインに移動
		switch (m_nLine)
		{
		case LINE_RIGHT:	// 右端
			nChangeDir = 0;
			break;

		default:
			m_nLine++;
			nChangeDir = 1;
			break;
		}
		// 列切り替え処理
		ChangeLine(nChangeDir);
	}
	else if ((pKeyboard->GetTrigger(DIK_A) == true
		|| pKeyboard->GetTrigger(DIK_LEFTARROW) == true
		|| pJoypad->GetTrigger(CInputJoypad::JOYKEY_LEFT) == true
		|| pJoypad->GetLeftStickTrigger(CInputJoypad::JOYKEY_LSTICK_LEFT) == true)
		&& m_bJump == false)
	{ // 左のラインに移動
		switch (m_nLine)
		{
		case LINE_LEFT:	// 右端
			nChangeDir = 0;
			break;

		default:
			m_nLine--;
			nChangeDir = -1;
			break;
		}

		// 列切り替え処理
		ChangeLine(nChangeDir);
	}

	// 重力
	m_move.y -= GRAVITY;

	// 移動量更新
	m_pos += m_move;

	// 移動量の更新
	m_move.x += (0.0f - m_move.x) * CORRECTION_MOVE;
	m_move.y += (0.0f - m_move.y) * CORRECTION_MOVE;
}

#define JUMP		(40.0f)	// ジャンプ
#define CHANGE_X	(40.0f)	// X方向切り替え

//=============================
// 列切り替え処理
//=============================
void CPlayer::ChangeLine(int nChangeDir)
{
	m_move.x = CHANGE_X * nChangeDir;
	m_move.y = JUMP;
	m_bJump = true;
}

//=============================
// フィールド判定処理
//=============================
void CPlayer::CollisionField(CField* pField)
{
	if (m_pos.y > pField->GetHeight(m_pos))return;
	// 設置処理
	m_move.x = 0.0f;
	m_move.y = 0.0f;
	m_bJump = false;
	m_pos.y = pField->GetHeight(m_pos);
}

//=============================
// 情報読み取り処理
//=============================
HRESULT CPlayer::LoadScripts(const char* pFilePath)
{
	// キャラクター情報読み取り
	if (LoadCharactor(pFilePath) == false)
	{
		return E_FAIL;
	}

	return S_OK;
}

//=============================
// キャラクター情報読み取り処理
//=============================
bool CPlayer::LoadCharactor(const char* pFilePath)
{
	// モデル名取得
	if (LoadModelFile(pFilePath) == false) return false;

	// 設定
	if (LoadModelSetting(pFilePath) == false) return false;

	// モーション設定
	//if (LoadMotion(pFilePath) == false) return false;
		 
	return true;
}

//=============================
// モデル名取得
//=============================
bool CPlayer::LoadModelFile(const char* pFilePath)
{
	FILE* pFile = fopen(pFilePath, "r");

	if (pFile == NULL)
	{ // 読み込みに失敗
		return false;
	}

	// 外部ファイル読み取り処理を生成
	CLoadScripts* pLoadScripts = new CLoadScripts();

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]
	int nCntModel = 0;	// モデルの読み込み数を格納

	// モデルファイル名取得
	do
	{ // 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

		if (strstr(&aStringCopy[0], NUM_MODEL) != NULL)
		{ // NUM_MODELを読み込んだ
			m_nNumModel = pLoadScripts->LoadInt(&aStringCopy[0]);
			if (m_nNumModel != MODEL_MAX)
			{
				// ファイルを閉じる
				fclose(pFile);	
				// 外部ファイル読み取り処理を破棄
				SAFE_DELETE_SINGULAR(pLoadScripts); 
				return false;	// スクリプトと内部指定値が異なる場合早期リターン
			}
		}

		if (strstr(&aStringCopy[0], MODEL_FILENAME) != NULL)
		{ // MODEL_FILENAMEを読み込んだ
			pLoadScripts->LoadFileName(&aStringCopy[0]);
			memcpy(&m_aFileName[nCntModel][0], &aStringCopy[0], sizeof aStringCopy);
			nCntModel++;
			if (nCntModel == m_nNumModel)
			{
				bLoadEnd = true;
			}
		}
	} while (bLoadEnd == false);

	// ファイルを閉じる
	fclose(pFile);

	// 外部ファイル読み取り処理を破棄
	SAFE_DELETE_SINGULAR(pLoadScripts);

	return true;
}

//=============================
// モデル設置情報
//=============================
bool CPlayer::LoadModelSetting(const char* pFilePath)
{
	FILE* pFile = fopen(pFilePath, "r");

	if (pFile == NULL)
	{ // 読み込みに失敗
		return false;
	}

	// 外部ファイル読み取り処理を生成
	CLoadScripts* pLoadScripts = new CLoadScripts();

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]
	int nIdx = 0;
	int aParentIdx[MODEL_MAX] = {};	// 親を指定

	do
	{ // 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

		if (strstr(&aStringCopy[0], CHARACTERSET) != NULL)
		{ // CHARACTERSETを読み込んだ
			while (1)
			{
				// 文字列のクリア
				memset(&aString[0], 0, sizeof aString);
				memset(&aStringCopy[0], 0, sizeof aStringCopy);

				// 読み込んだ行の文字列判定
				fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
				pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

				if (strstr(&aStringCopy[0], NUM_PARTS) != NULL)
				{ // NUM_PARTSを読み込んだ
					int nNumParts = pLoadScripts->LoadInt(&aStringCopy[0]);
					if (m_nNumModel != nNumParts)
					{ // モデル数とパーツ数が合わない場合早期リターン
						// ファイルを閉じる
						fclose(pFile);
						// 外部ファイル読み取り処理を破棄
						SAFE_DELETE_SINGULAR(pLoadScripts);
						return false;
					}
				}

				if (strstr(&aStringCopy[0], PARTSSET) != NULL)
				{ // PARTSSETを読み込んだ
					int nParent = -1;
					D3DXVECTOR3 pos = INIT_D3DVEC3;
					D3DXVECTOR3 rot = INIT_D3DVEC3;
					
					while (1)
					{
						// 文字列のクリア
						memset(&aString[0], 0, sizeof aString);
						memset(&aStringCopy[0], 0, sizeof aStringCopy);

						// 読み込んだ行の文字列判定
						fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
						pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

						if (strstr(&aStringCopy[0], INDEX) != NULL)
						{ // INDEXを読み込んだ
							nIdx = pLoadScripts->LoadInt(&aStringCopy[0]);
						}
						if (strstr(&aStringCopy[0], PARENT) != NULL)
						{ // PARENTを読み込んだ
							aParentIdx[nIdx] = pLoadScripts->LoadInt(&aStringCopy[0]);
						}
						if (strstr(&aStringCopy[0], POS) != NULL)
						{ // POSを読み込んだ
							pos = pLoadScripts->LoadVector3(&aStringCopy[0]);
						}
						if (strstr(&aStringCopy[0], ROT) != NULL)
						{ // ROTを読み込んだ
							rot = pLoadScripts->LoadVector3(&aStringCopy[0]);
						}
						if (strstr(&aStringCopy[0], END_PARTSSET) != NULL)
						{ // END_PARTSSETを読み込んだ
							m_apModel[nIdx] = CModel::Create(pos, rot, &m_aFileName[nIdx][0]);
							break;
						}
					}
				}
				
				if (strstr(&aStringCopy[0], END_CHARACTERSET) != NULL)
				{ // END_CHARACTERSETを読み込んだ
					bLoadEnd = true;
					break;
				}
			}
		}
	} while (bLoadEnd == false);

	// ファイルを閉じる
	fclose(pFile);

	// 外部ファイル読み取り処理を破棄
	SAFE_DELETE_SINGULAR(pLoadScripts);

	// モデルの親を指定
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (aParentIdx[nCntModel] == -1)
		{
			m_apModel[nCntModel]->SetParent(NULL);
			continue;
		}
		m_apModel[nCntModel]->SetParent(m_apModel[aParentIdx[nCntModel]]);
	}

	return true;
}

//=============================
// モデル設置情報
//=============================
bool CPlayer::LoadMotion(const char* pFilePath)
{
	FILE* pFile = fopen(pFilePath, "r");

	if (pFile == NULL)
	{ // 読み込みに失敗
		return false;
	}

	// 外部ファイル読み取り処理を生成
	CLoadScripts* pLoadScripts = new CLoadScripts();

	m_pMotion = new CMotion();
	// モーションにモデルの情報を設定
	m_pMotion->SetModel(&m_apModel[0], m_nNumModel);

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]
	int nIdx = 0;
	int aParentIdx[MODEL_MAX] = {};	// 親を指定
	CMotion::INFO motionInfo[MOTIONTYPE_MAX] = {};	// モーション情報
	int nCntMotion = 0;
	int nCntKey = 0;
	do
	{ // 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り
		if (strstr(&aStringCopy[0], MOTIONSET) != NULL)
		{ // MOTIONSETを読み込んだ
			while (1)
			{
				// 文字列のクリア
				memset(&aString[0], 0, sizeof aString);
				memset(&aStringCopy[0], 0, sizeof aStringCopy);

				// 読み込んだ行の文字列判定
				fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
				pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

				if (strstr(&aStringCopy[0], END_MOTIONSET) != NULL)
				{ // END_MOTIONSETを読み込んだ
					m_pMotion->SetInfo(motionInfo[nCntMotion]);
					nCntMotion++;
					if (nCntMotion == MOTIONTYPE_MAX)
					{
						bLoadEnd = true;
					}
					break;
				}
				else if (strstr(&aStringCopy[0], LOOP) != NULL)
				{ // LOOPを読み込んだ
					int nLoop = pLoadScripts->LoadInt(&aStringCopy[0]);
					switch (nLoop)
					{
					case 0:	// ループなし
						motionInfo[nCntMotion].bLoop = false;
						break;

					case 1:	// ループあり
						motionInfo[nCntMotion].bLoop = true;
						break;
					}
				}
				else if (strstr(&aStringCopy[0], NUM_KEY) != NULL)
				{ // NUM_KEYを読み込んだ
					motionInfo[nCntMotion].nNumKey = pLoadScripts->LoadInt(&aStringCopy[0]);
					nCntKey = 0;
				}
				else if (strstr(&aStringCopy[0], KEYSET) != NULL)
				{ // KEYSETを読み込んだ
					int nCntModel = 0;
					while (1)
					{
						// 文字列のクリア
						memset(&aString[0], 0, sizeof aString);
						memset(&aStringCopy[0], 0, sizeof aStringCopy);

						// 読み込んだ行の文字列判定
						fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
						pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

						if (strstr(&aStringCopy[0], END_KEYSET) != NULL)
						{ // END_KEYSETを読み込んだ
							nCntKey++;	// キー数の加算
							break;
						}
						else if (strstr(&aStringCopy[0], FRAME) != NULL)
						{ // FRAMEを読み込んだ
							motionInfo[nCntMotion].aKeyInfo[nCntKey].nFrame = pLoadScripts->LoadInt(&aStringCopy[0]);
						}
						else if (strstr(&aStringCopy[0], KEY) != NULL)
						{ // KEYを読み込んだ
							while (1)
							{
								// 文字列のクリア
								memset(&aString[0], 0, sizeof aString);
								memset(&aStringCopy[0], 0, sizeof aStringCopy);

								// 読み込んだ行の文字列判定
								fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
								pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

								if (strstr(&aStringCopy[0], END_KEY) != NULL)
								{ // END_KEYを読み込んだ
									nCntModel++;
									break;
								}
								else if (strstr(&aStringCopy[0], POS) != NULL)
								{ // POSを読み込んだ
									D3DXVECTOR3 pos = pLoadScripts->LoadVector3(&aStringCopy[0]);
									motionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntModel].fPosX = pos.x;
									motionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntModel].fPosY = pos.y;
									motionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntModel].fPosZ = pos.z;
								}
								else if (strstr(&aStringCopy[0], ROT) != NULL)
								{ // ROTを読み込んだ
									D3DXVECTOR3 rot = pLoadScripts->LoadVector3(&aStringCopy[0]);
									motionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntModel].fRotX = rot.x;
									motionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntModel].fRotY = rot.y;
									motionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntModel].fRotZ = rot.z;
								}
							}

						}
					}
				}

			}
		}
	} while (bLoadEnd == false);


	// ファイルを閉じる
	fclose(pFile);

	// 外部ファイル読み取り処理を破棄
	SAFE_DELETE_SINGULAR(pLoadScripts);

	return true;
}