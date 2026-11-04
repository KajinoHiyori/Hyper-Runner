//==========================================
// 
// 数値処理[number3D.h]
// Author : Kajino Hiyori _Create2026/06/01
// 
//==========================================
#include "main.h"
#include "input.h"
#include "color.h"
#include "number3D.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"

//=============================
// マクロ定義
//=============================
#define WIDTH	(500.0f)						// 幅
#define DEPTH	(500.0f)						// 奥行
#define HEIGHT	(500.0f)						// 高さ
#define NORMAL	(D3DXVECTOR3(0.0f, 1.0f, 0.0f))	// 基本の法線
#define MOVE	(1.5f)							// 移動量
#define ROTATE	(0.05f)							// 回転量

//=============================
// 静的メンバ変数宣言
//=============================
const char* CNumber3D::m_pFilename = "data\\TEXTURE\\number.png";

//=============================
// コンストラクタ
//=============================
CNumber3D::CNumber3D()
{
	m_pos = INIT_D3DVEC3;	// 表示位置
	m_rot = INIT_D3DVEC3;	// 向き
	m_col = COLOR_WHITE;	// 色
	m_fWidth = 0.0f;		// 横幅
	m_fHeight = 0.0f;		// 奥行き
	m_pVtxBuff = NULL;		// 頂点バッファ	
	m_nIdxTexture = 0;		// テクスチャのインデックス
	m_nNum = 0;	// 数値
}

//=============================
// デストラクタ
//=============================
CNumber3D::~CNumber3D()
{

}

//=============================
// 3Dポリゴン生成
//=============================
CNumber3D* CNumber3D::Create(D3DXVECTOR3 pos, D3DXCOLOR col, float fWidth, float fHeight, int nNum)
{
	CNumber3D* pNumber3D = NULL;	// 数値へのポインタを用意
	if (pNumber3D == NULL)
	{ // NULLが設定できている場合
		pNumber3D = new CNumber3D;
		if (pNumber3D != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pNumber3D->Init(pos, col, fWidth, fHeight, nNum);
			// 位置設定
			pNumber3D->SetPosition(pos);

			return pNumber3D;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CNumber3D::Init(D3DXVECTOR3 pos, D3DXCOLOR col, float fWidth, float fHeight, int nNum)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャの設定
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register(m_pFilename);

	// 初期化
	m_pos = pos;
	m_fWidth = fWidth;		// 横幅
	m_fHeight = fHeight;		// 奥行き
	m_col = col;		// 色
	m_nNum = nNum;	// 数値

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VTX3D,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,									// 頂点フォーマット
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点情報の更新
	SetVtx();

	return S_OK;
}

//=============================
// ポリゴンの終了処理
//=============================
void CNumber3D::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);
}

//=============================
// ポリゴンの更新処理
//=============================
void CNumber3D::Update(void)
{
}

//=============================
// ポリゴンの描画処理
//=============================
void CNumber3D::Draw(D3DXMATRIX mtxParent)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	D3DXMATRIX	mtxView;	// ビューマトリックスの格納
	D3DXMATRIX	mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXMATRIX	mtxParentLocal;					// 親のマトリックス

	// ポリゴンのワールドマトリックスを初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ビューマトリックスを取得する
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// パーツの位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// 親マトリックスを設定
	mtxParentLocal = mtxParent;

	// 算出したパーツのワールドマトリックスと親モデルのマトリックスを掛け合わせる
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParentLocal);

	// パーツのワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// UIの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//=============================
// 頂点情報の更新
//=============================
void CNumber3D::SetVtx(void)
{
	VERTEX_3D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_fWidth, 0.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f, -m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_fWidth, -m_fHeight, 0.0f);

	// 法線の設定
	pVtx[0].nor = NORMAL;
	pVtx[1].nor = NORMAL;
	pVtx[2].nor = NORMAL;
	pVtx[3].nor = NORMAL;

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(m_nNum * 0.1f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(m_nNum * 0.1f + 0.1f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(m_nNum * 0.1f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(m_nNum * 0.1f + 0.1f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//=============================
// 頂点色の更新
//=============================
void CNumber3D::SetVtxColor(void)
{
	VERTEX_3D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//=============================
// 数値設定
//=============================
int CNumber3D::SetTecU(int nDigit, int nSource, int nCnt)
{
	int nTecU = 0;

	if (nCnt == nDigit - 1)
	{
		nTecU = nSource % 10 / 1;
	}
	else
	{
		float fNumX = powf(static_cast <float>(10), static_cast <float>(nDigit - nCnt));		// 剰余算
		float fNumY = powf(static_cast <float>(10), static_cast <float>(nDigit - nCnt - 1));	// 除算
		nTecU = nSource % static_cast<int>(fNumX) / static_cast<int>(fNumY);
	}

	return nTecU;
}
