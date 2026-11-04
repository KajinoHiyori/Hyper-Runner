//==========================================
// 
// メッシュリング処理[meshring.h]
// Create : 2026/10/21~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "meshring.h"
#include "input.h"
#include "color.h"
#include "manager.h"
#include "renderer.h"

//=============================
// マクロ定義
//=============================
#define DIVISION_W	(8)				// 横の分割数
#define DIVISION_D	(2)					// 高さの分割数
#define WIDTH_SIZE	(DIVISION_W + 1)	// 横幅の大きさ
#define DEPTH_SIZE	(DIVISION_D + 1)	// 奥行きの大きさ
#define PRIMITIVE	(DIVISION_W * DIVISION_D * 2 + (DIVISION_D - 1) * 4)	// プリミティブ数
#define INDEX		(PRIMITIVE + 2)				// インデックス数
#define VERTEX		(WIDTH_SIZE * DEPTH_SIZE)	// 頂点数
#define SIZE		(100.0f)	// 高さ
#define LIFE		(30)		// 寿命
#define MOVE		(1.5f)		// 移動量
#define ROTATE		(0.05f)		// 回転量
#define INSIDE		(3.0f)		// 内側の増加量
#define OUTSIDE		(1.5f)		// 内側の増加量
#define NORMAL		(D3DXVECTOR3(0.0f, 0.0f, -1.0f))	// 法線

//=============================
// コンストラクタ
//=============================
CMeshRing::CMeshRing(CObject::PRIORITY priority) : CObject(priority)
{
	// 値のクリア
	m_pos = INIT_D3DVEC3;
	m_rot = INIT_D3DVEC3;
	m_pVtxBuff = NULL;
	m_pIdxBuff = NULL;
	m_type = RINGTYPE_NONE;
	m_colInside = COLOR_WHITE;
	m_colOutside = COLOR_WHITE;
	m_fInside = SIZE;
	m_fOutside = SIZE;
	m_nLife = LIFE;
}

//=============================
// デストラクタ
//=============================
CMeshRing::~CMeshRing()
{

}

//=============================
// 生成処理
//=============================
CMeshRing* CMeshRing::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, RINGTYPE type, float fInside, float fOutside, int nLife)
{
	CMeshRing* pRing = NULL;

	if (pRing == NULL)
	{
		pRing = new CMeshRing;	// インスタンス生成

		if (pRing != NULL)
		{ // インスタンス生成に成功
			// プレイヤーの初期化処理
			pRing->Init(pos, rot, type, fInside, fOutside, nLife);

			return pRing;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CMeshRing::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, RINGTYPE type, float fInside, float fOutside, int nLife)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 各種数値の格納
	m_pos = pos;
	m_pos.y += 0.1f;	// ちらつきを軽減
	m_rot = rot;
	m_type = type;
	m_nLife = nLife;
	m_fInside = fInside;
	m_fOutside = fOutside;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * VERTEX,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,									// 頂点フォーマット
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点情報の設定
	SetVtx();

	// インデックスバッファの生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * INDEX,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	// インデックスバッファの設定
	SetIdx();

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CMeshRing::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	// インデックスバッファの破棄
	SAFE_RELEASE(m_pIdxBuff);
}

//=============================
// 更新処理
//=============================
void CMeshRing::Update(void)
{
	// 各種数値の変更
	m_fInside += INSIDE;
	m_fOutside += OUTSIDE;
	m_nLife--;
	m_colInside.a -= 0.1f;
	m_colOutside.a -= 0.1f;

	// 寿命の管理
	if (m_nLife <= 0 || (m_fInside >= m_fOutside))
	{ // 自身を破棄
		Release();
		return;
	}
			
	// 頂点情報の設定
	SetVtx();
}

//=============================
// 描画処理
//=============================
void CMeshRing::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();
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
	pDevice->SetTexture(0, NULL);

	// リングの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		0,
		VERTEX,			// 必要な頂点数
		0,
		PRIMITIVE);		// 描画するプリミティブの数
}

//=============================
// 頂点情報の設定
//=============================
void CMeshRing::SetVtx(void)
{
	VERTEX_3D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntDepth = 0; nCntDepth < DEPTH_SIZE; nCntDepth++, pVtx += WIDTH_SIZE)
	{
		if (nCntDepth == 0)
		{
			for (int nCntWidth = 0; nCntWidth < WIDTH_SIZE; nCntWidth++)
			{
				// 頂点座標の設定
				pVtx[nCntWidth].pos.x = sinf(-D3DX_PI / (DIVISION_W / 2) * (nCntWidth)) * (nCntDepth + 1) * m_fInside;
				pVtx[nCntWidth].pos.y = 0.0f;
				pVtx[nCntWidth].pos.z = cosf(-D3DX_PI / (DIVISION_W / 2) * (nCntWidth)) * (nCntDepth + 1) * m_fInside;

				// テクスチャ座標の設定
				pVtx[nCntWidth].tex.x = (float)nCntWidth / DIVISION_W;
				pVtx[nCntWidth].tex.y = (float)nCntDepth / (DIVISION_D);

				// 法線の設定
				pVtx[nCntWidth].nor = NORMAL;

				// 頂点カラーの設定
				pVtx[nCntWidth].col = m_colInside;
			}
		}
		else
		{
			for (int nCntWidth = 0; nCntWidth < WIDTH_SIZE; nCntWidth++)
			{
				// 頂点座標の設定
				pVtx[nCntWidth].pos.x = sinf(-D3DX_PI / (DIVISION_W / 2) * (nCntWidth)) * (nCntDepth + 1) * m_fOutside;
				pVtx[nCntWidth].pos.y = 0.0f;
				pVtx[nCntWidth].pos.z = cosf(-D3DX_PI / (DIVISION_W / 2) * (nCntWidth)) * (nCntDepth + 1) * m_fOutside;

				// テクスチャ座標の設定
				pVtx[nCntWidth].tex.x = (float)nCntWidth / DIVISION_W;
				pVtx[nCntWidth].tex.y = (float)nCntDepth / (DIVISION_D);

				// 法線の設定
				pVtx[nCntWidth].nor = NORMAL;

				// 頂点カラーの設定
				pVtx[nCntWidth].col = m_colOutside;
			}
		}
	}

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//=============================
// インデックスバッファの設定
//=============================
void CMeshRing::SetIdx(void)
{
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
}