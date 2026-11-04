//========================================================
// 
// 光源処理[light.cpp]
// Author : Kajino Hiyori _Create2026/05/31
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "light.h"
#include "renderer.h"
#include "input.h"
#include "manager.h"
#include "color.h"
#include "debugproc.h"

//=============================
// マクロ定義
//=============================
#define DIRECTOPN0	(D3DXVECTOR3(0.2f, -0.8f, 0.4f))		// ライトの方向
#define DIRECTOPN1	(D3DXVECTOR3(-0.2f, 0.9f, -0.4f))		// ライトの方向
#define DIRECTOPN2	(D3DXVECTOR3(0.9f, 0.1f, 0.4f))			// ライトの方向
#define LIGHT_BRIGHTEST		(D3DXCOLOR(1.000f,	1.000f,	1.000f,	1.000f))	// 1番目に明るい光源
#define LIGHT_BRIGHTER		(D3DXCOLOR(0.700f,	0.700f,	0.700f,	1.000f))	// 2番目に明るい光源
#define LIGHT_BRIGHT		(D3DXCOLOR(0.300f,	0.300f,	0.300f,	1.000f))	// 3番目に明るい光源

//=============================
// コンストラクタ
//=============================
CLight::CLight()
{
	// ライトの情報をクリア
	ZeroMemory(&m_aLight[0], sizeof(D3DLIGHT9) * NUM_LIGHT);
}

//=============================
// デストラクタ
//=============================
CLight::~CLight()
{

}

//=============================
// ライトの初期化処理
//=============================
HRESULT CLight::Init(void)
{
	// 基本ライトの用意
	Default();

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CLight::Uninit(void)
{

}

//=============================
// 更新処理
//=============================
void CLight::Update(void)
{
}

//=============================
// ライト設定処理
//=============================
void CLight::Set(CScene* pScene)
{
	// 基本ライトの用意
	Default();
}

//=============================
// 基本ライトの初期化処理
//=============================
void CLight::Default(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();
	D3DXVECTOR3 vecDir;		// ライトの方向ベクトル

	for (int nCntLight = 0; nCntLight < NUM_LIGHT; nCntLight++)
	{
		// ライトの種類を設定
		m_aLight[nCntLight].Type = D3DLIGHT_DIRECTIONAL;

		switch (nCntLight)
		{
		case 0:	// 0番目のライト
			// ライトの拡散光を設定
			m_aLight[nCntLight].Diffuse = LIGHT_BRIGHTEST;
			// ライトの方向を設定
			vecDir = DIRECTOPN0;
			break;

		case 1:	// 1番目のライト
			// ライトの拡散光を設定
			m_aLight[nCntLight].Diffuse = LIGHT_BRIGHTER;
			// ライトの方向を設定
			vecDir = DIRECTOPN1;
			break;

		case 2:	// 2番目のライト
			// ライトの拡散光を設定
			m_aLight[nCntLight].Diffuse = LIGHT_BRIGHT;
			// ライトの方向を設定
			vecDir = DIRECTOPN2;
			break;
		}

		D3DXVec3Normalize(&vecDir, &vecDir);	// ベクトルを正規化
		m_aLight[nCntLight].Direction = vecDir;
		// ライトを設定
		pDevice->SetLight(nCntLight, &m_aLight[nCntLight]);
		// ライトを有効にする
		pDevice->LightEnable(nCntLight, TRUE);
	}
}