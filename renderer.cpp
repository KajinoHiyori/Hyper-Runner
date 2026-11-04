//==========================================
// 
// レンダラー処理[renderer.cpp]
// Author : Kajino Hiyori _Create2026/05/08
// フィードバックエフェクトの処理の一部に
// chatGPTを用いています
// 
//==========================================
#include "renderer.h"
#include "object.h"
#include "debugproc.h"
#include "manager.h"
#include "camera.h"
#include "input.h"

//========================================================================
// マクロ定義
//========================================================================
#define FEEDBACK	(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f))
#define BLAR		(15.0f)
#define CLEAR_COLOR	(D3DCOLOR_RGBA(73, 39, 3, 0))

//========================================================================
// コンストラクタ
//========================================================================
CRenderer::CRenderer()
{
	m_pD3D = NULL;			// Direct3DオブジェクトへのポインタにNULLを設定
	m_pD3DDevice = NULL;	// Direct3DデバイスへのポインタにNULLを設定
#ifdef ENABLE_MULTITARGETRENDERING
	m_apTextureMT[0] = {};	// レンダリングターゲット用テクスチャ
	m_apRenderMT[0] = {};	// テクスチャレンダリング用インターフェース
	m_pZBuffMT = NULL;		// テクスチャレンダリング用Zバッファ
	m_pVtxBuffMT = NULL;	// 頂点バッファのクリア
	m_bClearMT = true;		// 画面クリア管理
	m_bFeedBack = false;	// フィードバック有無を管理
	m_bFeedBackDest = false;
#endif
}

//========================================================================
// デストラクタ
//========================================================================
CRenderer::~CRenderer()
{

}

//========================================================================
// 初期化処理
//========================================================================
HRESULT CRenderer::Init(const HWND hWnd, const BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;	// ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp;	// プレゼンテーションパラメータ

	// DirectX3Dオブジェクトの生成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3D == NULL)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));							// パラメータのゼロクリア
	d3dpp.BackBufferWidth = SCREEN_WIDTH;						// ゲーム画面のサイズ(幅)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;						// ゲーム画面のサイズ(高さ)
	d3dpp.BackBufferFormat = d3ddm.Format;						// バックバッファの形式
	d3dpp.BackBufferCount = 1;									// バックバッファの数
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					// ダブルバッファの切り替え
	d3dpp.EnableAutoDepthStencil = TRUE;						// デプスバッファのとステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;					// デプスバッファとして16bitを使う
	d3dpp.Windowed = bWindow;									// ウィンドウモード
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	// リフレッシュレート
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// インターバル

	// Direct3Dデバイスの生成
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pD3DDevice)))
	{
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&m_pD3DDevice)))
		{
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&m_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

#ifdef ENABLE_MULTITARGETRENDERING
	// テクスチャレンダリング用Zバッファの生成
	m_pD3DDevice->CreateDepthStencilSurface(SCREEN_WIDTH,
		SCREEN_HEIGHT,
		D3DFMT_D16,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&m_pZBuffMT,
		NULL);

	for (int nCntFeedback = 0; nCntFeedback < NUM_FEEDBACK; nCntFeedback++)
	{
		LPDIRECT3DSURFACE9 pRenderDef, pZBuffDef;	// ローカルで情報を保存

		// レンダリングターゲット用テクスチャの生成
		m_pD3DDevice->CreateTexture(SCREEN_WIDTH,
			SCREEN_HEIGHT,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&m_apTextureMT[nCntFeedback],
			NULL);

		// テクスチャレンダリング用インターフェースの生成
		m_apTextureMT[nCntFeedback]->GetSurfaceLevel(0, &m_apRenderMT[nCntFeedback]);

		m_pD3DDevice->GetRenderTarget(0, &pRenderDef);		// 現在のレンダリングターゲットを取得して保存
		m_pD3DDevice->GetDepthStencilSurface(&pZBuffDef);	// 現在のZバッファを取得して保存
		m_pD3DDevice->SetRenderTarget(0, m_apRenderMT[nCntFeedback]);		// レンダリングターゲットを生成したテクスチャに設定
		m_pD3DDevice->SetDepthStencilSurface(m_pZBuffMT);	// Zバッファを生成したZバッファに設定

		// レンダリングターゲット用テクスチャのクリア
		m_pD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

		m_pD3DDevice->SetRenderTarget(0, pRenderDef);	// レンダリングターゲットを元に戻す
		m_pD3DDevice->SetDepthStencilSurface(pZBuffDef);	// Zバッファを元に戻す
	}

	// テクスチャレンダリング用ビューポートの設定
	m_viewportMT.X = 0;
	m_viewportMT.Y = 0;
	m_viewportMT.Width = SCREEN_WIDTH;
	m_viewportMT.Height = SCREEN_HEIGHT;
	m_viewportMT.MinZ = 0.0f;
	m_viewportMT.MaxZ = 1.0f;

	// フィードバック用ポリゴンの生成
	// 頂点バッファの生成
	m_pD3DDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &m_pVtxBuffMT, NULL);

	VERTEX_2D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuffMT->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos.x = SCREEN_CENTER.x - SCREEN_WIDTH * 0.5f - BLAR;
	pVtx[0].pos.y = SCREEN_CENTER.y - SCREEN_HEIGHT * 0.5f - BLAR;
	pVtx[0].pos.z = SCREEN_CENTER.z;
	pVtx[1].pos.x = SCREEN_CENTER.x + SCREEN_WIDTH * 0.5f + BLAR;
	pVtx[1].pos.y = SCREEN_CENTER.y - SCREEN_HEIGHT * 0.5f - BLAR;
	pVtx[1].pos.z = SCREEN_CENTER.z;
	pVtx[2].pos.x = SCREEN_CENTER.x - SCREEN_WIDTH * 0.5f - BLAR;
	pVtx[2].pos.y = SCREEN_CENTER.y + SCREEN_HEIGHT * 0.5f + BLAR;
	pVtx[2].pos.z = SCREEN_CENTER.z;
	pVtx[3].pos.x = SCREEN_CENTER.x + SCREEN_WIDTH * 0.5f + BLAR;
	pVtx[3].pos.y = SCREEN_CENTER.y + SCREEN_HEIGHT * 0.5f + BLAR;
	pVtx[3].pos.z = SCREEN_CENTER.z;

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 色の設定
	pVtx[0].col = FEEDBACK;
	pVtx[1].col = FEEDBACK;
	pVtx[2].col = FEEDBACK;
	pVtx[3].col = FEEDBACK;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuffMT->Unlock();
#endif

	// レンダーステートの設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	// カリングの設定
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);	// アルファブレンドの設定1
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); // アルファブレンドの設定2
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// アルファブレンドの設定3

	// サンプラーステートの設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	// テクスチャの拡縮補完
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// テクスチャの拡縮補完
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// テクスチャの繰り返し設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// テクスチャの繰り返し設定

	// テクスチャステージステートの設定[AI使用]
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	return S_OK;
}

//========================================================================
// 終了処理
//========================================================================
void CRenderer::Uninit(void)
{
#ifdef ENABLE_MULTITARGETRENDERING
	for (int nCntFeedback = 0; nCntFeedback < NUM_FEEDBACK; nCntFeedback++)
	{
		// レンダリングターゲット用テクスチャの破棄
		SAFE_RELEASE(m_apTextureMT[nCntFeedback]);

		// テクスチャレンダリング用インターフェース
		SAFE_RELEASE(m_apRenderMT[nCntFeedback]);
	}

	// テクスチャレンダリング用Zバッファの破棄
	SAFE_RELEASE(m_pZBuffMT);

	// フィードバック用ポリゴンの頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuffMT);
#endif
	// Direct3Dデバイスの破棄
	SAFE_RELEASE(m_pD3DDevice);

	// Direct3Dオブジェクトの破棄
	SAFE_RELEASE(m_pD3D);
}

//========================================================================
// 更新処理
//========================================================================
void CRenderer::Update(void)
{
	// 全てのオブジェクトを更新
	CObject::UpdateAll();

#ifdef _DEBUG
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	if (pKeyboard->GetTrigger(DIK_F) == true)
	{
		m_bFeedBackDest = m_bFeedBack ^ 1;
		m_bFeedBack = m_bFeedBack ^ 1;
		m_bClearTexture = true;
	}
#endif // _DEBUG
}

//========================================================================
// 描画処理
//========================================================================
void CRenderer::Draw(void)
{
	// カメラ
	CCamera* pCamera = CManager::GetCamera();

	// フェード
	CFade* pFade = CManager::GetFade();

	// デバッグ表示
	CDebugProc* pDebugProc = CManager::GetDebugProc();

#ifdef ENABLE_MULTITARGETRENDERING
	if (m_bClearTexture)
	{
		ClearTextures();
		m_bClearTexture = false;
	}
	else
	{
		// 画面のクリア(バックバッファとZバッファのクリア)
		m_pD3DDevice->Clear(0, NULL,
			(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
			CLEAR_COLOR, 1.0f, 0);
	}
#else
	// 画面のクリア(バックバッファとZバッファのクリア)
	m_pD3DDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		CLEAR_COLOR, 1.0f, 0);
#endif

#ifdef ENABLE_MULTITARGETRENDERING
	// 現在の状態をローカルで保存
	LPDIRECT3DSURFACE9 pRenderDef, pZBuffDef;
	D3DVIEWPORT9 viewportDef;
	D3DXMATRIX mtxView, mtxProjectionDef;
	m_pD3DDevice->GetRenderTarget(0, &pRenderDef);	// レンダリングターゲット
	m_pD3DDevice->GetDepthStencilSurface(&pZBuffDef);	// Zバッファ
	m_pD3DDevice->GetViewport(&viewportDef);				// ビューポート
	m_pD3DDevice->GetTransform(D3DTS_VIEW, &mtxView);	// ビューマトリックス
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &mtxProjectionDef);	// プロジェクションマトリックス

	// レンダリングターゲットを0に指定
	if (m_bFeedBack == true)
	{
		ChangeTarget(pCamera->GetPosV(), pCamera->GetPosR(), pCamera->GetVecU());
	}
#endif // ENABLE_MULTITARGETRENDERING

	// 描画開始
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{ // 描画が成功した場合
		// カメラの用意
		pCamera->SetCamera();

		// 全てのオブジェクトの描画
		CObject::DrawAll();

#ifdef ENABLE_MULTITARGETRENDERING
		// フィードバック用ポリゴンにテクスチャ1を貼り付ける
		if (m_bFeedBack == true)
		{
			DrawPastImages();
		}
		//========================================================================================
		// レンダリングターゲットを元に戻す
		m_pD3DDevice->SetRenderTarget(0, pRenderDef);		// レンダリングターゲット
		m_pD3DDevice->SetDepthStencilSurface(pZBuffDef);	// Zバッファ
		m_pD3DDevice->SetViewport(&viewportDef);			// ビューポート
		m_pD3DDevice->SetTransform(D3DTS_VIEW, &mtxView);	// ビューマトリックス
		m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mtxProjectionDef);	// プロジェクションマトリックス

		// フィードバック用ポリゴンにテクスチャ0を貼り付ける
		if (m_bFeedBack == true)
		{
			DrawCurrentImages();
		}

		// テクスチャ0とテクスチャ1を入れ替える
		SwapTexture();

		// 画面クリア管理
		SetClear(true);
#endif

		// フェードの描画
		if (CManager::GetFadeState() != CFade::STATE_FADENONE)
		{
			pFade->Draw();
		}

		// デバッグ表示
		pDebugProc->Draw();

		// 描画終了
		m_pD3DDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

#ifdef ENABLE_MULTITARGETRENDERING
//========================================================================
// レンダリングターゲットの変更
//========================================================================
void CRenderer::ChangeTarget(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 vecU)
{
	D3DXMATRIX mtxView, mtxProjection;	// カメラ設定
	float fAspect = 0.0f;	// アスペクト比

	m_pD3DDevice->SetRenderTarget(0, m_apRenderMT[0]);		// レンダリングターゲットを生成したテクスチャに設定
	m_pD3DDevice->SetDepthStencilSurface(m_pZBuffMT);	// Zバッファを生成したZバッファに設定
	m_pD3DDevice->SetViewport(&m_viewportMT);			// テクスチャレンダリング用のビューポート設定

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&mtxView);

	// ビューマトリックスを作成
	D3DXMatrixLookAtLH(&mtxView, &posV, &posR, &vecU);	// 上方向ベクトル

	// ビューマトリックスの設定
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &mtxView);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&mtxProjection);
	fAspect = (float)m_viewportMT.Width / (float)m_viewportMT.Height;

#ifdef CAMERATYPE_ORTHO	// 投影方法
	// プロジェクションマトリックスの作成[左手座標系の平行投影]
	D3DXMatrixOrthoLH(&mtxProjection,
		(float)m_viewportMT.Width,	// ビューボリュームの幅
		(float)m_viewportMT.Height,	// ビューボリュームの高さ
		CAMERA_CLOSEST,			// ビューボリュームの近平面までの距離
		CAMERA_FARTHEST);		// ビューボリュームの遠平面までの距離
#else
	// プロジェクションマトリックスの作成[左手座標系の透視投影]
	D3DXMatrixPerspectiveFovLH(&mtxProjection,
		D3DXToRadian(FOV_DEFAULT),					// 視野角
		fAspect,									// アスペクト比
		CAMERA_CLOSEST,								// カメラから描画できる距離(カメラに近い場合)
		CAMERA_FARTHEST);							// カメラから描画できる距離(カメラから離れている場合)
#endif
	// プロジェクションマトリックスの設定
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mtxProjection);

	if (m_bClearMT == true)
	{
		// 画面のクリア(バックバッファとZバッファのクリア)
		m_pD3DDevice->Clear(0, NULL,
			(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
			CLEAR_COLOR, 1.0f, 0);
		m_bClearMT = false;
	}
}

//========================================================================
// フィードバック用ポリゴンにテクスチャ1を貼り付ける[過去の画を書き込む]
//========================================================================
void CRenderer::DrawPastImages(void)
{
	// アルファ値を適応
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// テクスチャステージステートを管理[AI使用]
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_pD3DDevice->SetStreamSource(0, m_pVtxBuffMT, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	m_pD3DDevice->SetFVF(FVF_VERTEX_2D);
	// テクスチャの設定
	m_pD3DDevice->SetTexture(0, m_apTextureMT[1]);
	
	if (m_bFeedBackDest == true)
	{
		m_bFeedBackDest = false; 
		return;
	}
	// ポリゴンの描画
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//========================================================================
// フィードバック用ポリゴンにテクスチャ0を貼り付ける[現在の画を書き込む]
//========================================================================
void CRenderer::DrawCurrentImages(void)
{
	// ポリゴンのアルファ値を無視
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	// 頂点バッファをデータストリームに設定
	m_pD3DDevice->SetStreamSource(0, m_pVtxBuffMT, 0, sizeof(VERTEX_2D));
	// 頂点フォーマットの設定
	m_pD3DDevice->SetFVF(FVF_VERTEX_2D);
	// テクスチャの設定
	m_pD3DDevice->SetTexture(0, m_apTextureMT[0]);	// 今回の画像を貼り付ける
	// ポリゴンの描画
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	// ポリゴンのアルファ値をデフォルトに戻す
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
}

//========================================================================
// テクスチャ0とテクスチャ1を入れ替える
//========================================================================
void CRenderer::SwapTexture(void)
{
	LPDIRECT3DSURFACE9 pRenderWk = NULL;
	pRenderWk = m_apRenderMT[0];
	m_apRenderMT[0] = m_apRenderMT[1];
	m_apRenderMT[1] = pRenderWk;
	LPDIRECT3DTEXTURE9 pTextureWK = NULL;
	pTextureWK = m_apTextureMT[0];
	m_apTextureMT[0] = m_apTextureMT[1];
	m_apTextureMT[1] = pTextureWK;
}

//========================================================================
// フィードバックと通常時の補完[AI使用]
//========================================================================
void CRenderer::ClearTextures(void)
{
	// 現在の情報格納
	LPDIRECT3DSURFACE9 pRenderDef = NULL;	// レンダリング先
	LPDIRECT3DSURFACE9 pZBuffDef = NULL;	// Xバッファ

	// 現在の描画先を保存
	m_pD3DDevice->GetRenderTarget(0, &pRenderDef);
	m_pD3DDevice->GetDepthStencilSurface(&pZBuffDef);

	for (int nCntFeedback = 0; nCntFeedback < NUM_FEEDBACK; nCntFeedback++)
	{
		// フィードバック用テクスチャを描画先に設定
		m_pD3DDevice->SetRenderTarget(0, m_apRenderMT[nCntFeedback]);
		m_pD3DDevice->SetDepthStencilSurface(m_pZBuffMT);
		m_pD3DDevice->SetViewport(&m_viewportMT);

		// 過去の画像を消去
		m_pD3DDevice->Clear(0, NULL,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			CLEAR_COLOR,
			1.0f,0);
	}

	// 描画先を下に戻す
	m_pD3DDevice->SetRenderTarget(0, pRenderDef);
	if (pZBuffDef != NULL)
	{
		m_pD3DDevice->SetDepthStencilSurface(pZBuffDef);
	}

	// 情報破棄
	SAFE_RELEASE(pRenderDef);

	SAFE_RELEASE(pZBuffDef);

	// 次回の描画で古い画像が使われないようにする
	m_bClearMT = true;
}

#endif