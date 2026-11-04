//==========================================
// 
// 2Dオブジェクト処理[object2D.cpp]
// Author : Kajino Hiyori _Create2026/05/08~
// 
//==========================================
#include "object2D.h"
#include "renderer.h"
#include "manager.h"
#include "debugproc.h"
#include "input.h"
#include "texture.h"

//=============================
// マクロ定義
//=============================
#define POLYGON_SIZE	(50.0f)	// ポリゴンのサイズ

//========================================================================
// コンストラクタ
//========================================================================
CObject2D::CObject2D(CObject::PRIORITY priority) : CObject(priority)
{
	m_pos = INIT_D3DVEC3;			// 表示位置
	m_rot = INIT_D3DVEC3;			// 向き
	m_col = COLOR_WHITE;			// 色
	m_fWidth = 0.0f;				// 横幅
	m_fHeight = 0.0f;				// 縦幅
	m_fLength = 0.0f;				// 対角線の長さ
	m_fAngle = 0.0f;				// 対角線の長さ
	m_nIdxTexture = -1;				// テクスチャインデックス
	m_pVtxBuff = NULL;				// 頂点バッファ	
	m_pTexture = NULL;				// テクスチャへのポインタ
}

//========================================================================
// デストラクタ
//========================================================================
CObject2D::~CObject2D()
{

}

//========================================================================
// オブジェクト生成
//========================================================================
CObject2D* CObject2D::Create(D3DXVECTOR3 pos, float fHeight, float fWidth)
{
	CObject2D* pObject2D = NULL;	// オブジェクトへのポインタを用意
	if (pObject2D == NULL)
	{ // NULLが設定できている場合
		pObject2D = new CObject2D;
		if (pObject2D != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pObject2D->Init(pos, fHeight, fWidth);
			// 位置設定
			pObject2D->SetPosition(pos);

			return pObject2D;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//========================================================================
// 初期化処理
//========================================================================
HRESULT CObject2D::Init(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 表示状態の設定
	m_pos = INIT_D3DVEC3;			// 表示位置
	m_rot = INIT_D3DVEC3;			// 向き
	m_col = COLOR_WHITE;			// 色
	m_fWidth = 0.0f;				// 横幅
	m_fHeight = 0.0f;				// 縦幅
	m_fLength = 0.0f;				// 対角線の長さ
	m_fAngle = 0.0f;				// 対角線の長さ

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 表示位置更新
	SetVtx();

	if (m_pVtxBuff != NULL)
	{
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

//========================================================================
// 初期化処理[オーバーロード]
//========================================================================
HRESULT CObject2D::Init(D3DXVECTOR3 pos, float fHeight, float fWidth)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 表示状態の設定
	// 表示状態の設定
	m_pos = pos;					// 表示位置
	m_rot = INIT_D3DVEC3;			// 向き
	m_col = COLOR_WHITE;			// 色
	m_fWidth = fWidth;				// 横幅
	m_fHeight = fHeight;			// 縦幅
	m_fLength = sqrtf(m_fWidth * m_fWidth + m_fHeight * m_fHeight) * 0.5f;	// 対角線の長さ
	m_fAngle = atan2f(m_fWidth, m_fHeight);				// 対角線の長さ

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);

	// 表示位置更新
	SetVtx();

	if (m_pTexture != NULL && m_pVtxBuff != NULL)
	{
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

//========================================================================
// 終了処理
//========================================================================
void CObject2D::Uninit(void)
{
	m_pTexture = NULL;	// テクスチャへのポインタにNULLを設定

	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);
}

//========================================================================
// 更新処理
//========================================================================
void CObject2D::Update(void)
{
	
}

//========================================================================
// 描画処理
//========================================================================
void CObject2D::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャ情報取得
	CTexture* pTexture = CManager::GetTexture();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//========================================================================
// テクスチャ設定
//========================================================================
void CObject2D::SetTexture(const char* pFileName)
{
	// テクスチャ情報取得
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register(pFileName);
}

//========================================================================
// 描画位置情報更新
//========================================================================
void CObject2D::SetVtx(void)
{
	// 表示位置の更新
	VERTEX_2D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos.x = m_pos.x - m_fWidth;
	pVtx[0].pos.y = m_pos.y - m_fHeight;
	pVtx[0].pos.z = 0.0f;
	pVtx[1].pos.x = m_pos.x + m_fWidth;
	pVtx[1].pos.y = m_pos.y - m_fHeight;
	pVtx[1].pos.z = 0.0f;
	pVtx[2].pos.x = m_pos.x - m_fWidth;
	pVtx[2].pos.y = m_pos.y + m_fHeight;
	pVtx[2].pos.z = 0.0f;
	pVtx[3].pos.x = m_pos.x + m_fWidth;
	pVtx[3].pos.y = m_pos.y + m_fHeight;
	pVtx[3].pos.z = 0.0f;

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//========================================================================
// 座標更新
//========================================================================
void CObject2D::SetPosition(D3DXVECTOR3 pos)
{
	m_pos = pos;

	// 頂点位置の更新
	SetVtx();
}

//========================================================================
// 角度更新
//========================================================================
void CObject2D::SetRotation(D3DXVECTOR3 rot)
{
	m_rot = rot;

	// 頂点位置の更新
	SetVtx();
}

//========================================================================
// 色更新
//========================================================================
void CObject2D::SetColor(D3DXCOLOR col)
{
	m_col = col;

	// 頂点情報の更新
	SetVtx();
}
