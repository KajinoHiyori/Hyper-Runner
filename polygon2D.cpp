//==========================================
// 
// ポリゴン処理[polygon2D.h]
// Author : Kajino Hiyori _Create2026/05/19~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "polygon2D.h"
#include "input.h"
#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "texture.h"

//=============================
// 静的メンバ変数宣言
//=============================
int CPolygon2D::m_nNumAll = 0;

//=============================
// コンストラクタ
//=============================
CPolygon2D::CPolygon2D()
{
	// 値のクリア
	m_nIdxTexture = -1;		// テクスチャインデックス
	m_pVtxBuff = NULL;		// 頂点バッファのクリア
	m_pos = INIT_D3DVEC3;	// 位置情報の初期化
	m_rot = INIT_D3DVEC3;	// 回転角度
	m_col = COLOR_WHITE;	// 表示する色の初期化
	m_fWidth = 0.0f;		// 幅の初期化
	m_fHeight = 0.0f;		// 縦幅の初期化
	m_fAngle = 0.0f;		// 対角線の角度
	m_fLength = 0.0f;		// 対角線の長さ
	m_type = CENTER_MIDDLE;	// 中心座標設定
	m_fTexMinX = 0.0f;			// テクスチャ読み込み幅
	m_fTexMinY = 0.0f;			// テクスチャ読み込み高さ
	m_fTexMaxX = 0.0f;			// テクスチャ読み込み幅
	m_fTexMaxY = 0.0f;			// テクスチャ読み込み高さ
	m_fWidthTop = 0.0f;		// 横幅[上端]
	m_fWidthButton = 0.0f;	// 横幅[下端]
	m_nNumAll++;			// 数字の表示数を加算
}

//=============================
// デストラクタ
//=============================
CPolygon2D::~CPolygon2D()
{

}

//=============================
// ポリゴン生成処理
//=============================
CPolygon2D* CPolygon2D::Create(D3DXVECTOR3 pos, float fWidth, float fHeight, D3DXCOLOR col, TYPE type, float fWidthButton)
{
	CPolygon2D* pPolygon2D = NULL;	// オブジェクトへのポインタを用意
	if (pPolygon2D == NULL)
	{ // NULLが設定できている場合
		pPolygon2D = new CPolygon2D;
		if (pPolygon2D != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pPolygon2D->Init(pos, fWidth, fHeight, col, type, fWidthButton);

			return pPolygon2D;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理[オーバーロード]
//=============================
HRESULT CPolygon2D::Init(D3DXVECTOR3 pos, float fWidth, float fHeight, D3DXCOLOR col, TYPE type, float fWidthButton)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 表示状態の設定
	m_pos = pos;			// 表示位置
	m_col = col;			// 色
	m_fWidth = fWidth;		// 横幅
	m_fHeight = fHeight;	// 縦幅
	m_type = type;			// 表示する数字を格納
	m_fTexMinX = 0.0f;			// テクスチャ読み込み幅
	m_fTexMinY = 0.0f;			// テクスチャ読み込み高さ	
	m_fTexMaxX = 1.0f;			// テクスチャ読み込み幅
	m_fTexMaxY = 1.0f;			// テクスチャ読み込み高さ	
	m_fWidthTop = fWidth;			// 横幅[上端]
	m_fWidthButton = fWidthButton;	// 横幅[下端]
	// 対角線の長さを算出
	m_fLength = sqrtf(fWidth * fWidth + fHeight * fHeight) * 0.5f;
	// 対角線の角度を算出
	m_fAngle = atan2f(fWidth, fWidth);

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
void CPolygon2D::Uninit(void)
{
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	m_nNumAll--;	// 表示数を減算
}

//=============================
// 更新処理
//=============================
void CPolygon2D::Update(void)
{
	SetVtx();
}

//=============================
// 描画処理
//=============================
void CPolygon2D::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャ取得
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
// 頂点バッファの更新
//=============================
void CPolygon2D::SetVtx(void)
{
	VERTEX_2D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 位置の設定
	switch (m_type)
	{
	case LEFT_TOP:		// 左揃え上寄せ
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
		break;

	case LEFT_MIDDLE:	// 左揃え中央寄せ
		pVtx[0].pos.x = m_pos.x;
		pVtx[0].pos.y = m_pos.y - m_fHeight;
		pVtx[0].pos.z = m_pos.z;
		pVtx[1].pos.x = m_pos.x + m_fWidth;
		pVtx[1].pos.y = m_pos.y - m_fHeight;
		pVtx[1].pos.z = m_pos.z;
		pVtx[2].pos.x = m_pos.x;
		pVtx[2].pos.y = m_pos.y + m_fHeight;
		pVtx[2].pos.z = m_pos.z;
		pVtx[3].pos.x = m_pos.x + m_fWidth;
		pVtx[3].pos.y = m_pos.y + m_fHeight;
		pVtx[3].pos.z = m_pos.z;
		break;

	case LEFT_BUTTON:	// 左揃え下寄せ
		pVtx[0].pos.x = m_pos.x;
		pVtx[0].pos.y = m_pos.y - m_fHeight;
		pVtx[0].pos.z = m_pos.z;
		pVtx[1].pos.x = m_pos.x + m_fWidth;
		pVtx[1].pos.y = m_pos.y - m_fHeight;
		pVtx[1].pos.z = m_pos.z;
		pVtx[2].pos.x = m_pos.x;
		pVtx[2].pos.y = m_pos.y;
		pVtx[2].pos.z = m_pos.z;
		pVtx[3].pos.x = m_pos.x + m_fWidth;
		pVtx[3].pos.y = m_pos.y;
		pVtx[3].pos.z = m_pos.z;
		break;

	case CENTER_TOP:	// 中央揃え上寄せ
		pVtx[0].pos.x = m_pos.x - m_fWidth;
		pVtx[0].pos.y = m_pos.y;
		pVtx[0].pos.z = m_pos.z;
		pVtx[1].pos.x = m_pos.x + m_fWidth;
		pVtx[1].pos.y = m_pos.y;
		pVtx[1].pos.z = m_pos.z;
		pVtx[2].pos.x = m_pos.x - m_fWidth;
		pVtx[2].pos.y = m_pos.y + m_fHeight;
		pVtx[2].pos.z = m_pos.z;
		pVtx[3].pos.x = m_pos.x + m_fWidth;
		pVtx[3].pos.y = m_pos.y + m_fHeight;
		pVtx[3].pos.z = m_pos.z;
		break;

	case CENTER_MIDDLE:	// 中央揃え中央寄せ
		pVtx[0].pos.x = m_pos.x - m_fWidth;
		pVtx[0].pos.y = m_pos.y - m_fHeight;
		pVtx[0].pos.z = m_pos.z;
		pVtx[1].pos.x = m_pos.x + m_fWidth;
		pVtx[1].pos.y = m_pos.y - m_fHeight;
		pVtx[1].pos.z = m_pos.z;
		pVtx[2].pos.x = m_pos.x - m_fWidth;
		pVtx[2].pos.y = m_pos.y + m_fHeight;
		pVtx[2].pos.z = m_pos.z;
		pVtx[3].pos.x = m_pos.x + m_fWidth;
		pVtx[3].pos.y = m_pos.y + m_fHeight;
		pVtx[3].pos.z = m_pos.z;
		break;

	case CENTER_BUTTON:	// 中央揃え下寄せ
		pVtx[0].pos.x = m_pos.x - m_fWidth;
		pVtx[0].pos.y = m_pos.y - m_fHeight;
		pVtx[0].pos.z = m_pos.z;
		pVtx[1].pos.x = m_pos.x + m_fWidth;
		pVtx[1].pos.y = m_pos.y - m_fHeight;
		pVtx[1].pos.z = m_pos.z;
		pVtx[2].pos.x = m_pos.x - m_fWidth;
		pVtx[2].pos.y = m_pos.y;
		pVtx[2].pos.z = m_pos.z;
		pVtx[3].pos.x = m_pos.x + m_fWidth;
		pVtx[3].pos.y = m_pos.y;
		pVtx[3].pos.z = m_pos.z;
		break;

	case RIGHT_TOP:	// 右揃え上寄せ
		pVtx[0].pos.x = m_pos.x - m_fWidth;
		pVtx[0].pos.y = m_pos.y;
		pVtx[0].pos.z = m_pos.z;
		pVtx[1].pos.x = m_pos.x;
		pVtx[1].pos.y = m_pos.y;
		pVtx[1].pos.z = m_pos.z;
		pVtx[2].pos.x = m_pos.x - m_fWidth;
		pVtx[2].pos.y = m_pos.y + m_fHeight;
		pVtx[2].pos.z = m_pos.z;
		pVtx[3].pos.x = m_pos.x;
		pVtx[3].pos.y = m_pos.y + m_fHeight;
		pVtx[3].pos.z = m_pos.z;
		break;

	case RIGHT_MIDDLE:	// 右揃え中央寄せ
		pVtx[0].pos.x = m_pos.x - m_fWidth;
		pVtx[0].pos.y = m_pos.y - m_fHeight;
		pVtx[0].pos.z = m_pos.z;
		pVtx[1].pos.x = m_pos.x;
		pVtx[1].pos.y = m_pos.y - m_fHeight;
		pVtx[1].pos.z = m_pos.z;
		pVtx[2].pos.x = m_pos.x - m_fWidth;
		pVtx[2].pos.y = m_pos.y + m_fHeight;
		pVtx[2].pos.z = m_pos.z;
		pVtx[3].pos.x = m_pos.x;
		pVtx[3].pos.y = m_pos.y + m_fHeight;
		pVtx[3].pos.z = m_pos.z;
		break;
		
	case RIGHT_BUTTON:	// 右揃え下寄せ
		pVtx[0].pos.x = m_pos.x - m_fWidth;
		pVtx[0].pos.y = m_pos.y - m_fHeight;
		pVtx[0].pos.z = m_pos.z;
		pVtx[1].pos.x = m_pos.x;
		pVtx[1].pos.y = m_pos.y - m_fHeight;
		pVtx[1].pos.z = m_pos.z;
		pVtx[2].pos.x = m_pos.x - m_fWidth;
		pVtx[2].pos.y = m_pos.y;
		pVtx[2].pos.z = m_pos.z;
		pVtx[3].pos.x = m_pos.x;
		pVtx[3].pos.y = m_pos.y;
		pVtx[3].pos.z = m_pos.z;
		break;

	case ROTATE:	// 回転
		pVtx[0].pos.x = m_pos.x - sinf(m_fAngle - m_rot.z) * m_fLength;
		pVtx[0].pos.y = m_pos.y - cosf(m_fAngle - m_rot.z) * m_fLength;
		pVtx[0].pos.z = m_pos.z;
		pVtx[1].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y - cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = m_pos.z;
		pVtx[2].pos.x = m_pos.x - sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = m_pos.z;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle - m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle - m_rot.z) * m_fLength;
		pVtx[3].pos.z = m_pos.z;
		break;

	case UPRIGHT:	// 上にすぼむ
		pVtx[0].pos.x = m_pos.x - m_fWidthTop;
		pVtx[0].pos.y = m_pos.y - m_fHeight;
		pVtx[0].pos.z = m_pos.z;
		pVtx[1].pos.x = m_pos.x + m_fWidthTop;
		pVtx[1].pos.y = m_pos.y - m_fHeight;
		pVtx[1].pos.z = m_pos.z;
		pVtx[2].pos.x = m_pos.x - m_fWidthButton;
		pVtx[2].pos.y = m_pos.y + m_fHeight;
		pVtx[2].pos.z = m_pos.z;
		pVtx[3].pos.x = m_pos.x + m_fWidthButton;
		pVtx[3].pos.y = m_pos.y + m_fHeight;
		pVtx[3].pos.z = m_pos.z;
		break;
	}

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
	pVtx[0].tex = D3DXVECTOR2(m_fTexMinX, m_fTexMinY);
	pVtx[1].tex = D3DXVECTOR2(m_fTexMaxX, m_fTexMinY);
	pVtx[2].tex = D3DXVECTOR2(m_fTexMinX, m_fTexMaxY);
	pVtx[3].tex = D3DXVECTOR2(m_fTexMaxX, m_fTexMaxY);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}