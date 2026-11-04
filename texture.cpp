//========================================================
// 
// テクスチャ管理処理[texture.cpp]
// Author : Kajino Hiyori _Create2026/06/01
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "texture.h"
#include "renderer.h"
#include "manager.h"

//=============================
// 静的メンバ変数宣言
//=============================
int CTexture::m_nNumAll = 0;	// テクスチャの総数を格納

//=============================
// コンストラクタ
//=============================
CTexture::CTexture()
{
	// テクスチャのクリア
	memset(&m_apTexture[0], NULL, sizeof m_apTexture);
	memset(&m_apFileName[0], NULL, sizeof m_apFileName);
}

//=============================
// デストラクタ
//=============================
CTexture::~CTexture()
{

}

//=============================
// 登録済テクスチャの読み込み
//=============================
HRESULT CTexture::Load(void)
{
	return S_OK;
}

//=============================
// 全テクスチャの破棄
//=============================
void CTexture::Unload(void)
{
	for (int nCntTex = 0; nCntTex < NUM_TEXTURE; nCntTex++)
	{
		SAFE_RELEASE(m_apTexture[nCntTex]);
		SAFE_DELETE_PLURAL(m_apFileName[nCntTex]);;
	}
}

//=============================
// テクスチャの登録
//=============================
int CTexture::Register(const char* pFileName)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// インデックスを格納
	int nIdx = m_nNumAll;

	for (int nCntTexture = 0; nCntTexture < m_nNumAll; nCntTexture++)
	{
		if (strcmp(pFileName, m_apFileName[nCntTexture]) == 0)
		{ // 既存テクスチャ
			return nCntTexture;
		}
	}

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, pFileName, &m_apTexture[nIdx]);
	int nLength = strlen(pFileName);
	m_apFileName[nIdx] = new char[nLength + 1];
	strcpy(m_apFileName[nIdx], pFileName);
	
	m_nNumAll++;	// 総数を加算
	return nIdx;
}

//=============================
// テクスチャの登録
//=============================
LPDIRECT3DTEXTURE9 CTexture::GetAddress(int nIdx)
{
	if (nIdx == -1)
	{ // テクスチャが設定されていない
		return NULL;
	}

	return m_apTexture[nIdx];
}
