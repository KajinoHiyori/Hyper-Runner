//==========================================
// 
// マグマ処理[magma.h]
// Author : Kajino Hiyori _Create2026/05/18~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "magma.h"
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
#define MAGMA_WIDTH	(1000.0f)		// マグマのサイズ
#define MAGMA_DEPTH	(10000.0f)		// マグマのサイズ
#define SCROLL(x)	(0.0005f * x + -0.0001f)	// スクロール速度

//=============================
// 静的メンバ変数宣言
//=============================
const char* CMagma::m_apFilenameType[CMagma::TYPE_MAX] =
{
	"data\\TEXTURE\\magma\\field000.jpg",	// テクスチャ000
	"data\\TEXTURE\\magma\\field008.jpg",	// テクスチャ001
};
int CMagma::m_nNumAll = 0;

//=============================
// コンストラクタ
//=============================
CMagma::CMagma(CObject::PRIORITY priority): CObjectField(priority)
{
	// メンバ変数の値をクリア
	m_nNumAll++;			// マグマの総数を加算
}

//=============================
// デストラクタ
//=============================
CMagma::~CMagma()
{

}

//=============================
// マグマ生成処理
//=============================
CMagma* CMagma::Create(D3DXVECTOR3 pos)
{
	CMagma* pMagma = NULL;

	if (pMagma == NULL)
	{
		pMagma = new CMagma;	// インスタンス生成
		
		if (pMagma != NULL)
		{ // インスタンス生成に成功
			// マグマの初期化処理
			pMagma->Init(pos);

			return pMagma;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CMagma::Init(D3DXVECTOR3 pos)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	CObjectField::Init(pos, INIT_D3DVEC3, COLOR_WHITE, MAGMA_WIDTH, MAGMA_DEPTH);

	// テクスチャの読み込み
	for (int nCntType = 0; nCntType < TYPE_MAX; nCntType++)
	{
		CObjectField::SetTextureIdx(pTexture->Register(m_apFilenameType[nCntType]));
		// スクロール速度を指定
		CObjectField::SetScroll(1, SCROLL(nCntType));
	}
	
	// オブジェクトにタイプを指定
	CObjectField::SetType(TYPE_MAGMA);

	
	return S_OK;
}

//=============================
// 終了処理
//=============================
void CMagma::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// 終了処理
	CObjectField::Uninit();
}

//=============================
// 更新処理
//=============================
void CMagma::Update(void)
{
	// フィールドの更新処理
	CObjectField::Update();
}

//=============================
// 描画処理
//=============================
void CMagma::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 描画処理
	CObjectField::Draw();
}