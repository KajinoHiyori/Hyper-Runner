//==========================================
// 
// 数値処理[number2D.h]
// Author : Kajino Hiyori _Create2026/05/19~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "number2D.h"
#include "input.h"
#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "texture.h"

//=============================
// 静的メンバ変数宣言
//=============================
const char* CNumber2D::m_pFilenameType =
{
	"data\\TEXTURE\\number.png",
};
int CNumber2D::m_nNumAll = 0;

//=============================
// コンストラクタ
//=============================
CNumber2D::CNumber2D()
{
	// 値のクリア
	m_nIdxTexture = 0;
	m_pVtxBuff = NULL;	// 頂点バッファのクリア
	m_nNum = 0;	// 表示する数字
	m_pos = INIT_D3DVEC3;	// 位置情報の初期化
	m_col = COLOR_WHITE;	// 表示する色の初期化
	m_fWidth = 0.0f;	// 幅の初期化
	m_fHeight = 0.0f;	// 縦幅の初期化
	m_nNumAll++;	// 数字の表示数を加算
}

//=============================
// デストラクタ
//=============================
CNumber2D::~CNumber2D()
{

}

//=============================
// 数値生成処理
//=============================
CNumber2D* CNumber2D::Create(D3DXVECTOR3 pos, int nNum)
{
	CNumber2D* pNumber2D = NULL;	// オブジェクトへのポインタを用意
	if (pNumber2D == NULL)
	{ // NULLが設定できている場合
		pNumber2D = new CNumber2D;
		if (pNumber2D != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pNumber2D->Init(pos, nNum);

			// 位置設定
			pNumber2D->SetPosition(pos);

			return pNumber2D;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 数値生成処理
//=============================
CNumber2D* CNumber2D::Create(D3DXVECTOR3 pos, int nNum, float fSize)
{
	CNumber2D* pNumber2D = NULL;	// オブジェクトへのポインタを用意
	if (pNumber2D == NULL)
	{ // NULLが設定できている場合
		pNumber2D = new CNumber2D;
		if (pNumber2D != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pNumber2D->Init(pos, nNum, fSize);

			// 位置設定
			pNumber2D->SetPosition(pos);

			return pNumber2D;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CNumber2D::Init(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャの設定
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register(m_pFilenameType);

	// 表示状態の設定
	m_pos = INIT_D3DVEC3;	// 表示位置
	m_col = COLOR_WHITE;	// 色
	m_fWidth = NUMBER_WIDTH;		// 横幅
	m_fHeight = NUMBER_HEIGHT;		// 縦幅
	m_nNum = 0;			// 表示する数字を格納

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);

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

//=============================
// 初期化処理[オーバーロード]
//=============================
HRESULT CNumber2D::Init(D3DXVECTOR3 pos, int nNum)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャの設定
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register(m_pFilenameType);

	// 表示状態の設定
	m_pos = pos;	// 表示位置
	m_col = COLOR_WHITE;	// 色
	m_fWidth = NUMBER_WIDTH;		// 横幅
	m_fHeight = NUMBER_HEIGHT;		// 縦幅
	m_nNum = nNum;			// 表示する数字を格納

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);

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

//=============================
// 初期化処理[オーバーロード]
//=============================
HRESULT CNumber2D::Init(D3DXVECTOR3 pos, int nNum, float fSize)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャの設定
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register(m_pFilenameType);

	// 表示状態の設定
	m_pos = pos;	// 表示位置
	m_col = COLOR_WHITE;	// 色
	m_fWidth = fSize;		// 横幅
	m_fHeight = fSize;		// 縦幅
	m_nNum = nNum;			// 表示する数字を格納

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);

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

//=============================
// 終了処理
//=============================
void CNumber2D::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	m_nNumAll--;	// 表示数を減算
}

//=============================
// 更新処理
//=============================
void CNumber2D::Update(void)
{
	SetVtx();
}

//=============================
// 描画処理
//=============================
void CNumber2D::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャの取得
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

//=============================
// 位置の設定
//=============================
void CNumber2D::SetPosition(D3DXVECTOR3 pos)
{
	// 位置情報の代入
	m_pos = pos;

	// 頂点バッファの更新
	SetVtx();
}


//=============================
// 頂点バッファの更新
//=============================
void CNumber2D::SetVtx(void)
{
	VERTEX_2D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 位置の設定
	pVtx[0].pos.x = m_pos.x;
	pVtx[0].pos.y = m_pos.y;
	pVtx[0].pos.z = m_pos.z;
	pVtx[1].pos.x = m_pos.x + m_fWidth;
	pVtx[1].pos.y = m_pos.y;
	pVtx[1].pos.z = m_pos.z;
	pVtx[2].pos.x = m_pos.x;
	pVtx[2].pos.y = m_pos.y + m_fHeight;
	pVtx[2].pos.z = m_pos.z;
	pVtx[3].pos.x = m_pos.x + m_fWidth;
	pVtx[3].pos.y = m_pos.y + m_fHeight;
	pVtx[3].pos.z = m_pos.z;

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 色の設定
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
// 数値更新
//=============================
void CNumber2D::SetNum(int nNum)
{
	// 表示する数字を変更
	m_nNum = nNum;

	// 頂点バッファの更新
	SetVtx();
}

//=============================
// 数値設定
//=============================
int CNumber2D::SetNumberTecU(int nDigit, int nSource, int nCnt)
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
