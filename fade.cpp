//==========================================
// 
// フェード処理[fade.h]
// Author : Kajino Hiyori _Create2026/07/06~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "fade.h"
#include "manager.h"
#include "renderer.h"
#include "polygon2D.h"
#include "game.h"

//=============================
// マクロ定義
//=============================
#define	FADE_SPEED	(0.05f)	// フェードの移り変わりの速さ

//=============================
// コンストラクタ
//=============================
CFade::CFade()
{
	m_pPolygon2D = NULL;	// ポリゴン2Dへのポインタ
	m_state = STATE_FADENONE;	// フェード状態
	m_type = TYPE_NONE;	// フェードの種類
	m_modeNext = CScene::MODE_GAME;	// 次のモード
}

//=============================
// デストラクタ
//=============================
CFade::~CFade()
{

}

//=============================
// フェードの生成
//=============================
CFade* CFade::Create(TYPE type, D3DXVECTOR3 pos, D3DXCOLOR col)
{
	CFade* pFade = NULL;
	if (pFade == NULL)
	{
		pFade = new CFade;	// インスタンス生成
		if (pFade != NULL)
		{
			// 初期化処理
			pFade->Init(type, pos, col);
			return pFade;
		}
	}
	return NULL;	// 生成失敗
}

//=============================
// 初期化処理
//=============================
HRESULT CFade::Init(TYPE type, D3DXVECTOR3 pos, D3DXCOLOR col)
{
	// ポリゴンの用意
	m_pPolygon2D = CPolygon2D::Create(pos, (float)SCREEN_WIDTH * 0.5f, (float)SCREEN_HEIGHT * 0.5f, col);
	m_type = type;
	m_state = STATE_FADENONE;

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CFade::Uninit(void)
{
	// ポリゴンの破棄
	SAFE_UNINIT(m_pPolygon2D);
}

//=============================
// 更新処理
//=============================
void CFade::Update(void)
{
	// 現在のモード取得
	CScene::MODE mode = CManager::GetMode();
	CScene* pScene = CManager::GetScene();
	CGame* pGame = NULL;

	// ポリゴン情報取得
	D3DXVECTOR3 pos = m_pPolygon2D->GetPosition();	// 位置
	D3DXCOLOR col = m_pPolygon2D->GetColor();		// 色

	switch (m_type)
	{
	case TYPE_ALPHA:	// アルファ値変更
		if (m_state == STATE_FADEIN)
		{ // フェードイン状態のとき
			col.a -= FADE_SPEED; // ポリゴンを透明にする
			if (col.a <= 0.0f)
			{
				col.a = 0.0f;
				m_state = STATE_FADENONE;
			}
		}
		else if (m_state == STATE_FADEOUT)
		{ // フェードアウト状態のとき
			col.a += FADE_SPEED;	// ポリゴンを不透明にする
			if (col.a >= 1.0f)
			{
				col.a = 1.0f;
				m_state = STATE_FADEIN;
				// モードの設定
				if (mode == CScene::MODE_GAME)
				{
					pGame = static_cast<CGame*>(pScene);
					if (pGame->GetState() == CGame::STATE_CLEAR)
					{
						CManager::SetMode(m_modeNext, true);
					}
					else
					{
						CManager::SetMode(m_modeNext);
					}
				}
				else
				{
					CManager::SetMode(m_modeNext);
				}
			}
		}
		break;

	case TYPE_SHUTTER:	// シャッター[Y]

		break;
	}

	// 描画情報更新
	m_pPolygon2D->SetColor(col);
	m_pPolygon2D->SetPosition(pos);
}

//=============================
// 描画処理
//=============================
void CFade::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();
	// ライトをオフにする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ポリゴン2Dの描画
	m_pPolygon2D->Draw();

	// ライトをオンにする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//=============================
// フェードの設定
//=============================
void CFade::SetFade(TYPE type, D3DXVECTOR3 pos, D3DXCOLOR col, CScene::MODE mode, STATE state)
{
	m_type = type;
	m_state = state;
	m_modeNext = mode;
	if (state == STATE_FADEOUT)
	{ // フェードアウトの場合、アルファ値を0に設定
		col.a = 0.0f;
	}
	m_pPolygon2D->SetPosition(pos);
	m_pPolygon2D->SetColor(col);
}