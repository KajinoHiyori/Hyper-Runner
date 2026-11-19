//==========================================
// 
// ステンシルシャドウ処理[shadowST.h]
// Author : Kajino Hiyori _Create2026/09/28~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "shadowST.h"
#include "camera.h"
#include "color.h"
#include "debugproc.h"
#include "renderer.h"
#include "manager.h"
#include "light.h"
#include "polygon2D.h"

//=============================
// マクロ定義
//=============================

//=============================
// 静的メンバ変数宣言
//=============================
int CShadowST::m_nNumAll = 0;	// 総数
CPolygon2D* CShadowST::m_pShadow = NULL;	// 画面全体に貼るポリゴン情報
const char* CShadowST::m_pModelName = "data\\MODEL\\stencilshadow\\stencilshadow.x";	// モデル名

//=============================
// コンストラクタ
//=============================
CShadowST::CShadowST(CObject::PRIORITY priority): CObjectX(priority)
{
	m_fRadius = 0.0f;	// 半径[拡大率]
	m_fHeight = 0.0f;	// 高さ[拡大率]
	m_nNumAll++;
}

//=============================
// デストラクタ
//=============================
CShadowST::~CShadowST()
{

}

//=============================
// ステンシルシャドウ生成処理
//=============================
CShadowST* CShadowST::Create(D3DXVECTOR3 pos, float fRadius)
{
	CShadowST* pShadowST = NULL;

	if (pShadowST == NULL)
	{
		pShadowST = new CShadowST;	// インスタンス生成
		
		if (pShadowST != NULL)
		{ // インスタンス生成に成功
			// ステンシルシャドウの初期化処理
			pShadowST->Init(pos, fRadius);

			return pShadowST;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

#define HEIGHT	(10.0f)	// 高さの拡大率
#define SHADOW_WIDTH	((float)SCREEN_WIDTH)	// 影描画の幅
#define SHADOW_HEIGHT	((float)SCREEN_HEIGHT)	// 影描画の幅
//=============================
// 初期化処理
//=============================
HRESULT CShadowST::Init(D3DXVECTOR3 pos, float fRadius)
{
	// オブジェクトにタイプを指定
	CObjectX::Init(pos, INIT_D3DVEC3, m_pModelName);
	CObjectX::SetType(CObject::TYPE_SHADOW);
	m_fRadius = fRadius;
	m_fHeight = HEIGHT;
	CObjectX::SetScale(D3DXVECTOR3(m_fRadius, m_fHeight, m_fRadius));

	// ポリゴンの用意
	if (m_pShadow != NULL) return S_OK;
	m_pShadow = CPolygon2D::Create(SCREEN_CENTER, SHADOW_WIDTH, SHADOW_HEIGHT, COLOR_BLACK);

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CShadowST::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// Xオブジェクトの終了処理
	CObjectX::Uninit();
}

//=============================
// 更新処理
//=============================
void CShadowST::Update(void)
{
	
}

//=============================
// 描画処理
//=============================
void CShadowST::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();
	
	// ステンシルテストを有効にする
	pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
	// Zバッファへの書き込みを無効化
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	// カラーバッファへの書き込みを無効化
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00000000);

	// ステンシルバッファの比較パラメータを設定
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);	// Zステンシルテストに合格&&Zテストに合格
	pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCR);	// Zステンシルテストに合格&&Zテストに不合格
	pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);	// ステンシルテストが不合格

	// 表面をカリング
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	// モデル描画
	CObjectX::Draw();

	//ステンシルテストの参照値を1にする
	pDevice->SetRenderState(D3DRS_STENCILREF, 0x01);
	
	// ステンシルバッファの比較パラメータを設定
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);	// Zステンシルテストに合格&&Zテストに合格
	pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);	// Zステンシルテストに合格&&Zテストに不合格
	pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);	// ステンシルテストが不合格
	
	// 裏面をカリング
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	// モデル描画
	CObjectX::Draw();

	// カラーバッファへの書き込みを有効化
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x000000F);
	// ステンシルバッファの参照値を2に設定
	pDevice->SetRenderState(D3DRS_STENCILREF, 0x02);

	// ステンシルバッファの比較パラメータを設定
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);	// Zステンシルテストに合格&&Zテストに合格
	pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);	// Zステンシルテストに合格&&Zテストに不合格
	pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);	// ステンシルテストが不合格
	
	// ポリゴンの描画
	m_pShadow->Draw();

	// ステンシルテストを無効にする
	pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	// Zバッファへの書き込みを有効化
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	// カラーバッファへの書き込みを有効化
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);
}

//=============================
// ステンシルポリゴンの終了処理
//=============================
void CShadowST::UninitPolygon(void)
{
	SAFE_UNINIT(m_pShadow);
}

//=============================
// ステンシルポリゴンの描画処理
//=============================
void CShadowST::DrawStencil(void)
{

}