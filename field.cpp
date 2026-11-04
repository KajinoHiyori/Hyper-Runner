//==========================================
// 
// フィールド処理[field.h]
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
#include "game.h"
#include "object3D.h"
#include "field.h"
#include "camera.h"
#include "debugproc.h"
#include "loadscripts.h"

//=============================
// マクロ定義
//=============================
#define FIELDSET		("FIELDSET")		// FIELDSET
#define END_FIELDSET	("END_FIELDSET")	// END_FIELDSET
#define TEXTURENAME		("TEXTURENAME")		// TEXTURENAME
#define POS				("POS")				// POS
#define ROT				("ROT")				// ROT
#define WIDTH			("WIDTH")			// WIDTH
#define DEPTH			("DEPTH")			// DEPTH
#define DIVISION		("DIVISION")		// DIVISION
#define MAXHEIGHT		("MAXHEIGHT")		// MAXHEIGHT
#define HEIGHTVALUE		("HEIGHTVALUE")		// HEIGHTVALUE
#define TEXTURERANGE	("TEXTURERANGE")	// TEXTURERANGE
#define FIELDTYPE		("FIELDTYPE")		// FIELDTYPE

//=============================
// 静的メンバ変数宣言
//=============================
int CField::m_nNumAll = 0;

//=============================
// コンストラクタ
//=============================
CField::CField(CObject::PRIORITY priority) : CMeshField(priority)
{
	m_nNumAll++;
}

//=============================
// デストラクタ
//=============================
CField::~CField()
{

}

//=============================
// フィールド生成処理
//=============================
CField* CField::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CField* pField = NULL;

	if (pField == NULL)
	{
		pField = new CField;	// インスタンス生成

		if (pField != NULL)
		{ // インスタンス生成に成功
			// フィールドの初期化処理
			pField->Init(pos, rot);

			return pField;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CField::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	if (FAILED(LoadScripts(GAMESCRIPTS_PATH)))
	{
		return E_FAIL;
	}

	// オブジェクトにタイプを指定
	CMeshField::SetType(CObject::TYPE_FIELD);

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CField::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// 3Dオブジェクトの終了処理
	CMeshField::Uninit();
}

//=============================
// 更新処理
//=============================
void CField::Update(void)
{
	// メッシュフィールドの更新処理
	CMeshField::Update();
}

//=============================
// 描画処理
//=============================
void CField::Draw(void)
{
	// 3Dオブジェクトの描画処理
	CMeshField::Draw();
}

//=============================
// 高度取得
//=============================
float CField::GetHeight(D3DXVECTOR3 pos)
{
	return CMeshField::GetHeight(pos);
}

//=============================
// 外部ファイル読み込み処理
//=============================
HRESULT CField::LoadScripts(const char* pFileName)
{
	// スクリプト読み込み処理インスタンス生成
	CLoadScripts* pLoadScripts = NULL;
	pLoadScripts = new CLoadScripts;

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]

	char aTextureName[MAX_STRING];	// モデル名
	memset(&aTextureName[0], 0, sizeof aTextureName);
	D3DXVECTOR3 pos = INIT_D3DVEC3;	// 位置
	D3DXVECTOR3 rot = INIT_D3DVEC3;	// 向き
	float fWidth = 0.0f;	// 幅
	float fDepth = 0.0f;	// 奥行き
	int nDivison = 0;		// 分割数
	int nMaxHeight = 0;	// 最高高度基準値
	float fHeightValue = 0.0f;	// 高度乗算値
	float fTexRange = 0.0f;			// テクスチャ読み込み幅
	CMeshField::TYPE type = CMeshField::TYPE_FLAT;	// フィールドの状態
	// ファイルを開く
	FILE* pFile = fopen(pFileName, "r");

	if (pFile == NULL)
	{ // 読み込みに失敗
		return E_FAIL;
	}

	do
	{
		// 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

		if (strstr(&aStringCopy[0], FIELDSET) != NULL)
		{ // FIELDSETを読み込んだ
			while (1)
			{
				// 文字列のクリア
				memset(&aString[0], 0, sizeof aString);
				memset(&aStringCopy[0], 0, sizeof aStringCopy);

				// 読み込んだ行の文字列判定
				fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
				pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

				if (strstr(&aStringCopy[0], TEXTURENAME) != NULL)
				{ // TEXTURENAMEを読み込んだ
					pLoadScripts->LoadFileName(&aStringCopy[0]);
					memcpy(&aTextureName[0], &aStringCopy[0], sizeof aStringCopy);
				}
				if (strstr(&aStringCopy[0], POS) != NULL)
				{ // POSを読み込んだ
					pos = pLoadScripts->LoadVector3(&aStringCopy[0]);
				}
				if (strstr(&aStringCopy[0], ROT) != NULL)
				{ // ROTを読み込んだ
					rot = pLoadScripts->LoadVector3(&aStringCopy[0]);
				}
				if (strstr(&aStringCopy[0], WIDTH) != NULL)
				{ // WIDTHを読み込んだ
					fWidth = pLoadScripts->LoadFloat(&aStringCopy[0]);
				}
				if (strstr(&aStringCopy[0], DEPTH) != NULL)
				{ // DEPTHを読み込んだ
					fDepth = pLoadScripts->LoadFloat(&aStringCopy[0]);
				}
				if (strstr(&aStringCopy[0], DIVISION) != NULL)
				{ // DIVISIONを読み込んだ
					nDivison = pLoadScripts->LoadInt(&aStringCopy[0]);
				}
				if (strstr(&aStringCopy[0], MAXHEIGHT) != NULL)
				{ // MAXHEIGHTを読み込んだ
					nMaxHeight = pLoadScripts->LoadInt(&aStringCopy[0]);
				}
				if (strstr(&aStringCopy[0], HEIGHTVALUE) != NULL)
				{ // HEIGHTVALUEを読み込んだ
					fHeightValue = pLoadScripts->LoadFloat(&aStringCopy[0]);
				}
				if (strstr(&aStringCopy[0], TEXTURERANGE) != NULL)
				{ // TEXTURERANGEを読み込んだ
					fTexRange = pLoadScripts->LoadFloat(&aStringCopy[0]);
				}
				if (strstr(&aStringCopy[0], FIELDTYPE) != NULL)
				{ // FIELDTYPEを読み込んだ
					int nType = pLoadScripts->LoadInt(&aStringCopy[0]);
					type = (CMeshField::TYPE)nType;
				}
				if (strstr(&aStringCopy[0], END_FIELDSET) != NULL)
				{ // END_PLAYERSETを読み込んだ
					bLoadEnd = true;
					break;
				}
			}
		}
	} while (bLoadEnd == false);

	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// 3Dオブジェクトの初期化
	CMeshField::Init(pos, type, fWidth, fDepth, nDivison, nMaxHeight, fHeightValue, fTexRange);
	CMeshField::SetTextureIdx(CManager::GetTexture()->Register(&aTextureName[0]));

	// スクリプト破棄
	SAFE_DELETE_SINGULAR(pLoadScripts);

	return S_OK;
}




