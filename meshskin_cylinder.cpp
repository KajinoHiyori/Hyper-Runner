//========================================================
// 
// メッシュスキン[円柱]処理[meshskin_cylinder.cpp]
// Author : Kajino Hiyori _Create2026/11/11
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "meshskin_cylinder.h"
#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "input.h"
#include "scene.h"
#include "game.h"

//=============================
// マクロ定義
//=============================
#define DIVISION_W	(12)		// 横の分割数
#define DIVISION_H	(2)		// 高さの分割数
#define WIDTH_SIZE	(DIVISION_W + 1)	// 横幅の大きさ
#define HEIGHT_SIZE	(DIVISION_H + 1)	// 奥行きの大きさ
#define PRIMITIVE	(DIVISION_W * DIVISION_H * 2 + (DIVISION_H - 1) * 4)	// プリミティブ数
#define INDEX		(PRIMITIVE + 2)					// インデックス数
#define VERTEX		(WIDTH_SIZE * HEIGHT_SIZE)		// 頂点数
#define SIZE		(70.0f)	// 大きさ
#define HEIGHT		(950.0f)	// 高さ
#define MOVE		(1.5f)		// 移動量
#define ROTATE		(0.05f)		// 回転量
#define NORMAL		(D3DXVECTOR3(0.0f, 0.0f, -1.0f))	// 法線
#define DEFAULT		(D3DXVECTOR3(0.0f, 0.0f, 0.0f))		// デフォルト
#define POS			(D3DXVECTOR3(0.0f, -250.0f, 0.0f))	// 位置
#define DIS_ALPHA	(0.5f)	// 半透明設定
#define WAVE		(0.05f)	// 波打ち管理
#define WAVE_SIZE	(50.0f)	// 波の高度管理

//=============================
// コンストラクタ
//=============================
CMeshSkinCylinder::CMeshSkinCylinder(CObject::PRIORITY priority) : CObject(priority)
{
	m_pVtxBuff = NULL;	// 頂点バッファへのポインタ
	m_pIdxBuff = NULL;	// インデックスバッファへのポインタ
	m_pos = INIT_D3DVEC3;		// 位置
	m_rot = INIT_D3DVEC3;		// 向き
	m_col = COLOR_WHITE;		// 色
	m_fRadius = 0.0f;	// 半径
	m_fHeight = 0.0f;	// 高さ
	m_nIdxTexture = -1;			// テクスチャインデックス
	m_nDivisionW = 0;		// 横分割数
	m_nDivisionH = 0;		// 縦分割数
	m_nWidthSize = 0;		// 横幅の大きさ
	m_nHeightSize = 0;		// 高さの大きさ
	m_nNumPrimitive = 0;	// プリミティブ数
	m_nNumIdx = 0;			// インデックス数
	m_nNumVertex = 0;		// 頂点数
}

//=============================
// デストラクタ
//=============================
CMeshSkinCylinder::~CMeshSkinCylinder(void)
{

}

//=============================
// 生成処理
//=============================
CMeshSkinCylinder* CMeshSkinCylinder::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fRadius, float fHeight)
{
	CMeshSkinCylinder* pMeshCylinder = NULL;	// オブジェクトへのポインタを用意
	if (pMeshCylinder == NULL)
	{ // NULLが設定できている場合
		pMeshCylinder = new CMeshSkinCylinder;
		if (pMeshCylinder != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pMeshCylinder->Init(pos, rot, col, fRadius, fHeight);

			return pMeshCylinder;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CMeshSkinCylinder::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fRadius, float fHeight)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 初期化
	m_pos = pos;
	m_rot = rot;
	m_col = col;
	m_fRadius = fRadius;
	m_fHeight = fHeight;
	m_nDivisionW = DIVISION_W;		// 横分割数
	m_nDivisionH = DIVISION_H;		// 縦分割数
	m_nWidthSize = m_nDivisionW + 1;		// 横幅の大きさ
	m_nHeightSize = m_nDivisionH + 1;		// 高さの大きさ
	m_nNumPrimitive = (m_nDivisionW * m_nDivisionH * 2 + (m_nDivisionH - 1) * 4);	// プリミティブ数
	m_nNumIdx = m_nNumPrimitive + 2;			// インデックス数
	m_nNumVertex = m_nWidthSize * m_nHeightSize;		// 頂点数

	// ボーン情報設定
	m_aBone[0].pos = INIT_D3DVEC3;
	m_aBone[0].rot = INIT_D3DVEC3;
	m_aBone[1].pos = D3DXVECTOR3(0.0f, 30.0f, 0.0f);
	m_aBone[1].rot = INIT_D3DVEC3;

	// オフセット設定
	SetOffset();

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVertex,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,									// 頂点フォーマット
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点情報更新
	SetVtx();

	// インデックスバッファの生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIdx,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	// インデックスバッファ更新
	SetIdx();

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CMeshSkinCylinder::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	// インデックスバッファの破棄
	SAFE_RELEASE(m_pIdxBuff);
}

//=============================
// 更新処理
//=============================
void CMeshSkinCylinder::Update(void)
{
}

//=============================
// 描画処理
//=============================
void CMeshSkinCylinder::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();
	// テクスチャ取得
	CTexture* pTexture = CManager::GetTexture();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス

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
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// 描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		0,
		m_nNumVertex,			// 必要な頂点数
		0,
		m_nNumPrimitive);		// 描画するプリミティブの数
}

//=============================
// オフセット処理
//=============================
void CMeshSkinCylinder::SetOffset(void)
{
	int nWidth = 0;
	int n = 0;
	for (int nCntHeight = 0; nCntHeight < m_nHeightSize; nCntHeight++, nWidth += m_nWidthSize)
	{
		for (int nCntWidth = 0; nCntWidth < m_nWidthSize; nCntWidth++)
		{
			// 頂点座標の設定
			m_aPosOffset[0][nCntWidth + nWidth].x = sinf(D3DX_PI / (m_nDivisionW / 2) * (nCntWidth)) * m_fRadius;
			m_aPosOffset[0][nCntWidth + nWidth].y = nCntHeight * m_fHeight;
			m_aPosOffset[0][nCntWidth + nWidth].z = cosf(D3DX_PI / (m_nDivisionW / 2) * (nCntWidth)) * m_fRadius;
			n++;
		}
	}
	
	if (n != m_nNumVertex) return;

	for (int nCntVtx = 0; nCntVtx < m_nNumVertex; nCntVtx++)
	{
		m_aPosVtx[nCntVtx] = m_aPosOffset[0][nCntVtx];
	}
}

//=============================
// 頂点バッファ設定処理
//=============================
void CMeshSkinCylinder::SetVtx(void)
{
	VERTEX_3D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntHeight = 0; nCntHeight < m_nHeightSize; nCntHeight++, pVtx += m_nWidthSize)
	{
		for (int nCntWidth = 0; nCntWidth < m_nWidthSize; nCntWidth++)
		{
			// テクスチャ座標の設定
			pVtx[nCntWidth].tex.x = (float)nCntWidth / m_nDivisionW;
			pVtx[nCntWidth].tex.y = (float)nCntHeight / (m_nDivisionH);

			// 法線の設定
			pVtx[nCntWidth].nor = NORMAL;

			// 頂点カラーの設定
			pVtx[nCntWidth].col = m_col;
		}
	}

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

void SetDrawVtxBuff(void);


//=============================
// インデックスバッファ設定処理
//=============================
void CMeshSkinCylinder::SetIdx(void)
{
	WORD* pIdx;	// インデックス情報へのポインタ	
	// インデックスバッファをロックし、頂点番号へのポインタを取得
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	// 頂点番号データを設定
	int nCntDepth = 0;	// 段ずれの回数をカウント

	for (int nCntVtx = 0; nCntVtx < m_nNumIdx / 2; nCntVtx++)
	{
		if (nCntVtx % (m_nDivisionW + 2) == m_nWidthSize)
		{
			nCntDepth++;	// 段ずれの回数を追加
			pIdx[0] = nCntVtx - nCntDepth;
			pIdx[1] = nCntVtx - nCntDepth + (m_nDivisionW + 2);
		}
		else
		{
			pIdx[0] = nCntVtx - nCntDepth + m_nWidthSize;
			pIdx[1] = nCntVtx - nCntDepth;
		}
		pIdx += 2;
	}

	// インデックスバッファをアンロック
	m_pIdxBuff->Unlock();
}