//========================================================
// 
// 影処理[shadow.h]
// Author : Kajino Hiyori _Create2026/07/09
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "scene.h"
#include "game.h"
#include "shadow.h"
#include "field.h"
#include "meshfield.h"
#include "color.h"
#include "debugproc.h"
#include "texture.h"
//#include "player.h"

//=============================
// マクロ定義
//=============================
#define NUM_SHADOW	(CShadow::TYPE_MAX)			// 影の種類の最大数
#define WIDTH		(10.0f)						// 幅
#define DEPTH		(10.0f)						// 奥行
#define HEIGHT		(100.0f)						// 高さ
#define NORMAL		(D3DXVECTOR3(0.0f, 1.0f, 0.0f))	// 基本の法線
#define DEFAULT		(D3DXVECTOR3(0.0f, 0.0f, 0.0f))	// 基本の座標
#define MOVE		(1.5f)							// 移動量
#define ROTATE		(0.05f)							// 回転量
#define SIZE		(10.0f)							// 大きさ

//=============================
// 静的メンバ変数宣言
//=============================
const char* CShadow::m_apFilename[NUM_SHADOW] =
{
	"data\\TEXTURE\\shadow000.jpg",	// [TYPE_CIRCLE]
	"data\\TEXTURE\\shadow001.jpg",	// [TYPE_SQUARE]
};
int CShadow::m_nNumAll = 0;	// 総数のクリア

//=============================
// コンストラクタ
//=============================
CShadow::CShadow(CObject::PRIORITY priority) : CObject3D(priority)
{
	m_type = TYPE_CIRCLE;	// 影の種類
	m_fWidth = 0.0f;		// 大きさ[幅]
	m_fDepth = 0.0f;		// 大きさ[奥行き]
	m_nIdxTexture = -1;		// テクスチャインデックス
	m_nNumAll++;			// 総数の加算
}

//=============================
// デストラクタ
//=============================
CShadow::~CShadow()
{

}

//=============================
// 影生成処理
//=============================
CShadow* CShadow::Create(TYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth)
{
	CShadow* pShadow = NULL;

	if (pShadow == NULL)
	{
		pShadow = new CShadow;	// インスタンス生成

		if (pShadow != NULL)
		{ // インスタンス生成に成功
			// エフェクトの初期化処理
			pShadow->Init(type, pos, rot, fWidth, fDepth);
			return pShadow;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CShadow::Init(TYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// テクスチャの読み込み
	int nIdx = pTexture->Register(m_apFilename[type]);

	// 3Dオブジェクトの初期化
	CObject3D::Init(pos, rot, COLOR_05fSHADOW, fWidth, fDepth);
	CObject3D::SetTextureIdx(nIdx);			// テクスチャ指定
	CObject3D::SetColor(COLOR_05fSHADOW);	// 色指定

	// オブジェクトにタイプを指定
	CObject3D::SetType(TYPE_SHADOW);

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CShadow::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// 3Dオブジェクトの終了処理
	CObject3D::Uninit();
}

//=============================
// 更新処理
//=============================
void CShadow::Update(void)
{

}

//=============================
// 描画処理
//=============================
void CShadow::Draw(void)
{
	// Object3Dの描画
	CObject3D::Draw();
}

//=============================
// 位置更新処理
//=============================
void CShadow::SetPosition(D3DXVECTOR3 pos, D3DXVECTOR3 rot, CField* pField)
{
	float fHeight = 0.0f;	// 高度格納
	D3DXVECTOR3 nor = INIT_D3DVEC3;

	if (pField != NULL)
	{
		fHeight = pField->CMeshField::GetHeight(pos, &nor);	// 高度取得
		// 角度の変更
  		rot.x = atan2f(-nor.z, -nor.y);
		rot.y = 0.0f;
		rot.z = atan2f(-nor.x, -nor.y);
	}

	// 位置更新
	pos.y = fHeight + 0.1f;
	CObject3D::SetPosition(pos);
	CObject3D::SetRotation(rot);
}