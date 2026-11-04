//========================================================
// 
// オブジェクトX処理[objectX.cpp]
// Author : Kajino Hiyori _Create2026/06/01
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "objectX.h"
#include "camera.h"
#include "input.h"
#include "color.h"
#include "debugproc.h"
#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "fileX.h"

//=============================
// マクロ定義
//=============================
#define MOVE			(0.1f)				// 移動量
#define JUMP			(20.0f)				// ジャンプ
#define ROTATE			(0.05f)				// 回転量
#define SHOT			(-10.0f)			// 発射位置
#define SHADOW			(10.0f)				// 影の大きさ
#define CORRECTION_ROT	(0.2f)				// 回転の減衰係数
#define RIGHT			(D3DX_PI / 2)					// 右を向く
#define LEFT			(-(D3DX_PI / 2))				// 左を向く
#define BACK			(D3DX_PI)						// 後ろを向く
#define FRONT			(0.0f)							// 正面を向く
#define RIGHT_BACK		(D3DX_PI / 4 + D3DX_PI / 2)		// 右後ろ
#define RIGHT_FRONT		(D3DX_PI / 4)					// 右手前
#define LEFT_BACK		(-(D3DX_PI / 4 + D3DX_PI / 2))	// 左後ろ
#define LEFT_FRONT		(-(D3DX_PI / 4))				// 左手前
#define LENGTH			(191.0f)		// フィールドの範囲
#define WING			(8.0f)			// 飛行機の羽根の幅
#define HIGHT			(2.0f)			// 飛行機の羽根の高さ
#define DEPTH			(2.0f)			// 飛行機の羽根の末端
#define SIZE			(3.0f)			// エフェクトのサイズ
#define LIFE			(20)			// エフェクトの寿命
#define GRAVITY			(0.5f)			// 重力
#define POS				(D3DXVECTOR3(-100.0f, 5.0f, 0.0f))				// プレイヤーの位置
#define NORMAL			(D3DXVECTOR3(0.0f, 1.0f, 0.0f))					// 基本の法線
#define VTX_MIN			(D3DXVECTOR3(10000.0f, 10000.0f, 10000.0f))		// プレイヤーの大きさの初期化値(最小)
#define VTX_MAX			(D3DXVECTOR3(-10000.0f, -10000.0f, -10000.0f))	// プレイヤーの大きさの初期化値(最大)
#define NORMAL_FIELD	(D3DXVECTOR3(0.0f, 1.0f, 0.0f))	// 平面情報

//=============================
// 静的メンバ変数宣言
//=============================
int CObjectX::m_nNumAll = 0;	// 総数のクリア

//=============================
// コンストラクタ
//=============================
CObjectX::CObjectX(CObject::PRIORITY priority) : CObject(priority)
{
	m_colType = COLORTYPE_DEFAULT;
	m_pos = INIT_D3DVEC3;		// モデルの位置
	m_rot = INIT_D3DVEC3;		// モデルの向き
	m_col = COLOR_WHITE;		// モデルの色
	m_vtxMin = INIT_D3DVEC3;	// モデルの大きさ(最小)
	m_vtxMax = INIT_D3DVEC3;	// モデルの大きさ(最大)
#ifdef ENABLE_FILEX
	m_nIdx = -1;		// Xファイルインデックス
#else
	m_pMesh = NULL;				// メッシュ(頂点情報)へのポインタ
	m_pBuffMat = NULL;			// マテリアルへのポインタ
	m_dwNumMat = NULL;			// マテリアルの数
	memset(&m_aIdxTexture[0], 0, sizeof m_aIdxTexture);		// テクスチャのインデックス
#endif
	m_bScale = false;			// 拡大管理
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);	// 拡大率
	m_nNumAll++;
}

//=============================
// デストラクタ
//=============================
CObjectX::~CObjectX()
{

}

//=============================
// オブジェクトX生成処理
//=============================
CObjectX* CObjectX::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, const char* pFileName, bool bScale, D3DXVECTOR3 scl)
{
	CObjectX* pObjectX = NULL;	// オブジェクトへのポインタを用意
	if (pObjectX == NULL)
	{ // NULLが設定できている場合
		pObjectX = new CObjectX;
		if (pObjectX != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pObjectX->Init(pos, rot, pFileName, bScale, scl);
			// 位置設定
			pObjectX->SetPosition(pos);
			return pObjectX;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CObjectX::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, const char* pFileName, bool bScale, D3DXVECTOR3 scl)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 初期化
	m_pos = pos;
	m_rot = rot;
	m_scl = scl;
	if (m_scl.x == 1.0f && m_scl.y == 1.0f && m_scl.z == 1.0f)
	{
		m_bScale = false;
	}
	else
	{
		m_bScale = true;
	}

#ifdef ENABLE_FILEX
	CFileX* pFileX = CManager::GetFileX();
	m_nIdx = pFileX->Register(pFileName);	// モデル情報登録
	m_vtxMin = pFileX->GetVtxMin(m_nIdx);
	m_vtxMax = pFileX->GetVtxMax(m_nIdx);
#else
	D3DXMATERIAL* pMat;	// マテリアル
	int nNumVtx = 0;	// 頂点数
	DWORD dwSizeFVF;			// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;				// 頂点バッファへのポインタ

	// Xファイルの読み込み
	D3DXLoadMeshFromX(pFileName, D3DXMESH_SYSTEMMEM, pDevice, NULL, &m_pBuffMat, NULL, &m_dwNumMat, &m_pMesh);

	// 頂点数を取得
	nNumVtx = m_pMesh->GetNumVertices();
	// 頂点フォーマットのサイズを取得
	dwSizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

	// 頂点バッファをロック
	m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);
	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++, pVtxBuff += dwSizeFVF)
	{
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;	// 頂点座標の代入
		// X座標の比較
		if (m_vtxMin.x >= vtx.x)			// 保存されている最小のX座標より小さい場合
		{
			m_vtxMin.x = vtx.x;
		}
		else if (m_vtxMax.x <= vtx.x)	// 保存されている最大のX座標より大きい場合
		{
			m_vtxMax.x = vtx.x;
		}

		// Y座標の比較
		if (m_vtxMin.y >= vtx.y)			// 保存されている最小のY座標より小さい場合
		{
			m_vtxMin.y = vtx.y;
		}
		else if (m_vtxMax.y <= vtx.y)	// 保存されている最大のY座標より大きい場合
		{
			m_vtxMax.y = vtx.y;
		}

		// Z座標の比較
		if (m_vtxMin.z >= vtx.z)			// 保存されている最小のZ座標より小さい場合
		{
			m_vtxMin.z = vtx.z;
		}
		else if (m_vtxMax.z <= vtx.z)	// 保存されている最大のZ座標より大きい場合
		{
			m_vtxMax.z = vtx.z;
		}
	}
	// 頂点バッファをアンロック
	m_pMesh->UnlockVertexBuffer();

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();
	// テクスチャの設定
	CTexture* pTexture = CManager::GetTexture();

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		if (pMat[nCntMat].pTextureFilename != NULL)	// テクスチャファイルが存在する場合
		{
			m_aIdxTexture[nCntMat] = pTexture->Register(pMat[nCntMat].pTextureFilename);
		}
		else
		{
			m_aIdxTexture[nCntMat] = -1;
		}
	}
#endif
	return S_OK;
}

//=============================
// 初期化処理
//=============================
HRESULT CObjectX::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax, D3DXVECTOR3 scl, bool bScale, LPD3DXBUFFER pBuffMat, DWORD dwNumMat, LPD3DXMESH pMesh, int* pIdxTexture, const char* pFileName)
{
#ifdef ENABLE_FILEX
	CFileX* pFileX = CManager::GetFileX();
	m_nIdx = pFileX->Register(pFileName);	// モデル情報登録
	m_vtxMin = pFileX->GetVtxMin(m_nIdx);
	m_vtxMax = pFileX->GetVtxMax(m_nIdx);
#else
	// 初期化
	m_pos = pos;
	m_rot = rot;
	m_vtxMin = vtxMin;
	m_vtxMax = vtxMax;
	m_scl = scl;
	m_bScale = bScale;
	m_pBuffMat = pBuffMat;
	m_dwNumMat = dwNumMat;
	m_pMesh = pMesh;

	// Xファイルの読み込み
	//D3DXLoadMeshFromX(pFileName, D3DXMESH_SYSTEMMEM, CManager::GetRenderer()->GetDevice(), NULL, &m_pBuffMat, NULL, &m_dwNumMat, &m_pMesh);

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		m_aIdxTexture[nCntMat] = pIdxTexture[nCntMat];
	}
#endif
	return S_OK;
}

//=============================
// 終了処理
//=============================
void CObjectX::Uninit(void)
{
#ifdef ENABLE_FILEX

#else
	// メッシュの破棄
	SAFE_RELEASE(m_pMesh);

	// マテリアルの破棄
	SAFE_RELEASE(m_pBuffMat);
#endif
	m_nNumAll--;
}

//=============================
// 終了処理[メッシュ等の破棄管理]
//=============================
void CObjectX::Uninit(bool bEnable)
{
#ifdef ENABLE_FILEX

#else
	if (bEnable == true)
	{
		// メッシュの破棄
		SAFE_RELEASE(m_pMesh);

		// マテリアルの破棄
		SAFE_RELEASE(m_pBuffMat);
		m_nNumAll--;
		return;
	}

	// メッシュのクリア
	if (m_pMesh != NULL)
	{
		m_pMesh = NULL;
	}

	// マテリアルのクリア
	if (m_pBuffMat != NULL)
	{
		m_pBuffMat = NULL;
	}
#endif
	m_nNumAll--;
}

//=============================
// 更新処理
//=============================
void CObjectX::Update(void)
{
}

//=============================
// 描画処理
//=============================
void CObjectX::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	D3DXMATRIX mtxScale, mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;	// 現在のマテリアルを保存
	D3DXMATERIAL* pMat;		// マテリアルデータへのポインタ

	// ワールドマトリックスの初期化(デフォルトの値にする)
	D3DXMatrixIdentity(&m_mtxWorld);

	if (m_bScale == true)
	{
		// モデルの法線を正規化
		pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

		// 拡大率を変更
		D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);
	}

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

#ifdef ENABLE_FILEX
	CFileX* pFileX = CManager::GetFileX();

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pFileX->GetBuffMat(m_nIdx)->GetBufferPointer();

	// テクスチャの設定
	CTexture* pTexture = CManager::GetTexture();

	// マテリアルごとに描画
	for (int nCntMat = 0; nCntMat < (int)pFileX->GetNumMat(m_nIdx); nCntMat++)
	{
		// マテリアルの取得
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// マテリアルの色を保存
		float fRed = pMat[nCntMat].MatD3D.Diffuse.r;
		float fGreen = pMat[nCntMat].MatD3D.Diffuse.g;
		float fBlue = pMat[nCntMat].MatD3D.Diffuse.b;
		float fAlpha = pMat[nCntMat].MatD3D.Diffuse.a;

		if (m_colType == COLORTYPE_CHANGE)
		{
			pMat[nCntMat].MatD3D.Diffuse.r = m_col.r;
			pMat[nCntMat].MatD3D.Diffuse.g = m_col.g;
			pMat[nCntMat].MatD3D.Diffuse.b = m_col.b;
		}
		else if (m_colType == COLORTYPE_ALPHA)
		{
			pMat[nCntMat].MatD3D.Diffuse.a = m_col.a;
		}

		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(pFileX->GetIdxTexture(m_nIdx, nCntMat)));

		// モデルの描画
		pFileX->GetMesh(m_nIdx)->DrawSubset(nCntMat);

		// 元のマテリアルの色に戻す
		pMat[nCntMat].MatD3D.Diffuse.r = fRed;
		pMat[nCntMat].MatD3D.Diffuse.g = fGreen;
		pMat[nCntMat].MatD3D.Diffuse.b = fBlue;
		pMat[nCntMat].MatD3D.Diffuse.a = fAlpha;
	}

#else
	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	// テクスチャの設定
	CTexture* pTexture = CManager::GetTexture();

	// マテリアルごとに描画
	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		// マテリアルの取得
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// マテリアルの色を保存
		float fRed = pMat[nCntMat].MatD3D.Diffuse.r;
		float fGreen = pMat[nCntMat].MatD3D.Diffuse.g;
		float fBlue = pMat[nCntMat].MatD3D.Diffuse.b;
		float fAlpha = pMat[nCntMat].MatD3D.Diffuse.a;

		if (m_colType == COLORTYPE_CHANGE)
		{
			pMat[nCntMat].MatD3D.Diffuse.r = m_col.r;
			pMat[nCntMat].MatD3D.Diffuse.g = m_col.g;
			pMat[nCntMat].MatD3D.Diffuse.b = m_col.b;
		}
		else if (m_colType == COLORTYPE_ALPHA)
		{
			pMat[nCntMat].MatD3D.Diffuse.a = m_col.a;
		}

		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(m_aIdxTexture[nCntMat]));

		// モデルの描画
		m_pMesh->DrawSubset(nCntMat);

		// 元のマテリアルの色に戻す
		pMat[nCntMat].MatD3D.Diffuse.r = fRed;
		pMat[nCntMat].MatD3D.Diffuse.g = fGreen;
		pMat[nCntMat].MatD3D.Diffuse.b = fBlue;
		pMat[nCntMat].MatD3D.Diffuse.a = fAlpha;
	}
#endif

	// 保存していたマテリアルに戻す
	pDevice->SetMaterial(&matDef);

	if (m_bScale == true)
	{ // モデルの法線を正規化
		pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	}
}

//=============================
// 描画処理
//=============================
void CObjectX::Draw(D3DXMATRIX mtx)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();
	D3DMATERIAL9 matDef;	// 現在のマテリアルを保存
	D3DXMATERIAL* pMat;		// マテリアルデータへのポインタ
	// テクスチャの設定
	CTexture* pTexture = CManager::GetTexture();
	
	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtx);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);
#ifdef ENABLE_FILEX
	CFileX* pFileX = CManager::GetFileX();

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pFileX->GetBuffMat(m_nIdx)->GetBufferPointer();

	// マテリアルごとに描画
	for (int nCntMat = 0; nCntMat < (int)pFileX->GetNumMat(m_nIdx); nCntMat++)
	{
		// マテリアルの取得
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// マテリアルの色を保存
		float fRed = pMat[nCntMat].MatD3D.Diffuse.r;
		float fGreen = pMat[nCntMat].MatD3D.Diffuse.g;
		float fBlue = pMat[nCntMat].MatD3D.Diffuse.b;
		float fAlpha = pMat[nCntMat].MatD3D.Diffuse.a;

		if (m_colType == COLORTYPE_CHANGE)
		{
			pMat[nCntMat].MatD3D.Diffuse.r = m_col.r;
			pMat[nCntMat].MatD3D.Diffuse.g = m_col.g;
			pMat[nCntMat].MatD3D.Diffuse.b = m_col.b;
		}
		else if (m_colType == COLORTYPE_ALPHA)
		{
			pMat[nCntMat].MatD3D.Diffuse.a = m_col.a;
		}

		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(pFileX->GetIdxTexture(m_nIdx, nCntMat)));

		// モデルの描画
		pFileX->GetMesh(m_nIdx)->DrawSubset(nCntMat);

		// 元のマテリアルの色に戻す
		pMat[nCntMat].MatD3D.Diffuse.r = fRed;
		pMat[nCntMat].MatD3D.Diffuse.g = fGreen;
		pMat[nCntMat].MatD3D.Diffuse.b = fBlue;
		pMat[nCntMat].MatD3D.Diffuse.a = fAlpha;
	}
#else
	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtx);

	// マテリアルごとに描画
	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		// マテリアルの取得
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);
		
		// マテリアルの色を保存
		float fRed = pMat[nCntMat].MatD3D.Diffuse.r;
		float fGreen = pMat[nCntMat].MatD3D.Diffuse.g;
		float fBlue = pMat[nCntMat].MatD3D.Diffuse.b;

		if (m_colType == COLORTYPE_CHANGE)
		{
			pMat[nCntMat].MatD3D.Diffuse.r = m_col.r;
			pMat[nCntMat].MatD3D.Diffuse.g = m_col.g;
			pMat[nCntMat].MatD3D.Diffuse.b = m_col.b;
		}

		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(m_aIdxTexture[nCntMat]));

		// モデルの描画
		m_pMesh->DrawSubset(nCntMat);

		// 元のマテリアルの色に戻す
		pMat[nCntMat].MatD3D.Diffuse.r = fRed;
		pMat[nCntMat].MatD3D.Diffuse.g = fGreen;
		pMat[nCntMat].MatD3D.Diffuse.b = fBlue;
	}
#endif
	// 保存していたマテリアルに戻す
	pDevice->SetMaterial(&matDef);
}