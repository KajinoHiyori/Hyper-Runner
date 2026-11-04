//==========================================
// 
// 床と平行なポリゴン処理[polygonField.cpp]
// Author : Kajino Hiyori _Create2026/09/03
// 
//==========================================
#include "main.h"
#include "input.h"
#include "color.h"
#include "polygonField.h"
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
// コンストラクタ
//=============================
CPolygonField::CPolygonField()
{
	m_pos = INIT_D3DVEC3;	// 表示位置
	m_rot = INIT_D3DVEC3;	// 向き
	m_col = COLOR_WHITE;	// 色
	m_fWidth = 0.0f;		// 横幅
	m_fDepth = 0.0f;		// 奥行き
	m_pVtxBuff = NULL;		// 頂点バッファ	
	m_nIdxTexture = 0;		// テクスチャのインデックス
	m_type = CENTER_MIDDLE;	// アンカーポイントの初期化
	m_fTexMinX = 0.0f;		// テクスチャ読み込み幅[最大値]
	m_fTexMinY = 0.0f;		// テクスチャ読み込み高さ[最小値]
	m_fTexMaxX = 0.0f;		// テクスチャ読み込み幅[最大値]
	m_fTexMaxY = 0.0f;		// テクスチャ読み込み高さ[最小値]
}

//=============================
// デストラクタ
//=============================
CPolygonField::~CPolygonField()
{

}

//=============================
// 床と平行なポリゴン生成
//=============================
CPolygonField* CPolygonField::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float fDepth, TYPE type)
{
	CPolygonField* pPolygonField = NULL;	// ポリゴン3Dへのポインタを用意
	if (pPolygonField == NULL)
	{ // NULLが設定できている場合
		pPolygonField = new CPolygonField;
		if (pPolygonField != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pPolygonField->Init(pos, rot, col, fWidth, fDepth, type);
			// 位置設定
			pPolygonField->SetPosition(pos);

			return pPolygonField;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CPolygonField::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float fDepth, TYPE type)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャの設定
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = -1;

	// 初期化
	m_pos = pos;
	m_rot = rot;
	m_fWidth = fWidth;		// 横幅
	m_fDepth = fDepth;		// 奥行き
	m_col = col;		// 色
	m_type = type;		// アンカーポイント

	m_fTexMinX = 0.0f;		// テクスチャ読み込み幅[最大値]
	m_fTexMinY = 0.0f;		// テクスチャ読み込み高さ[最小値]
	m_fTexMaxX = 1.0f;		// テクスチャ読み込み幅[最大値]
	m_fTexMaxY = 1.0f;		// テクスチャ読み込み高さ[最小値]

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
void CPolygonField::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);
}

//=============================
// ポリゴンの更新処理
//=============================
void CPolygonField::Update(void)
{
}

//=============================
// ポリゴンの描画処理
//=============================
void CPolygonField::Draw(D3DXMATRIX mtxParent)
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
// ポリゴンの描画処理
//=============================
void CPolygonField::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス

	// ライトをオフにする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

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

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// ライトをオンにする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//=============================
// 頂点情報の更新
//=============================
void CPolygonField::SetVtx(void)
{
	VERTEX_3D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定[アンカーポイントに応じて変更]
	switch (m_type)
	{
	case LEFT_TOP:		// 左揃え上寄せ
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(m_fWidth, 0.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, -m_fDepth);
		pVtx[3].pos = D3DXVECTOR3(m_fWidth, 0.0f, -m_fDepth);
		break;

	case LEFT_MIDDLE:	// 左揃え中央寄せ
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, m_fDepth);
		pVtx[1].pos = D3DXVECTOR3(m_fWidth, 0.0f, m_fDepth);
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, -m_fDepth);
		pVtx[3].pos = D3DXVECTOR3(m_fWidth, 0.0f, -m_fDepth);
		break;

	case LEFT_BUTTON:	// 左揃え下寄せ
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, m_fDepth);
		pVtx[1].pos = D3DXVECTOR3(m_fWidth, 0.0f, m_fDepth);
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(m_fWidth, 0.0f, 0.0f);
		break;

	case CENTER_TOP:	// 中央揃え上寄せ
		pVtx[0].pos = D3DXVECTOR3(-m_fWidth, 0.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(m_fWidth, 0.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-m_fWidth, 0.0f, -m_fDepth);
		pVtx[3].pos = D3DXVECTOR3(m_fWidth, 0.0f, -m_fDepth);
		break;

	case CENTER_MIDDLE:	// 中央揃え中央寄せ
		pVtx[0].pos = D3DXVECTOR3(-m_fWidth, 0.0f, m_fDepth);
		pVtx[1].pos = D3DXVECTOR3(m_fWidth, 0.0f, m_fDepth);
		pVtx[2].pos = D3DXVECTOR3(-m_fWidth, 0.0f, -m_fDepth);
		pVtx[3].pos = D3DXVECTOR3(m_fWidth, 0.0f, -m_fDepth);
		break;

	case CENTER_BUTTON:	// 中央揃え下寄せ
		pVtx[0].pos = D3DXVECTOR3(m_fWidth, 0.0f, m_fDepth);
		pVtx[1].pos = D3DXVECTOR3(-m_fWidth, 0.0f, m_fDepth);
		pVtx[2].pos = D3DXVECTOR3(m_fWidth, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(-m_fWidth, 0.0f, 0.0f);
		break;

	case RIGHT_TOP:	// 右揃え上寄せ
		pVtx[0].pos = D3DXVECTOR3(-m_fWidth, 0.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-m_fWidth, 0.0f, -m_fDepth);
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, -m_fDepth);
		break;

	case RIGHT_MIDDLE:	// 右揃え中央寄せ
		pVtx[0].pos = D3DXVECTOR3(-m_fWidth, 0.0f, m_fDepth);
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, m_fDepth);
		pVtx[2].pos = D3DXVECTOR3(-m_fWidth, 0.0f, -m_fDepth);
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, -m_fDepth);
		break;

	case RIGHT_BUTTON:	// 右揃え下寄せ
		pVtx[0].pos = D3DXVECTOR3(-m_fWidth, 0.0f, m_fDepth);
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, m_fDepth);
		pVtx[2].pos = D3DXVECTOR3(-m_fWidth, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
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
	pVtx[0].tex = D3DXVECTOR2(m_fTexMinX, m_fTexMinY);
	pVtx[1].tex = D3DXVECTOR2(m_fTexMaxX, m_fTexMinY);
	pVtx[2].tex = D3DXVECTOR2(m_fTexMinX, m_fTexMaxY);
	pVtx[3].tex = D3DXVECTOR2(m_fTexMaxX, m_fTexMaxY);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//=============================
// 頂点色の更新
//=============================
void CPolygonField::SetVtxColor(void)
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