//==========================================
// 
// オブジェクト処理[object.cpp]
// Author : Kajino Hiyori _Create2026/05/08~
// 
//==========================================
#include "object.h"
#include "debugproc.h"
#include "manager.h"
#include "renderer.h"

//=============================
// 静的メンバ変数宣言
//=============================
int CObject::m_nNumAll = 0;	// 最大数を初期化
CObject* CObject::m_apTop[MAX_PRIORITYS] = {};	// 先頭オブジェクトへのポインタ
CObject* CObject::m_apCur[MAX_PRIORITYS] = {};	// 最後尾オブジェクトへのポインタ

//========================================================================
// コンストラクタ[オーバーロード]
//========================================================================
CObject::CObject(PRIORITY priority)
{
	// int型にキャスト
	int nPriority = static_cast<int>(priority);
	
	if (m_apTop[nPriority] == NULL)
	{ // オブジェクトが1つも生成されていない場合
		m_apTop[nPriority] = this;	// 自分自身を先頭に設定
	}

	// 前のポインタを設定
	if (m_apTop[nPriority] != this)
	{ // 先頭ではない場合
		m_pPrev = m_apCur[nPriority];	// 生成前の最後尾のポインタを前のポインタとして登録
		CObject* pObjectPrev = m_apCur[nPriority];	// 最後尾のポインタを取得
		pObjectPrev->m_pNext = this;	// 前のポインタのpNextに、現在生成したオブジェクトのポインタを格納
	}
	else
	{ // 先頭の場合
		m_pPrev = NULL;
	}

	m_apCur[nPriority] = this;	// 自分自身を最後尾に設定
	m_pNext = NULL;	// 自分自身の次は未生成なのでNULLを設定
	m_type = TYPE_NONE;			// 種類の初期化
	m_nPriority = nPriority;			// 自身の優先順位の初期化
	m_bDeath = false;	// 死亡フラグ
	m_nNumAll++;				// 総数をカウントアップ
}

//========================================================================
// デストラクタ
//========================================================================
CObject::~CObject()
{

}

//========================================================================
// 全てのオブジェクトを破棄
//========================================================================
void CObject::ReleaseAll(void)
{
	for (int nCntPriority = 0; nCntPriority < MAX_PRIORITYS; nCntPriority++)
	{
		CObject* pObject = m_apTop[nCntPriority];	// 先頭アドレスを取得

		while (pObject != NULL)
		{ // 次のオブジェクトがある限りループ
			CObject* pObjectNext = pObject->GetNext();	// 次のオブジェクトへのポインタを設定
			// 更新処理
			pObject->Release();
			pObject = pObjectNext;	// NULL判定を行う
		}

		// 死亡フラグに応じて破棄
		pObject = m_apTop[nCntPriority];	// 先頭アドレスを再取得
		while (pObject != NULL)
		{ // 次のオブジェクトがある限りループ
			CObject* pObjectNext = pObject->GetNext();	// 次のオブジェクトへのポインタを設定
			// 破棄処理
			if (pObject->m_bDeath == true)
			{ // 死亡フラグが立っている場合
				// リストを繋ぎ直す
				if (pObject == m_apTop[nCntPriority])
				{ // 自身が先頭だった場合
					m_apTop[nCntPriority] = pObject->m_pNext;
				}
				if (pObject == m_apCur[nCntPriority])
				{ // 自身が最後尾の場合
					m_apCur[nCntPriority] = pObject->m_pPrev;
				}

				if (pObject->m_pPrev != NULL)
				{
					pObject->m_pPrev->m_pNext = pObject->m_pNext;
				}
				if (pObject->m_pNext != NULL)
				{
					pObject->m_pNext->m_pPrev = pObject->m_pPrev;
				}

				// メモリ破棄
				SAFE_UNINIT(pObject);
			}

			pObject = pObjectNext;	// NULL判定を行う
		}
	}

	if (m_nNumAll != 0)
	{// 破棄に失敗
		CDebugProc* pDebug = CManager::GetDebugProc();
		pDebug->Print("ERROR!!\n");
	}
}

//========================================================================
// 全てのオブジェクトを更新
//========================================================================
void CObject::UpdateAll(void)
{
	// デバッグ表示
	CDebugProc* pDebugProc = CManager::GetDebugProc();

	for (int nCntPriority = 0; nCntPriority < MAX_PRIORITYS; nCntPriority++)
	{
		CObject* pObject = m_apTop[nCntPriority];	// 先頭アドレスを取得
		while (pObject != NULL)
		{ // 次のオブジェクトがある限りループ
			CObject* pObjectNext = pObject->GetNext();	// 次のオブジェクトへのポインタを設定
			// 更新処理
			pObject->Update();
			pObject = pObjectNext;	// NULL判定を行う
		}

		// 死亡フラグに応じて破棄
		pObject = m_apTop[nCntPriority];	// 先頭アドレスを再取得
		while (pObject != NULL)
		{ // 次のオブジェクトがある限りループ
			CObject* pObjectNext = pObject->GetNext();	// 次のオブジェクトへのポインタを設定
			// 破棄処理
			if (pObject->m_bDeath == true)
			{ // 死亡フラグが立っている場合
				// リストを繋ぎ直す
				if (pObject == m_apTop[nCntPriority])
				{ // 自身が先頭だった場合
					m_apTop[nCntPriority] = pObject->m_pNext;
				}
				if (pObject == m_apCur[nCntPriority])
				{ // 自身が最後尾の場合
					m_apCur[nCntPriority] = pObject->m_pPrev;
				}

				if (pObject->m_pPrev != NULL)
				{
					pObject->m_pPrev->m_pNext = pObject->m_pNext;
				}
				if (pObject->m_pNext != NULL)
				{
					pObject->m_pNext->m_pPrev = pObject->m_pPrev;
				}

				// メモリ破棄
				SAFE_UNINIT(pObject);
			}
			pObject = pObjectNext;	// NULL判定を行う
		}
	}
}

//========================================================================
// 全てのオブジェクトを描画
//========================================================================
void CObject::DrawAll(void)
{
	for (int nCntPriority = 0; nCntPriority < MAX_PRIORITYS; nCntPriority++)
	{
		// デバイスの取得
		CRenderer* pRenderer = CManager::GetRenderer();
		LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

		if (nCntPriority == PRIORITY_MANAGER) continue;	// 描画を行わないクラス
		
		// レンダーステートを一括管理
		if (nCntPriority == PRIOTITY_ADDALPHA)
		{ // 加算合成
			// アルファテストを有効にする
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);	// アルファテストを有効にする
			pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// 比較方法を設定(基準値より大きい場合描画)
			pDevice->SetRenderState(D3DRS_ALPHAREF, 0);	// アルファテストの参照値を設定(この場合、0より大きい場合は描画)
		
			// アルファブレンディングを加算合成に設定
			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		
			// ライトをオフにする
			pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		}
		else if (nCntPriority == PRIOTITY_DISALPHA)
		{ // 減算合成
			// アルファテストを有効にする
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
			pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
		
			// 減算合成の設定
			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		}
		else if (nCntPriority == PRIORITY_SMOKE)
		{ // 
			// アルファテストを有効にする
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);	// アルファテストを有効にする
			pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// 比較方法を設定(基準値より大きい場合描画)
			pDevice->SetRenderState(D3DRS_ALPHAREF, 0);	// アルファテストの参照値を設定(この場合、0より大きい場合は描画)

			// ライトをオフにする
			pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		}

		CObject* pObject = m_apTop[nCntPriority];	// 先頭アドレスを取得
		while (pObject != NULL)
		{ // 次のオブジェクトがある限りループ
			CObject* pObjectNext = pObject->GetNext();	// 次のオブジェクトへのポインタを設定
			// 終了処理
			pObject->Draw();

			pObject = pObjectNext;	// NULL判定を行う
		}

		if (nCntPriority == PRIOTITY_ADDALPHA)
		{ // 加算合成
			// ライトをオンにする
			pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		
			// アルファブレンディングを元に戻す
			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		
			// アルファテストを無効にする
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);	// アルファテストを無効にする
			pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// 比較方法を設定(条件に関わらず描画)
			pDevice->SetRenderState(D3DRS_ALPHAREF, 0);	// アルファテストの参照値を設定(この場合、0より大きい場合は描画)
		}
		else if (nCntPriority == PRIOTITY_DISALPHA)
		{ // 減算合成
			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		
			// アルファテストを無効にする
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
			pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
		}
		else if (nCntPriority == PRIORITY_SMOKE)
		{
			// ライトをオンにする
			pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

			// アルファテストを無効にする
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);	// アルファテストを無効にする
			pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// 比較方法を設定(条件に関わらず描画)
			pDevice->SetRenderState(D3DRS_ALPHAREF, 0);	// アルファテストの参照値を設定(この場合、0より大きい場合は描画)
		}
	}
}

//========================================================================
// 自分自身を破棄
//========================================================================
void CObject::Release(void)
{
	if (m_bDeath == false)
	{
		m_bDeath = true;	// 死亡フラグを立てる
		m_nNumAll--;	// 総数を減少
	}
}