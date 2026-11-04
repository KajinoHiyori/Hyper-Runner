//========================================================
// 
// オブジェクトX処理[model.cpp]
// Author : Kajino Hiyori _Create2026/06/02
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "model.h"
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
#define NORMAL			(D3DXVECTOR3(0.0f, 1.0f, 0.0f))					// 基本の法線
#define VTX_MIN			(D3DXVECTOR3(10000.0f, 10000.0f, 10000.0f))		// プレイヤーの大きさの初期化値(最小)
#define VTX_MAX			(D3DXVECTOR3(-10000.0f, -10000.0f, -10000.0f))	// プレイヤーの大きさの初期化値(最大)

//=============================
// コンストラクタ
//=============================
CModel::CModel()
{
	m_pos = INIT_D3DVEC3;			// モデルの位置
	m_posOffset = INIT_D3DVEC3;		// モデルの位置[オフセット]
	m_rot = INIT_D3DVEC3;			// モデルの向き
	m_rotOffset = INIT_D3DVEC3;		// モデルの向き[オフセット]
	m_pParent = NULL;			// 親モデルへのポインタ
	m_vtxMin = INIT_D3DVEC3;	// モデルの大きさ(最小)
	m_vtxMax = INIT_D3DVEC3;	// モデルの大きさ(最大)
	m_nIdx = -1;				// Xファイルインデックス
}

//=============================
// デストラクタ
//=============================
CModel::~CModel()
{

}

//=============================
// オブジェクトX生成処理
//=============================
CModel* CModel::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, const char* pFileName)
{
	CModel* pModel = NULL;	// オブジェクトへのポインタを用意
	if (pModel == NULL)
	{ // NULLが設定できている場合
		pModel = new CModel;
		if (pModel != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pModel->Init(pos, rot, pFileName);

			return pModel;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CModel::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, const char* pFileName)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	D3DXMATERIAL* pMat;	// マテリアル
	int nNumVtx = 0;	// 頂点数
	DWORD dwSizeFVF;			// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;				// 頂点バッファへのポインタ

	// 初期化
	m_pos = pos;		// モデル位置
	m_posOffset = pos;	// モデル位置[オフセット]
	m_rot = rot;		// モデル角度
	m_rotOffset = rot;	// モデル角度[オフセット]
	m_vtxMin = VTX_MIN;
	m_vtxMax = VTX_MAX;

	// Xファイルの読み込み
	CFileX* pFileX = CManager::GetFileX();
	m_nIdx = pFileX->Register(pFileName);	// モデル情報登録
	m_vtxMin = pFileX->GetVtxMin(m_nIdx);
	m_vtxMax = pFileX->GetVtxMax(m_nIdx);

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CModel::Uninit(void)
{
}

//=============================
// 更新処理
//=============================
void CModel::Update(void)
{
	
}

//=============================
// 描画処理
//=============================
void CModel::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;	// 現在のマテリアルを保存
	D3DXMATERIAL* pMat;		// マテリアルデータへのポインタ

	// ワールドマトリックスの初期化(デフォルトの値にする)
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// 親のマトリックスと掛け合わせる
	D3DXMATRIX mtxParent;	// 親マトリックスの格納
	if (m_pParent != NULL)
	{ // 親モデルがある場合
		// 親モデルのマトリックスを取得
		mtxParent = m_pParent->GetMtxWorld();
	}
	else
	{ // 親モデルがない場合
		// 直前のマトリックス = 管理元のマトリックスを取得
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
	}

	// 算出したパーツのワールドマトリックスと親モデルのマトリックスを掛け合わせる
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

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

	// 保存していたマテリアルに戻す
	pDevice->SetMaterial(&matDef);
}