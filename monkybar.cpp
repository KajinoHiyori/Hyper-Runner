//==========================================
// 
// 雲梯処理[monkybar.h]
// Author : Kajino Hiyori
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "monkybar.h"
#include "camera.h"
#include "input.h"
#include "color.h"
#include "debugproc.h"
#include "renderer.h"
#include "texture.h"
#include "scene.h"
#include "game.h"
#include "ranking.h"
#include "effect.h"
#include "player.h"

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
#define SHADOW_POS			(D3DXVECTOR3(CObjectX::GetPosition().x, CObjectX::GetPosition().y + 5.0f, CObjectX::GetPosition().z))	// 影の位置
#define MONKYBARSET				("MONKYBARSET")		// MONKYBARSET
#define END_MONKYBARSET			("END_MONKYBARSET")	// END_MONKYBARSET
#define MONKYBARINFO			("MONKYBARINFO")		// MONKYBARINFO
#define END_MONKYBARINFO		("END_MONKYBARINFO")	// END_MONKYBARINFO
#define NUMMONKYBAR				("NUMMONKYBAR")			// NUMMONKYBAR
// ファイルパス
#define MONKYBARINFO_PATH		("data\\SCRIPTS\\monkybar_info.txt")	// 雲梯情報

//=============================
// 静的メンバ変数宣言
//=============================
int CMonkyBar::m_nNumAll = 0;
const char* CMonkyBar::m_pFileName = "data\\MODEL\\monkeybars\\monkeybar.x";

//=============================
// コンストラクタ
//=============================
CMonkyBar::CMonkyBar(CObject::PRIORITY priority): CObjectX(priority)
{
	m_posOld = INIT_D3DVEC3;	// モデルの1フレーム前の位置
	m_move = INIT_D3DVEC3;		// モデルの移動量
	m_bJump = false;			// ジャンプフラグ
	m_state = STATE_NONE;		// ステート管理
	m_nStateCounter = 0;		// ステート管理カウンター
	m_fSize = 0.0f;				// サイズ
	m_pShadow = NULL;			// 影
	m_nNumAll++;
}

//=============================
// デストラクタ
//=============================
CMonkyBar::~CMonkyBar()
{

}

//=============================
// 雲梯生成処理
//=============================
CMonkyBar* CMonkyBar::Create(D3DXVECTOR3 pos)
{
	CMonkyBar* pMonkyBar = NULL;

	if (pMonkyBar == NULL)
	{
		pMonkyBar = new CMonkyBar;	// インスタンス生成
		
		if (pMonkyBar != NULL)
		{ // インスタンス生成に成功
			// 雲梯の初期化処理
			pMonkyBar->Init(pos);

			return pMonkyBar;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CMonkyBar::Init(D3DXVECTOR3 pos)
{
	// オブジェクトにタイプを指定
	CObjectX::Init(pos, INIT_D3DVEC3, m_pFileName);
	CObjectX::SetType(CObject::TYPE_MONKYBAR);
	
	m_posOld = pos;	// モデルの1フレーム前の位置

	// 状態管理
	m_state = STATE_NONE;
	m_move = APPEAR_JUMP;

	// シャドウの生成
	if (m_pShadow != NULL) return E_FAIL;
//	m_pShadow = CShadow::Create(SHADOW_POS, 20.0f);

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CMonkyBar::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// Xオブジェクトの終了処理
	CObjectX::Uninit();

	// 影の破棄
	m_pShadow = NULL;
}

//=============================
// 更新処理
//=============================
void CMonkyBar::Update(void)
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

	if (m_pShadow == NULL) return;
	// 影の位置更新
	//m_pShadow->SetPosition(SHADOW_POS);
}

//=============================
// 描画処理
//=============================
void CMonkyBar::Draw(void)
{
	// Xオブジェクトの描画処理
	CObjectX::Draw();
}

//=============================
// 雲梯情報読み取り
//=============================
void CMonkyBar::LoadInfo(void)
{

}

//=============================
// 雲梯配置情報読み取り
//=============================
void CMonkyBar::LoadSetting(const char* pFileName)
{
	return;

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

		if (strstr(&aStringCopy[0], MONKYBARSET) != NULL)
		{ // MONKYBARSETを読み込んだ
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
				}
				if (strstr(&aStringCopy[0], END_MONKYBARSET) != NULL)
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

#define COLLISION	(15.0f)	// 雲梯との判定処理
//=============================
// プレイヤーとの判定処理
//=============================
bool CMonkyBar::Collision(D3DXVECTOR3 pos)
{
	D3DXVECTOR3 posBar = CObjectX::GetPosition();
	D3DXVECTOR3 vtxMin = CObjectX::GetVtxMin();
	D3DXVECTOR3 vtxMax = CObjectX::GetVtxMax();
	float fLenHeight = sqrtf((posBar.y - pos.y) * (posBar.y - pos.y));
	float fLengthDepth = sqrtf((posBar.z - pos.z) * (posBar.z - pos.z));
	if (posBar.x + vtxMin.x <= posBar.x		// X方向[左端]
		&& posBar.x + vtxMax.x >= posBar.x	// X方向[右端]
		&& fLenHeight <= COLLISION			// Y方向の判定
		&& fLengthDepth <= COLLISION)	// Z方向の判定
	{
		return true;
	}

	return false;
}

//=============================
// ゲーム処理
//=============================
void CMonkyBar::UpdateGame(void)
{

}