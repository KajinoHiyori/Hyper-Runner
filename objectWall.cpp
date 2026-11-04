//==========================================
// 
// オブジェクト処理[objectWall.h]
// Author : Kajino Hiyori _Create2026/06/01
// 
//==========================================
#include "main.h"
#include "input.h"
#include "color.h"
#include "objectWall.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"

//=============================
// マクロ定義
//=============================
#define WIDTH	(500.0f)						// 幅
#define HEIGHT	(500.0f)						// 奥行
#define HEIGHT	(500.0f)						// 高さ
#define NORMAL	(D3DXVECTOR3(0.0f, 1.0f, 0.0f))	// 基本の法線
#define MOVE	(1.5f)							// 移動量
#define ROTATE	(0.05f)							// 回転量
#define FIELD_TEXTURE		("data\\TEXTURE\\field000.jpg")	// フィールドテクスチャ

//=============================
// コンストラクタ
//=============================
CObjectWall::CObjectWall(CObjectWall::PRIORITY priority) : CObject(priority)
{
	m_pos = INIT_D3DVEC3;	// 表示位置
	m_rot = INIT_D3DVEC3;	// 向き
	m_col = COLOR_WHITE;	// 色
	m_fWidth = 0.0f;		// 横幅
	m_fHeight = 0.0f;		// 奥行き
	m_pVtxBuff = NULL;		// 頂点バッファ	
	memset(&m_aIdxTexture[0], 0, sizeof m_aIdxTexture);		// テクスチャのインデックス
	for (int nCntLayer = 0; nCntLayer < NUM_LAYER; nCntLayer++)
	{
		memset(&m_aTecU[nCntLayer][0], 0, sizeof(D3DXVECTOR2) * NUM_VTXWALL);	// テクスチャ座標
		m_aScroll[nCntLayer] = 0.0f;	// スクロール速度
	}
}

//=============================
// デストラクタ
//=============================
CObjectWall::~CObjectWall()
{

}

//=============================
// 3Dポリゴン生成
//=============================
CObjectWall* CObjectWall::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float fHeight, TYPE type)
{
	CObjectWall* pObjectWall = NULL;	// オブジェクトへのポインタを用意
	if (pObjectWall == NULL)
	{ // NULLが設定できている場合
		pObjectWall = new CObjectWall;
		if (pObjectWall != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pObjectWall->Init(pos, rot, col, fWidth, fHeight, type);
			// 位置設定
			pObjectWall->SetPosition(pos);

			return pObjectWall;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CObjectWall::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float fHeight, TYPE type)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャの設定
	CTexture* pTexture = CManager::GetTexture();
	m_aIdxTexture[0] = pTexture->Register(FIELD_TEXTURE);

	// 初期化
	m_pos = pos;
	m_rot = rot;
	m_fWidth = fWidth;		// 横幅
	m_fHeight = fHeight;		// 高さ
	m_col = col;		// 色
	m_type = type;

	for (int nCntLayer = 0; nCntLayer < NUM_LAYER; nCntLayer++)
	{
		m_aTecU[nCntLayer][0] = D3DXVECTOR2(0.0f, 0.0f);
		m_aTecU[nCntLayer][1] = D3DXVECTOR2(1.0f, 0.0f);
		m_aTecU[nCntLayer][2] = D3DXVECTOR2(0.0f, 1.0f);
		m_aTecU[nCntLayer][3] = D3DXVECTOR2(1.0f, 1.0f);
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D_MULTI) * NUM_VTXWALL,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D_MULTI,									// 頂点フォーマット
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点情報の更新
	SetVtx();

	return S_OK;
}

//=============================
// テクスチャ指定処理
//=============================
void CObjectWall::SetTextureIdx(int nIdxTexture)
{
	m_aIdxTexture[m_nNumLayer] = nIdxTexture; 
	m_nNumLayer++;
}

//=============================
// ポリゴンの終了処理
//=============================
void CObjectWall::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);
}

//=============================
// ポリゴンの更新処理
//=============================
void CObjectWall::Update(void)
{
	// マルチテクスチャのテクスチャ座標更新処理
	UpdateTexture();
}

//=============================
// ポリゴンの描画処理
//=============================
void CObjectWall::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス

	// ライトをオフにする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// テクスチャステージステートの設定
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);

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

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D_MULTI));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D_MULTI);

	// テクスチャの設定
	for (int nCntTexture = 0; nCntTexture < m_nNumLayer; nCntTexture++)
	{
		pDevice->SetTexture(nCntTexture, pTexture->GetAddress(m_aIdxTexture[nCntTexture]));
	}

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// ライトをオンにする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// テクスチャステージステートの初期化
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	for (int nCntTexture = 1; nCntTexture < m_nNumLayer; nCntTexture++)
	{ // マルチテクスチャの設定を無効化
		pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		pDevice->SetTexture(1, NULL);
	}
}

//=============================
// 頂点情報の更新
//=============================
void CObjectWall::SetVtx(void)
{
	VERTEX_3D_MULTI* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定[アンカーポイントに応じて変更]
	switch (m_type)
	{
	case LEFT_TOP:		// 左揃え上寄せ
	
		break;

	case LEFT_MIDDLE:	// 左揃え中央寄せ

		break;

	case LEFT_BUTTON:	// 左揃え下寄せ
		pVtx[0].pos = D3DXVECTOR3(-0.0f, m_fHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(+m_fWidth, m_fHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-0.0f, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(+m_fWidth, 0.0f, 0.0f);
		break;

	case CENTER_TOP:	// 中央揃え上寄せ
		
		break;

	case CENTER_MIDDLE:	// 中央揃え中央寄せ
		pVtx[0].pos = D3DXVECTOR3(-m_fWidth, -m_fHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(+m_fWidth, -m_fHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-m_fWidth, m_fHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(+m_fWidth, m_fHeight, 0.0f);
		break;

	case CENTER_BUTTON:	// 中央揃え下寄せ
		pVtx[0].pos = D3DXVECTOR3(-m_fWidth, m_fHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(+m_fWidth, m_fHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-m_fWidth, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(+m_fWidth, 0.0f, 0.0f);
		break;

	case RIGHT_TOP:	// 右揃え上寄せ
		
		break;

	case RIGHT_MIDDLE:	// 右揃え中央寄せ
		
		break;

	case RIGHT_BUTTON:	// 右揃え下寄せ
		
		break;
	}

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
	pVtx[0].tex = m_aTecU[0][0];
	pVtx[1].tex = m_aTecU[0][1];
	pVtx[2].tex = m_aTecU[0][2];
	pVtx[3].tex = m_aTecU[0][3];

	pVtx[0].texM = m_aTecU[1][0];
	pVtx[1].texM = m_aTecU[1][1];
	pVtx[2].texM = m_aTecU[1][2];
	pVtx[3].texM = m_aTecU[1][3];

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//=============================
// 頂点色の更新
//=============================
void CObjectWall::SetVtxColor(void)
{
	VERTEX_3D_MULTI* pVtx;
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
// テクスチャ座標の更新
//=============================
void CObjectWall::UpdateTexture(void)
{
	for (int nCntLayer = 0; nCntLayer < NUM_LAYER; nCntLayer++)
	{
		for (int nCntVtx = 0; nCntVtx < NUM_VTXWALL; nCntVtx++)
		{
			m_aTecU[nCntLayer][nCntVtx].x += m_aScroll[nCntLayer];
		}
	}

	VERTEX_3D_MULTI* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = m_aTecU[0][0];
	pVtx[1].tex = m_aTecU[0][1];
	pVtx[2].tex = m_aTecU[0][2];
	pVtx[3].tex = m_aTecU[0][3];

	pVtx[0].texM = m_aTecU[1][0];
	pVtx[1].texM = m_aTecU[1][1];
	pVtx[2].texM = m_aTecU[1][2];
	pVtx[3].texM = m_aTecU[1][3];

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}