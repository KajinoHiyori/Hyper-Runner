//========================================================
// 
// メッシュシリンダー処理[meshcylinder.cpp]
// Author : Kajino Hiyori _Create2026/07/21
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "meshcylinder.h"
#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "input.h"
#include "scene.h"
#include "game.h"
#include "field.h"

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
CMeshCylinder::CMeshCylinder(CObject::PRIORITY priority) : CObject(priority)
{
	m_pVtxBuff = NULL;	// 頂点バッファへのポインタ
	m_pIdxBuff = NULL;	// インデックスバッファへのポインタ
	m_pos = INIT_D3DVEC3;		// 位置
	m_rot = INIT_D3DVEC3;		// 向き
	m_col = COLOR_WHITE;		// 色
	m_fRadius = 0.0f;	// 半径
	m_fHeight = 0.0f;	// 高さ
	m_fCounter = 0.0f;	// 波打ちカウンター
	m_nWave = 0;		// 波の間隔
	m_nIdxTexture = -1;			// テクスチャインデックス
}

//=============================
// デストラクタ
//=============================
CMeshCylinder::~CMeshCylinder(void)
{

}

//=============================
// 生成処理
//=============================
CMeshCylinder* CMeshCylinder::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fRadius, float fHeight, int nWave)
{
	CMeshCylinder* pMeshCylinder = NULL;	// オブジェクトへのポインタを用意
	if (pMeshCylinder == NULL)
	{ // NULLが設定できている場合
		pMeshCylinder = new CMeshCylinder;
		if (pMeshCylinder != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pMeshCylinder->Init(pos, rot, col, fRadius, fHeight, nWave);

			return pMeshCylinder;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CMeshCylinder::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fRadius, float fHeight, int nWave)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 初期化
	m_pos = pos;
	m_rot = rot;
	m_col = col;
	m_col.a = DIS_ALPHA;
	m_fRadius = fRadius;
	m_fHeight = fHeight;
	m_nWave = nWave;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * VERTEX,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,									// 頂点フォーマット
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点情報更新
	SetVtx();

	// インデックスバッファの生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * INDEX,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	WORD* pIdx;	// インデックス情報へのポインタ	
	// インデックスバッファをロックし、頂点番号へのポインタを取得
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	// 頂点番号データを設定
	int nCntDepth = 0;	// 段ずれの回数をカウント

	for (int nCntVtx = 0; nCntVtx < INDEX / 2; nCntVtx++)
	{
		if (nCntVtx % (DIVISION_W + 2) == WIDTH_SIZE)
		{
			nCntDepth++;	// 段ずれの回数を追加
			pIdx[0] = nCntVtx - nCntDepth;
			pIdx[1] = nCntVtx - nCntDepth + (DIVISION_W + 2);
		}
		else
		{
			pIdx[0] = nCntVtx - nCntDepth + WIDTH_SIZE;
			pIdx[1] = nCntVtx - nCntDepth;
		}
		pIdx += 2;
	}

	// インデックスバッファをアンロック
	m_pIdxBuff->Unlock();

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CMeshCylinder::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	// インデックスバッファの破棄
	SAFE_RELEASE(m_pIdxBuff);
}

//=============================
// 更新処理
//=============================
void CMeshCylinder::Update(void)
{
	// メッシュフィールド情報
	CField* pField = NULL;

	switch (CManager::GetMode())
	{
	//case CScene::MODE_TITLE:	// タイトル
	//
	//	break;
	//
	//case CScene::MODE_TUTORIAL:	// チュートリアル
	//	pMeshField = static_cast<CTutorial*>(CManager::GetScene())->GetMeshField();
	//	break;

	case CScene::MODE_GAME:		// ゲーム
		//pField = static_cast<CGame*>(CManager::GetScene())->GetField();
		break;

	//case CScene::MODE_RESULT:	// リザルト
	//
	//	break;
	//
	//case CScene::MODE_RANKING:	// ランキング
	//
	//	break;
	}

	// 高度変更
	if (pField != NULL)
	{
		float fHeight = pField->GetHeight(m_pos);
		m_pos.y = fHeight;
	}

	// 上端の波を表現
	m_fCounter += WAVE;
	m_fCounter = AngleNormalize(m_fCounter);

	// 頂点情報の更新
	SetVtx();
}

//=============================
// 描画処理
//=============================
void CMeshCylinder::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();
	// テクスチャ取得
	CTexture* pTexture = CManager::GetTexture();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス

	// カリングをオフにする
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

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
		VERTEX,			// 必要な頂点数
		0,
		PRIMITIVE);		// 描画するプリミティブの数

	// カリングを元に戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//=============================
// 頂点バッファ設定処理
//=============================
void CMeshCylinder::SetVtx(void)
{
	VERTEX_3D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntHeight = 0; nCntHeight < HEIGHT_SIZE; nCntHeight++, pVtx += WIDTH_SIZE)
	{
		for (int nCntWidth = 0; nCntWidth < WIDTH_SIZE; nCntWidth++)
		{
			// 頂点座標の設定
			pVtx[nCntWidth].pos.x = sinf(-D3DX_PI / (DIVISION_W / 2) * (nCntWidth)) * m_fRadius;
			if (nCntHeight == DIVISION_H)
			{ // 上端をsinカーブで管理
				float fRot = D3DX_PI / (WIDTH_SIZE - 1);
				float fAngle = AngleNormalize(m_fCounter + (float)nCntWidth * fRot * m_nWave);
				pVtx[nCntWidth].pos.y = nCntHeight * m_fHeight + sinf(fAngle) * WAVE_SIZE;
			}
			else
			{
				pVtx[nCntWidth].pos.y = nCntHeight * m_fHeight;
			}
			pVtx[nCntWidth].pos.z = cosf(-D3DX_PI / (DIVISION_W / 2) * (nCntWidth)) * m_fRadius;

			// テクスチャ座標の設定
			pVtx[nCntWidth].tex.x = (float)nCntWidth / DIVISION_W;
			pVtx[nCntWidth].tex.y = (float)nCntHeight / (DIVISION_H);

			// 法線の設定
			pVtx[nCntWidth].nor = NORMAL;

			// 頂点カラーの設定
			pVtx[nCntWidth].col = m_col;
		}
	}

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}