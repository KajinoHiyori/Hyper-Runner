//========================================================
// 
// ビルボード処理[objectBillboard.cpp]
// Author : Kajino Hiyori _Create2026/06/01
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "objectBillboard.h"
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
int CBillboard::m_nNumAll = 0;	// 総数のクリア

//=============================
// コンストラクタ
//=============================
CBillboard::CBillboard(CObject::PRIORITY priority) : CObject(priority)
{
	m_pos = INIT_D3DVEC3;		// ビルボードの位置
	m_col = COLOR_WHITE;	// 色
	m_nIdxTexture = 0;		// テクスチャへのポインタ
	m_pVtxBuff = NULL;	// 頂点バッファへのポインタ
	m_fWidth = 0.0f;	// 幅
	m_fHeight = 0.0f;	// 高さ
	m_type = CENTER_MIDDLE;	// アンカーポイント位置
	m_nNumAll++;	// 総数の加算
}

//=============================
// デストラクタ
//=============================
CBillboard::~CBillboard()
{

}

//=============================
// ビルボードの生成
//=============================
CBillboard* CBillboard::Create(D3DXVECTOR3 pos, float fWidth, float fHeight, TYPE type)
{
	CBillboard* pBillboard = NULL;	// オブジェクトへのポインタを用意
	if (pBillboard == NULL)
	{ // NULLが設定できている場合
		pBillboard = new CBillboard;
		if (pBillboard != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pBillboard->Init(pos, fWidth, fHeight, type);
			// 位置設定
			pBillboard->SetPosition(pos);

			return pBillboard;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// ビルボードの初期化処理
//=============================
HRESULT CBillboard::Init(D3DXVECTOR3 pos, float fWidth, float fHeight, TYPE type)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 初期化
	m_pos = pos;
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	m_fHeight = fHeight;
	m_type = type;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,									// 頂点フォーマット
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点情報を設定
	SetVtxBuff();

	return S_OK;
}

//=============================
// ビルボードの終了処理
//=============================
void CBillboard::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);
	m_nNumAll--;	// 総数の減算
}

//=============================
// ビルボードの更新処理
//=============================
void CBillboard::Update(void)
{
}

//=============================
// ビルボードの描画処理
//=============================
void CBillboard::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();
	// テクスチャの取得
	CTexture* pTextere = CManager::GetTexture();

	D3DXMATRIX mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;		// ビューマトリックスの取得

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);	// Zテストの比較方法を変更(Zバッファの前後関係に関わらず描画する)
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);		// Zバッファ更新の有効/無効の設定

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// 比較方法を設定(基準値より大きい場合描画)
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);	// アルファテストの参照値を設定(この場合、0より大きい場合は描画)

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

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// 比較方法を設定(条件に関わらず描画)
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);	// アルファテストの参照値を設定(この場合、0より大きい場合は描画)
}

//=============================
// テクスチャインデックスを指定
//=============================
void CBillboard::SetTextureIdx(int nIdx)
{
	// テクスチャの設定
	m_nIdxTexture = nIdx;
}

//=============================
// 頂点情報を設定
//=============================
void CBillboard::SetVtxBuff(void)
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
		pVtx[2].pos = D3DXVECTOR3(0.0f, -m_fHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(m_fWidth, -m_fHeight, 0.0f);
		break;

	case LEFT_MIDDLE:	// 左揃え中央寄せ
		pVtx[0].pos = D3DXVECTOR3(0.0f, m_fHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(m_fWidth, m_fHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(0.0f, -m_fHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(m_fWidth, -m_fHeight, 0.0f);
		break;

	case LEFT_BUTTON:	// 左揃え下寄せ
		pVtx[0].pos = D3DXVECTOR3(0.0f, m_fHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(m_fWidth, m_fHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(m_fWidth, 0.0f, 0.0f);
		break;

	case CENTER_TOP:	// 中央揃え上寄せ
		pVtx[0].pos = D3DXVECTOR3(-m_fWidth, 0.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(m_fWidth, 0.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-m_fWidth, -m_fHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(m_fWidth, -m_fHeight, 0.0f);
		break;

	case CENTER_MIDDLE:	// 中央揃え中央寄せ
		pVtx[0].pos = D3DXVECTOR3(-m_fWidth, m_fHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(m_fWidth, m_fHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-m_fWidth, -m_fHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(m_fWidth, -m_fHeight, 0.0f);
		break;

	case CENTER_BUTTON:	// 中央揃え下寄せ
		pVtx[0].pos = D3DXVECTOR3(-m_fWidth, m_fHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(m_fWidth, m_fHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-m_fWidth, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(m_fWidth, 0.0f, 0.0f);
		break;

	case RIGHT_TOP:	// 右揃え上寄せ
		pVtx[0].pos = D3DXVECTOR3(-m_fWidth, 0.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-m_fWidth, -m_fHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(0.0f, -m_fHeight, 0.0f);
		break;

	case RIGHT_MIDDLE:	// 右揃え中央寄せ
		pVtx[0].pos = D3DXVECTOR3(-m_fWidth, m_fHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(0.0f, m_fHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-m_fWidth, -m_fHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(0.0f, -m_fHeight, 0.0f);
		break;

	case RIGHT_BUTTON:	// 右揃え下寄せ
		pVtx[0].pos = D3DXVECTOR3(-m_fWidth, m_fHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(0.0f, m_fHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-m_fWidth, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		break;
	}

	// norの設定
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
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}