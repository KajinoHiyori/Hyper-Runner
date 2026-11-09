//========================================================
// 
// Xファイル管理処理[fileX.h]
// Author : Kajino Hiyori _Create2026/10/22
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "renderer.h"
#include "manager.h"
#include "fileX.h"
#include "loadscripts.h"
//=============================
// マクロ定義
//=============================
#define LOADMODEL		("LOADMODEL")		// LOADMODEL
#define END_LOADMODEL	("END_LOADMODEL")	// END_LOADMODEL
#define MAXMODEL		("MAXMODEL")		// MAXMODEL
#define MODELNAME		("MODELNAME")		// MODELNAME

//=============================
// 静的メンバ変数宣言
//=============================
int CFileX::m_nNumAll = 0;	// Xファイルの総数を格納

//=============================
// コンストラクタ
//=============================
CFileX::CFileX()
{
	// Xファイルのクリア
	for (int nCntFile = 0; nCntFile < NUM_FILEX; nCntFile++)
	{
		m_apFileName[nCntFile] = NULL;	// 相対パスのコピー
		m_apMesh[nCntFile] = NULL;		// メッシュ(頂点情報)へのポインタ
		m_apBuffMat[nCntFile] = NULL;	// マテリアルへのポインタ
		m_adwNumMat[nCntFile] = 0;		// マテリアルの数
		m_aVtxMin[nCntFile] = INIT_D3DVEC3;		// Xファイル最小値
		m_aVtxMax[nCntFile] = INIT_D3DVEC3;		// Xファイル最大値
		memset(&m_aIdxTexture[nCntFile][0], -1, sizeof m_aIdxTexture[nCntFile]);	// テクスチャインデックス
	}
}

//=============================
// デストラクタ
//=============================
CFileX::~CFileX()
{

}

//=============================
// 登録済Xファイルの読み込み
//=============================
HRESULT CFileX::Load(void)
{
	
	return S_OK;
}

//=============================
// 全Xファイルの破棄
//=============================
void CFileX::Unload(void)
{
	for (int nCntFile = 0; nCntFile < NUM_FILEX; nCntFile++)
	{
		// ファイル名破棄
		SAFE_DELETE_PLURAL(m_apFileName[nCntFile]);

		if (m_adwNumMat[nCntFile] != 0)
		{ // マテリアル数破棄
			m_adwNumMat[nCntFile] = 0;
		}
		// メッシュ破棄
		SAFE_RELEASE(m_apMesh[nCntFile]);
		// マテリアルへのポインタ
		SAFE_RELEASE(m_apBuffMat[nCntFile]);
	}
}

//=============================
// Xファイルの登録
//=============================
int CFileX::Register(const char* pFileName)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// インデックスを格納
	int nIdx = m_nNumAll;

	for (int nCntFileX = 0; nCntFileX < m_nNumAll; nCntFileX++)
	{
		if (strcmp(pFileName, m_apFileName[nCntFileX]) == 0)
		{ // 既存Xファイル
			return nCntFileX;
		}
	}

	// Xファイルの読み込み
	D3DXLoadMeshFromX(pFileName, D3DXMESH_SYSTEMMEM, pDevice, NULL, &m_apBuffMat[nIdx], NULL, &m_adwNumMat[nIdx], &m_apMesh[nIdx]);

	// 頂点数を取得
	int nNumVtx = m_apMesh[nIdx]->GetNumVertices();
	// 頂点フォーマットのサイズを取得
	DWORD dwSizeFVF = D3DXGetFVFVertexSize(m_apMesh[nIdx]->GetFVF());
	BYTE* pVtxBuff;				// 頂点バッファへのポインタ
	D3DXMATERIAL* pMat;	// マテリアル

	// 頂点バッファをロック
	m_apMesh[nIdx]->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);
	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++, pVtxBuff += dwSizeFVF)
	{
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;	// 頂点座標の代入
		// X座標の比較
		if (m_aVtxMin[nIdx].x >= vtx.x)			// 保存されている最小のX座標より小さい場合
		{
			m_aVtxMin[nIdx].x = vtx.x;
		}
		else if (m_aVtxMax[nIdx].x <= vtx.x)	// 保存されている最大のX座標より大きい場合
		{
			m_aVtxMax[nIdx].x = vtx.x;
		}

		// Y座標の比較
		if (m_aVtxMin[nIdx].y >= vtx.y)			// 保存されている最小のY座標より小さい場合
		{
			m_aVtxMin[nIdx].y = vtx.y;
		}
		else if (m_aVtxMax[nIdx].y <= vtx.y)	// 保存されている最大のY座標より大きい場合
		{
			m_aVtxMax[nIdx].y = vtx.y;
		}

		// Z座標の比較
		if (m_aVtxMin[nIdx].z >= vtx.z)			// 保存されている最小のZ座標より小さい場合
		{
			m_aVtxMin[nIdx].z = vtx.z;
		}
		else if (m_aVtxMax[nIdx].z <= vtx.z)	// 保存されている最大のZ座標より大きい場合
		{
			m_aVtxMax[nIdx].z = vtx.z;
		}
	}
	// 頂点バッファをアンロック
	m_apMesh[nIdx]->UnlockVertexBuffer();

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_apBuffMat[nIdx]->GetBufferPointer();
	// テクスチャの設定
	CTexture* pTexture = CManager::GetTexture();

	for (int nCntMat = 0; nCntMat < (int)m_adwNumMat[nIdx]; nCntMat++)
	{
		if (pMat[nCntMat].pTextureFilename != NULL)	// テクスチャファイルが存在する場合
		{
			m_aIdxTexture[nIdx][nCntMat] = pTexture->Register(pMat[nCntMat].pTextureFilename);
		}
		else
		{
			m_aIdxTexture[nIdx][nCntMat] = -1;
		}
	}

	// ファイル名を登録
	int nLength = strlen(pFileName);
	m_apFileName[nIdx] = new char[nLength + 1];
	strcpy(m_apFileName[nIdx], pFileName);

	m_nNumAll++;	// 総数を加算
	return nIdx;
}