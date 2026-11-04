//==========================================
// 
// 2Dテクスチャアニメーション処理[object2D.cpp]
// Author : Kajino Hiyori _Create2026/05/08~
// 
//==========================================
#include "object2DAnim.h"
#include "renderer.h"
#include "manager.h"
#include "debugproc.h"
#include "input.h"

//=============================
// マクロ定義
//=============================
#define POLYGON_SIZE	(50.0f)	// ポリゴンのサイズ
#define RUNNING_FRAME	(5)			// ランニングマンフレーム数
#define RUNNING_ANIM	(10)		// ランニングマンコマ数

//========================================================================
// オブジェクト生成
//========================================================================
CObject2DAnim* CObject2DAnim::Create(D3DXVECTOR3 pos, D3DXCOLOR col)
{
	CObject2DAnim* pObject2DAnim = NULL;	// オブジェクトへのポインタを用意
	if (pObject2DAnim == NULL)
	{ // NULLが設定できている場合
		pObject2DAnim = new CObject2DAnim;
		if (pObject2DAnim != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pObject2DAnim->Init(pos, col);
			// 位置設定
			pObject2DAnim->SetPosition(pos);

			return pObject2DAnim;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//========================================================================
// コンストラクタ
//========================================================================
CObject2DAnim::CObject2DAnim(CObject::PRIORITY priority):CObject(priority)
{
	m_pos = INIT_D3DVEC3;	// 表示位置
	m_rot = INIT_D3DVEC3;	// 向き
	m_col = COLOR_WHITE;	// 色
	m_fWidth = 0.0f;	// 横幅
	m_fHeight = 0.0f;	// 縦幅
	m_fLength = 0.0f;	// 対角線の長さ
	m_fAngle = 0.0f;	// 対角線の長さ
	m_pVtxBuff = NULL;		// 頂点バッファ	
	m_pTexture = NULL;			// テクスチャへのポインタ
	m_nCounterAnim = 0;	// アニメーションカウンター
	m_nPatternAnim = 0;	// パターンナンバー
	m_nFrame = 0;		// パターンナンバーを加算するまでのフレーム数
	m_nMaxAnim = 0;		// 最大コマ数
	m_fTexHStart = 0.0f;	// テクスチャ読み込み開始位置[上下段がある場合]
	m_fTexWidth = 0.0f;	// テクスチャの読み込み幅
	m_fTexHeight = 0.0f;	// テクスチャの読み込み高さ
	m_bLoop = false;		// 繰り返し管理[true : 繰り返し有 / false : 繰り返し無]
	m_bEnd = false;			// 終了状態管理
}

//========================================================================
// デストラクタ
//========================================================================
CObject2DAnim::~CObject2DAnim()
{

}

//========================================================================
// 初期化処理
//========================================================================
HRESULT CObject2DAnim::Init(void)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
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
HRESULT CObject2DAnim::Init(D3DXVECTOR3 pos, D3DXCOLOR col)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 表示状態の設定
	// 表示状態の設定
	m_pos = pos;			// 表示位置
	m_rot = INIT_D3DVEC3;	// 向き
	m_col = col;			// 色
	m_fWidth = 0.0f;		// 横幅
	m_fHeight = 0.0f;		// 縦幅
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
void CObject2DAnim::Uninit(void)
{
	m_pTexture = NULL;	// テクスチャへのポインタにNULLを設定

	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);
}

//========================================================================
// 更新処理
//========================================================================
void CObject2DAnim::Update(void)
{
	// アニメーション実装
	m_nCounterAnim++;	// アニメーションカウンターを加算
	if (m_nCounterAnim > m_nFrame)
	{ // 規定値を越えた
		m_nCounterAnim = 0;	// カウンターの初期化
		m_nPatternAnim++;	// パターンナンバーを更新
		if (m_bLoop == true)
		{ // アニメーションが繰り返される
			m_nPatternAnim = m_nPatternAnim % m_nMaxAnim;	// パターンナンバーを正規化
		}
		else if (m_bLoop == false)
		{ // アニメーションが繰り返されない
			if (m_nPatternAnim > m_nMaxAnim)
			{
				m_bEnd = true;
			}
		}

		if (m_nPatternAnim > m_nMaxAnim / 2)
		{ // 上段の読み込みから下段に移行
			m_fTexHStart = m_fTexHeight;
		}
		else
		{ // 下段読み込みから上段に移行
			m_fTexHStart = 0.0f;
		}
	}
	// 表示位置更新
	SetVtx();
}

//========================================================================
// 描画処理
//========================================================================
void CObject2DAnim::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, m_pTexture);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//========================================================================
// アニメーション情報の設定
//========================================================================
void CObject2DAnim::SetAnimInfo(float fWidth, float fHeight, int nFrame, int nMaxAnim, float fTexW, float fTexH, bool bLoop)
{
	m_fWidth = fWidth;		// 横幅
	m_fHeight = fHeight;	// 縦幅
	m_fLength = sqrtf(m_fWidth * m_fWidth + m_fHeight * m_fHeight) * 0.5f;	// 対角線の長さ
	m_fAngle = atan2f(m_fWidth, m_fHeight);	// 対角線の長さ
	m_nFrame = nFrame;		// パターンナンバーを加算するまでのフレーム数
	m_nMaxAnim = nMaxAnim;	// 最大コマ数
	m_fTexHStart = 0.0f;	// テクスチャ読み込み開始位置[上下段がある場合]
	m_fTexWidth = fTexW;	// テクスチャの読み込み幅
	m_fTexHeight = fTexH;	// テクスチャの読み込み高さ
	m_bLoop = bLoop;		// 繰り返し管理
	m_bEnd = false;			// 終了状態管理

	// 描画情報更新処理
	SetVtx();
}

//========================================================================
// 描画位置情報更新
//========================================================================
void CObject2DAnim::SetVtx(void)
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
	pVtx[0].tex = D3DXVECTOR2(m_nPatternAnim * m_fTexWidth, m_fTexHStart);
	pVtx[1].tex = D3DXVECTOR2(m_nPatternAnim * m_fTexWidth + m_fTexWidth, m_fTexHStart);
	pVtx[2].tex = D3DXVECTOR2(m_nPatternAnim * m_fTexWidth, m_fTexHStart + m_fTexHeight);
	pVtx[3].tex = D3DXVECTOR2(m_nPatternAnim * m_fTexWidth + m_fTexWidth, m_fTexHStart + m_fTexHeight);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//========================================================================
// 座標更新
//========================================================================
void CObject2DAnim::SetPosition(D3DXVECTOR3 pos)
{
	m_pos = pos;

	// 頂点位置の更新
	SetVtx();
}
