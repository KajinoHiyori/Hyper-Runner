//========================================================
// 
// ビルボード処理[objectBillboardAnim.cpp]
// Author : Kajino Hiyori _Create2026/06/01
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "objectBillboardAnim.h"
#include "camera.h"
#include "input.h"
#include "color.h"
#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "debugproc.h"

//=============================
// マクロ定義
//=============================
#define WIDTH	(10.0f)								// 幅
#define DEPTH	(10.0f)								// 奥行
#define HEIGHT	(10.0f)								// 高さ
#define SHADOW	(10.0f)								// 影の大きさ
#define NORMAL	(D3DXVECTOR3(0.0f, 0.0f, -1.0f))	// 基本の法線
#define POS		(D3DXVECTOR3(0.0f, 0.0f, 0.0f))		// 位置
#define DEFAULT	(D3DXVECTOR3(0.0f, 0.0f, 0.0f))		// デフォルト
#define MOVE	(1.5f)								// 移動量
#define ROTATE	(0.05f)								// 回転量

//=============================
// 静的メンバ変数宣言
//=============================
int CBillboardAnim::m_nNumAll = 0;	// 総数のクリア

//=============================
// コンストラクタ
//=============================
CBillboardAnim::CBillboardAnim(CObject::PRIORITY priority) : CObject(priority)
{
	m_pos = INIT_D3DVEC3;		// ビルボードの位置
	m_col = COLOR_WHITE;	// 色
	m_nIdxTexture = 0;		// テクスチャへのポインタ
	m_pVtxBuff = NULL;	// 頂点バッファへのポインタ
	m_fWidth = 0.0f;	// 幅
	m_fHeight = 0.0f;	// 高さ
	m_bDisp = false;
	m_nNumAll++;	// 総数の加算
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

//=============================
// デストラクタ
//=============================
CBillboardAnim::~CBillboardAnim()
{

}

//=============================
// ビルボードの生成
//=============================
CBillboardAnim* CBillboardAnim::Create(D3DXVECTOR3 pos, float fWidth, float fHeight, bool bDisp)
{
	CBillboardAnim* pBillboardAnim = NULL;	// オブジェクトへのポインタを用意
	if (pBillboardAnim == NULL)
	{ // NULLが設定できている場合
		pBillboardAnim = new CBillboardAnim;
		if (pBillboardAnim != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pBillboardAnim->Init(pos, fWidth, fHeight, bDisp);
			// 位置設定
			pBillboardAnim->SetPosition(pos);

			return pBillboardAnim;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// ビルボードの初期化処理
//=============================
HRESULT CBillboardAnim::Init(D3DXVECTOR3 pos, float fWidth, float fHeight, bool bDisp)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 初期化
	m_pos = pos;
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	m_fHeight = fHeight;
	m_bDisp = bDisp;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,									// 頂点フォーマット
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点情報を設定
	SetVtx();

	return S_OK;
}

//=============================
// ビルボードの終了処理
//=============================
void CBillboardAnim::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);
	m_nNumAll--;	// 総数の減算
}

//=============================
// ビルボードの更新処理
//=============================
void CBillboardAnim::Update(void)
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
			if (m_nPatternAnim >= m_nMaxAnim)
			{
				m_bEnd = true;
				CObject::Release();	// 自身を破棄
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

//=============================
// ビルボードの描画処理
//=============================
void CBillboardAnim::Draw(void)
{
	if (m_bDisp == false)
	{ // 非表示の場合早期リターン
		return;
	}

	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();
	// テクスチャの取得
	CTexture* pTextere = CManager::GetTexture();

	D3DXMATRIX mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;		// ビューマトリックスの取得

	// ワールドマトリックスの初期化(デフォルトの値にする)
	D3DXMatrixIdentity(&m_mtxWorld);

	// ビューマトリックスを取得する
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// ポリゴンをカメラに対して正面に向ける
	D3DXMatrixInverse(&m_mtxWorld, NULL, &mtxView);	// 逆行列を求める
	m_mtxWorld._41 = 0.0f;
	m_mtxWorld._42 = 0.0f;
	m_mtxWorld._43 = 0.0f;

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
	pDevice->SetTexture(0, pTextere->GetAddress(m_nIdxTexture));

	// ビルボードの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//=============================
// テクスチャインデックスを指定
//=============================
void CBillboardAnim::SetTextureIdx(int nIdx)
{
	// テクスチャの設定
	m_nIdxTexture = nIdx;
}

//=============================
// アニメーション情報の設定
//=============================
void CBillboardAnim::SetAnimInfo(float fWidth, float fHeight, int nFrame, int nMaxAnim, float fTexW, float fTexH, bool bLoop)
{
	m_fWidth = fWidth;		// 横幅
	m_fHeight = fHeight;	// 縦幅
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


//=============================
// 頂点情報を設定
//=============================
void CBillboardAnim::SetVtx(void)
{
	VERTEX_3D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(-m_fWidth, m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_fWidth, m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_fWidth, -m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_fWidth, -m_fHeight, 0.0f);

	// rhwの設定
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
	pVtx[0].tex = D3DXVECTOR2(m_nPatternAnim * m_fTexWidth, m_fTexHStart);
	pVtx[1].tex = D3DXVECTOR2(m_nPatternAnim * m_fTexWidth + m_fTexWidth, m_fTexHStart);
	pVtx[2].tex = D3DXVECTOR2(m_nPatternAnim * m_fTexWidth, m_fTexHStart + m_fTexHeight);
	pVtx[3].tex = D3DXVECTOR2(m_nPatternAnim * m_fTexWidth + m_fTexWidth, m_fTexHStart + m_fTexHeight);


	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}