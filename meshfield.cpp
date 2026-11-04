//========================================================
// 
// メッシュフィールド処理[meshfield.cpp]
// Author : Kajino Hiyori _Create2026/06/12
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "meshfield.h"
#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "debugproc.h"
#include "input.h"

//=============================
// マクロ定義
//=============================
#define FILENAME		("data\\SCRIPTS\\meshfield.txt")	// スクリプト
#define	FIELD_WIDTH		(FIELD_SIZE / 2)	// 幅
#define FIELD_DEPTH		(FIELD_SIZE / 2)	// 奥行
#define PRIMITIVE		(DIVISION_W * DIVISION_D * 2 + (DIVISION_D - 1) * 4)	// プリミティブ数
#define INDEX			(PRIMITIVE + 2)											// インデックス数
#define HEIGHT			(0.0f)		// 高さ
#define HEIGHT_RAND		(5.0f)		// 高度変更
#define MOVE			(1.5f)		// 移動量
#define ROTATE			(0.05f)		// 回転量
#define TEXTURESIZE		(3.0f)		// テクスチャ読み込み幅
#define NORMAL			(D3DXVECTOR3(0.0f, 1.0f, 0.0f))	// 基本の法線
#define MAX_HEIGHT		(13.0f)	// 高度制限
#define RAND_IDX		(3)		// ランダム高度変更
#define FRAME			(120)	// フレーム管理

//=============================
// コンストラクタ
//=============================
CMeshField::CMeshField(CObject::PRIORITY priority): CObject(priority)
{
	m_pVtxBuff = NULL;		// 頂点バッファへのポインタ
	m_pIdxBuff = NULL;		// インデックスバッファへのポインタ
	m_pos = INIT_D3DVEC3;	// メッシュフィールドの位置
	m_rot = INIT_D3DVEC3;	// メッシュフィールドの向き
	m_nor = INIT_D3DVEC3;	// メッシュフィールドの法線方向
	m_nIdxTexture = -1;		// テクスチャへのインデックス
	m_pFileName = NULL;			// ファイル名
	m_bBump = false;			// 凸中
	m_bDent = false;			// 凹中
	memset(&m_aPosVtx[0], 0, sizeof m_aPosVtx);	// 頂点座標
	memset(&m_aNor[0], 0, sizeof m_aNor);		// 法線情報
	m_posReference = INIT_D3DVEC3;
	m_fReferenceRadius = 0.0f;
}

//=============================
// デストラクタ
//=============================
CMeshField::~CMeshField()
{

}

//=============================
// メッシュフィールド生成処理
//=============================
CMeshField* CMeshField::Create(D3DXVECTOR3 pos, TYPE type)
{
	CMeshField* pMeshField = NULL;	// オブジェクトへのポインタを用意
	if (pMeshField == NULL)
	{ // NULLが設定できている場合
		pMeshField = new CMeshField;
		if (pMeshField != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pMeshField->Init(pos, type);
			return pMeshField;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CMeshField::Init(D3DXVECTOR3 pos, TYPE type)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 初期化
	m_pos = pos;
	m_rot = INIT_D3DVEC3;
	m_pVtxBuff = NULL;
	m_pIdxBuff = NULL;
	m_fWidth = FIELD_SIZE * 0.5f;
	m_fDepth = FIELD_SIZE * 0.5f;
	m_nMaxHeight = (int)MAX_HEIGHT;
	m_fHeightValue = HEIGHT_RAND;
	m_fLoadTexRange = TEXTURESIZE;

	// テクスチャの読み込み
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = -1;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * VERTEX,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,									// 頂点フォーマット
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点バッファを生成
	SetVtxBuff(type);

	// インデックスバッファの生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * INDEX,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	// インデックスバッファを生成
	SetIdxBuff();

	return S_OK;
}

//=============================
// 初期化処理
//=============================
HRESULT CMeshField::Init(D3DXVECTOR3 pos, TYPE type, float fWidth, float fDepth, int nDivision, int nMaxHeight, float fHeightValue, float fTexSize)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 初期化
	m_pos = pos;
	m_rot = INIT_D3DVEC3;
	m_pVtxBuff = NULL;
	m_pIdxBuff = NULL;
	m_fWidth = fWidth;
	m_fDepth = fDepth;
	m_nMaxHeight = nMaxHeight;
	m_fHeightValue = fHeightValue;
	m_fLoadTexRange = fTexSize;
	m_nDivision = nDivision;			// 分割数
	m_nDivisionAdd1 = m_nDivision + 1;	// 分割数+1
	m_nNumVertex = m_nDivisionAdd1 * m_nDivisionAdd1;		// 頂点数
	m_nNumPrimitive = m_nDivision * m_nDivision * 2 + (m_nDivision - 1) * 4;	// プリミティブ数
	m_nNumIdx = m_nNumPrimitive + 2;			// インデックス数

	// テクスチャの読み込み
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = -1;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVertex,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,									// 頂点フォーマット
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点バッファを生成
	SetVtxBuff(type);

	// インデックスバッファの生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIdx,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	// インデックスバッファを生成
	SetIdxBuff();

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CMeshField::Uninit(void)
{
	// テクスチャの破棄
	m_nIdxTexture = -1;
	
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);

	// インデックスバッファの破棄
	SAFE_RELEASE(m_pIdxBuff);
}

//=============================
// 更新処理
//=============================
void CMeshField::Update(void)
{
	// 頂点バッファ設定
	SetVtxBuff();
}

//=============================
// 描画処理
//=============================
void CMeshField::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャの読み込み
	CTexture* pTexture = CManager::GetTexture();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス

	// ワールドマトリックスの初期化(デフォルトの値にする)
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// メッシュフィールドの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVertex, 0, m_nNumIdx);		// 描画するプリミティブの数
}

//=============================
// 高度設定[処理負荷 : 高]
// ->基準点/判定範囲を利用し
// 基準高度に合わせてのみの
// 判定と高低差の判定に対応
//=============================
float CMeshField::GetHeight(D3DXVECTOR3 pos, D3DXVECTOR3* pNor)
{
	// 距離判定
	float fDistance = sqrtf(((m_posReference.x - pos.x) * (m_posReference.x - pos.x)) + ((m_posReference.z - pos.z) * (m_posReference.z - pos.z)));
	if (m_fReferenceRadius < fDistance)
	{ // 凹み半径外であれば外積処理を弾く
		return m_pos.y;
	}

	// デバッグ情報
	CDebugProc* pDebugProc = CManager::GetDebugProc();

	float fHeight = 0.0f;			// 返り値
	D3DXVECTOR3 vecToPos[MAX_VERTEX];	// 対象物へのベクトル
	D3DXVECTOR3 vecLine0, vecLine1, vecLine2;	// 三角形の範囲指定

	if (pos.x < -m_fWidth || pos.x > m_fWidth || pos.z < -m_fDepth || pos.z > m_fDepth)
	{ // 吹き飛ばし対策
		return fHeight;
	}

	for (int nCntVtx = 0; nCntVtx < m_nNumVertex; nCntVtx++)
	{
		vecToPos[nCntVtx] = pos - m_aPosVtx[nCntVtx];	// 各頂点から対象物へのベクトルを格納
	}

	// どの区画の上に乗っているかを判定
	int nRateX = static_cast<int>((m_aPosVtx[m_nDivision].x + pos.x) / (m_aPosVtx[1].x - m_aPosVtx[0].x));				// メッシュ上における対象物の位置[X]
	int nRateZ = static_cast<int>((m_aPosVtx[m_nDivision].z - pos.z) / (m_aPosVtx[0].z - m_aPosVtx[m_nDivisionAdd1].z));	// メッシュ上における対象物の位置[Z]
	int mainVtx = nRateZ * m_nDivisionAdd1 + nRateX;

	if (mainVtx > m_nNumVertex || mainVtx < 0)
	{
		return m_pos.y;
	}

	// 三角形判定△
	vecLine0 = m_aPosVtx[mainVtx + m_nDivisionAdd1] - m_aPosVtx[mainVtx];
	vecLine1 = m_aPosVtx[mainVtx + m_nDivisionAdd1 + 1] - m_aPosVtx[mainVtx + m_nDivisionAdd1];
	vecLine2 = m_aPosVtx[mainVtx] - m_aPosVtx[mainVtx + m_nDivisionAdd1 + 1];

	if ((vecLine0.z * vecToPos[mainVtx].x) - (vecLine0.x * vecToPos[mainVtx].z) <= 0.0f
		&& (vecLine1.z * vecToPos[mainVtx + m_nDivisionAdd1].x) - (vecLine1.x * vecToPos[mainVtx + m_nDivisionAdd1].z) <= 0.0f
		&& (vecLine2.z * vecToPos[mainVtx + m_nDivisionAdd1 + 1].x) - (vecLine2.x * vecToPos[mainVtx + m_nDivisionAdd1 + 1].z) <= 0.0f)
	{ // 対象物がポリゴン上にある
		D3DXVECTOR3 nor;	// 面の法線を格納
		D3DXVECTOR3 vec1 = m_aPosVtx[mainVtx] - m_aPosVtx[mainVtx + m_nDivisionAdd1];
		D3DXVECTOR3 vec2 = m_aPosVtx[mainVtx + m_nDivisionAdd1 + 1] - m_aPosVtx[mainVtx + m_nDivisionAdd1];
		D3DXVec3Cross(&nor, &vec1, &vec2);
		D3DXVec3Normalize(&nor, &nor);

		if (m_aNor[mainVtx + m_nDivisionAdd1].y != 0.0f)
		{ // 法線yが0.0fでない場合[0割り対策]
			fHeight = ((-(pos.x - m_aPosVtx[mainVtx + m_nDivisionAdd1].x) * nor.x - (pos.z - m_aPosVtx[mainVtx + m_nDivisionAdd1].z) * nor.z) / nor.y) + m_aPosVtx[mainVtx + m_nDivisionAdd1].y;
			if (pNor != NULL)
			{
				pNor = &nor;
			}
			m_nor = nor;
			return fHeight;
		}
	}

	// 逆三角形判定▽
	vecLine0 = m_aPosVtx[mainVtx + 1] - m_aPosVtx[mainVtx];
	vecLine1 = m_aPosVtx[mainVtx + m_nDivisionAdd1 + 1] - m_aPosVtx[mainVtx + 1];
	vecLine2 = m_aPosVtx[mainVtx] - m_aPosVtx[mainVtx + m_nDivisionAdd1 + 1];

	if ((vecLine0.z * vecToPos[mainVtx].x) - (vecLine0.x * vecToPos[mainVtx].z) >= 0.0f
		&& (vecLine1.z * vecToPos[mainVtx + 1].x) - (vecLine1.x * vecToPos[mainVtx + 1].z) >= 0.0f
		&& (vecLine2.z * vecToPos[mainVtx + m_nDivisionAdd1 + 1].x) - (vecLine2.x * vecToPos[mainVtx + m_nDivisionAdd1 + 1].z) >= 0.0f)
	{ // 対象物がポリゴン上にある
		D3DXVECTOR3 nor;	// 面の法線を格納
		D3DXVECTOR3 vec1 = m_aPosVtx[mainVtx + m_nDivisionAdd1 + 1] - m_aPosVtx[mainVtx + 1];
		D3DXVECTOR3 vec2 = m_aPosVtx[mainVtx] - m_aPosVtx[mainVtx + 1];
		D3DXVec3Cross(&nor, &vec1, &vec2);
		D3DXVec3Normalize(&nor, &nor);

		if (m_aNor[mainVtx + 1].y != 0.0f)
		{ // 法線yが0.0fでない場合[0割り対策]
			fHeight = ((-(pos.x - m_aPosVtx[mainVtx + 1].x) * nor.x - (pos.z - m_aPosVtx[mainVtx + 1].z) * nor.z) / nor.y) + m_aPosVtx[mainVtx + 1].y;
			if (pNor != NULL)
			{
				pNor = &nor;
			}
			m_nor = nor;
			return fHeight;
		}
	}

	return fHeight;
}

//=============================
// 面を凹ませる
//=============================
void CMeshField::Dent(D3DXVECTOR3 pos, float fSize, float fDepth)
{
	// 使用されている頂点のと距離で凹み具合を管理
	for (int nCntVtx = 0; nCntVtx < m_nNumVertex; nCntVtx++)
	{
		float fDistance = sqrtf(((m_aPosVtx[nCntVtx].x - pos.x) * (m_aPosVtx[nCntVtx].x - pos.x)) + ((m_aPosVtx[nCntVtx].z - pos.z) * (m_aPosVtx[nCntVtx].z - pos.z)));	// 頂点と対象の距離を計算
		if (fDistance > fSize)
		{
			m_aPosDestVtx[nCntVtx].y = 0.0f;
			continue;	// 遠すぎる場合は頂点情報をリセット
		}

		// 距離に応じて高度を	
  		m_aPosDestVtx[nCntVtx].y = (1.0f - (fDistance / fSize)) * -fDepth;
	}

	// 頂点バッファ設定
	SetVtxBuff();
	m_bDent = true;
}

//=============================
// 面を突きあげる
//=============================
void CMeshField::Bump(D3DXVECTOR3 pos, float fSize, float fPeak)
{
	// 使用されている頂点のと距離で凹み具合を管理
	for (int nCntVtx = 0; nCntVtx < m_nNumVertex; nCntVtx++)
	{
		float fDistance = sqrtf(((m_aPosVtx[nCntVtx].x - pos.x) * (m_aPosVtx[nCntVtx].x - pos.x)) + ((m_aPosVtx[nCntVtx].z - pos.z) * (m_aPosVtx[nCntVtx].z - pos.z)));	// 頂点と対象の距離を計算
		m_aPosDestVtx[nCntVtx].y = 0.0f;
		if (fDistance > fSize) continue;	// 遠すぎる場合は頂点情報を維持

		// 距離に応じて高度を	変更
		if (m_aPosVtx[nCntVtx].y <= 0.0f)
		{
			m_aPosVtx[nCntVtx].y = 0.0f;
		}
		m_aPosVtx[nCntVtx].y += (1.0f - (fDistance / fSize)) * fPeak;
	}

	// 頂点バッファ設定
	SetVtxBuff();
	m_bDent = false;
	m_bBump = true;
	m_nCntFrame = 0;
}

//=============================
// 頂点バッファ設定
//=============================
void CMeshField::SetVtxBuff(void)
{
	if (m_bBump == true || m_bDent == true)
	{
		m_nCntFrame++;
	}

	// 目的値に向けて頂点情報を更新
	float fRate = (float)m_nCntFrame / (float)FRAME;
	CManager::GetDebugProc()->Print("m_nCntFrame %d : fRate %f\n", m_nCntFrame,  fRate);

	for (int nCntVtx = 0; nCntVtx < m_nNumVertex; nCntVtx++)
	{
		float fDiffY = m_aPosDestVtx[nCntVtx].y - m_aPosVtx[nCntVtx].y;
		m_aPosVtx[nCntVtx].y = m_aPosVtx[nCntVtx].y + fDiffY * fRate;
	}

	VERTEX_3D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点バッファの設定
	for (int nCntVtx = 0; nCntVtx < m_nNumVertex; nCntVtx++)
	{

		// 頂点座標の設定
		pVtx[nCntVtx].pos = m_aPosVtx[nCntVtx];

		// 法線の設定
		pVtx[nCntVtx].nor = m_aNor[nCntVtx];

		// 頂点カラーの設定
		pVtx[nCntVtx].col = COLOR_WHITE;
	}

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	if (m_nCntFrame >= FRAME)
	{
		m_bBump = false;
		m_nCntFrame = 0;
	}
}

//=============================
// 頂点バッファ設定
//=============================
void CMeshField::SetVtxBuff(TYPE type)
{
	VERTEX_3D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntDepth = 0; nCntDepth < m_nDivisionAdd1; nCntDepth++)
	{
		for (int nCntWidth = 0; nCntWidth < m_nDivisionAdd1; nCntWidth++)
		{
			// 頂点座標の設定
			m_aPosVtx[nCntWidth + nCntDepth * m_nDivisionAdd1].x = m_fWidth / m_nDivision * nCntWidth - m_fWidth / m_nDivision * m_nDivision / 2;
			m_aPosVtx[nCntWidth + nCntDepth * m_nDivisionAdd1].y = 0.0f;
			m_aPosVtx[nCntWidth + nCntDepth * m_nDivisionAdd1].z = m_fDepth / m_nDivision * -nCntDepth + m_fDepth / m_nDivision * m_nDivision / 2;

			// 高度のランダム化
			if (type == TYPE_UNEVEN)
			{ // 凹凸有
				m_aPosVtx[nCntWidth + nCntDepth * m_nDivisionAdd1].y = rand() % m_nMaxHeight * m_fHeightValue;
				m_aPosDestVtx[nCntWidth + nCntDepth * m_nDivisionAdd1].y = m_aPosVtx[nCntWidth + nCntDepth * m_nDivisionAdd1].y;
			}
			else
			{ // 平面
				m_aPosVtx[nCntWidth + nCntDepth * m_nDivisionAdd1].y = 0.0f;
			}

			// テクスチャ座標の設定
			pVtx[nCntWidth + nCntDepth * m_nDivisionAdd1].tex.x = (float)(nCntWidth) *m_fLoadTexRange;
			pVtx[nCntWidth + nCntDepth * m_nDivisionAdd1].tex.y = (float)(nCntDepth) *m_fLoadTexRange;
		}
	}


	// 法線設定
	SetNormalize();

	// 頂点バッファの設定
	for (int nCntVtx = 0; nCntVtx < m_nNumVertex; nCntVtx++)
	{

		// 頂点座標の設定
		pVtx[nCntVtx].pos = m_aPosVtx[nCntVtx];

		// 法線の設定
		pVtx[nCntVtx].nor = m_aNor[nCntVtx];

		// 頂点カラーの設定
		pVtx[nCntVtx].col = COLOR_WHITE;
	}

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//=============================
// インデックスバッファ設定
//=============================
void CMeshField::SetIdxBuff(void)
{
	WORD* pIdx;	// インデックス情報へのポインタ	
	// インデックスバッファをロックし、頂点番号へのポインタを取得
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	int nCntDepth = 0;	// 段ずれの回数をカウント

	// 頂点番号データを設定
	for (int nCntVtx = 0; nCntVtx < m_nNumIdx / 2; nCntVtx++)
	{
		if (nCntVtx == 0)
		{
			pIdx[0] = nCntVtx + m_nDivisionAdd1;
			pIdx[1] = nCntVtx;
		}
		else if (nCntVtx % (m_nDivision + 2) == m_nDivisionAdd1)
		{
			nCntDepth++;	// 段ずれの回数を追加
			pIdx[0] = nCntVtx - nCntDepth;
			pIdx[1] = nCntVtx - nCntDepth + (m_nDivision + 2);
		}
		else
		{
			pIdx[0] = nCntVtx - nCntDepth + m_nDivisionAdd1;
			pIdx[1] = nCntVtx - nCntDepth;
		}
		pIdx += 2;
	}

	// インデックスバッファをアンロック
	m_pIdxBuff->Unlock();
}

//=============================
// 法線設定
//=============================
void CMeshField::SetNormalize(void)
{
	D3DXVECTOR3 nor, nor1, nor2, nor3, nor4, nor5, nor6;	// 法線を格納
	D3DXVECTOR3 vec1, vec2;	// 方向ベクトルを格納
	
	for (int nCntDepth = 0; nCntDepth < m_nDivisionAdd1; nCntDepth++)
	{
		for (int nCntWidth = 0; nCntWidth < m_nDivisionAdd1; nCntWidth++)
		{
			// 法線を必要とする頂点を中心とした際の頂点情報
			int nLeftTop		= (nCntDepth - 1)	* m_nDivisionAdd1 + (nCntWidth - 1);		// 左上頂点
			int nCenterTop		= (nCntDepth - 1)	* m_nDivisionAdd1 + nCntWidth;			// 中央上頂点
			int nRightTop		= (nCntDepth - 1)	* m_nDivisionAdd1 + (nCntWidth + 1);		// 右上頂点
			int nLeftMiddle		= nCntDepth			* m_nDivisionAdd1 + (nCntWidth - 1);		// 左中段頂点
			int nCenterMiddle	= nCntDepth			* m_nDivisionAdd1 + nCntWidth;			// 中央中段頂点
			int nRightMiddle	= nCntDepth			* m_nDivisionAdd1 + (nCntWidth + 1);		// 右中段頂点
			int nLeftDown		= (nCntDepth + 1)	* m_nDivisionAdd1 + (nCntWidth - 1);		// 左下頂点
			int nCenterDown		= (nCntDepth + 1)	* m_nDivisionAdd1 + nCntWidth;			// 中央下頂点
			int nRightDown		= (nCntDepth + 1)	* m_nDivisionAdd1 + (nCntWidth + 1);		// 右下頂点

			if (nCntWidth == 0 && nCntDepth == 0)
			{ // 左上
				vec1 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nCenterDown];
				vec2 = m_aPosVtx[nRightDown] - m_aPosVtx[nCenterDown];
				D3DXVec3Cross(&nor1, &vec1, &vec2);
				D3DXVec3Normalize(&nor1, &nor1);
				vec1 = m_aPosVtx[nRightDown] - m_aPosVtx[nRightMiddle];
				vec2 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nRightMiddle];
				D3DXVec3Cross(&nor2, &vec1, &vec2);
				D3DXVec3Normalize(&nor2, &nor2);
				nor = (nor1 + nor2) / 2.0f;
			}
			else if (nCntWidth == m_nDivision && nCntDepth == 0)
			{ // 右上
				vec1 = m_aPosVtx[nCenterDown] - m_aPosVtx[nCenterMiddle];
				vec2 = m_aPosVtx[nLeftMiddle] - m_aPosVtx[nCenterMiddle];
				D3DXVec3Cross(&nor, &vec1, &vec2);
			}
			else if (nCntWidth == 0 && nCntDepth == m_nDivision)
			{ // 左下
				vec1 = m_aPosVtx[nCenterTop] - m_aPosVtx[nCenterMiddle];
				vec2 = m_aPosVtx[nRightMiddle] - m_aPosVtx[nCenterMiddle];
				D3DXVec3Cross(&nor, &vec1, &vec2);
			}
			else if (nCntWidth == m_nDivision && nCntDepth == m_nDivision)
			{ // 右下
				vec1 = m_aPosVtx[nLeftTop] - m_aPosVtx[nLeftMiddle];
				vec2 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nLeftMiddle];
				D3DXVec3Cross(&nor1, &vec1, &vec2);
				D3DXVec3Normalize(&nor1, &nor1);
				vec1 = m_aPosVtx[nLeftMiddle] - m_aPosVtx[nCenterTop];
				vec2 = m_aPosVtx[nLeftTop] - m_aPosVtx[nCenterTop];
				D3DXVec3Cross(&nor2, &vec1, &vec2);
				D3DXVec3Normalize(&nor2, &nor2);
				nor = (nor1 + nor2) / 2.0f;
			}
			else if (nCntDepth == 0)
			{ // 上端[左上/右上を除く]
				vec1 = m_aPosVtx[nCenterDown] - m_aPosVtx[nCenterMiddle];
				vec2 = m_aPosVtx[nLeftMiddle] - m_aPosVtx[nCenterMiddle];
				D3DXVec3Cross(&nor1, &vec1, &vec2);
				D3DXVec3Normalize(&nor1, &nor1);
				vec1 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nCenterDown];
				vec2 = m_aPosVtx[nRightDown] - m_aPosVtx[nCenterDown];
				D3DXVec3Cross(&nor2, &vec1, &vec2);
				D3DXVec3Normalize(&nor2, &nor2);
				vec1 = m_aPosVtx[nRightDown] - m_aPosVtx[nRightMiddle];
				vec2 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nRightMiddle];
				D3DXVec3Cross(&nor3, &vec1, &vec2);
				D3DXVec3Normalize(&nor3, &nor3);
				nor = (nor1 + nor2 + nor3) / 3.0f;
			}
			else if (nCntDepth == m_nDivision)
			{ // 下端[左下/右下を除く]
				vec1 = m_aPosVtx[nCenterTop] - m_aPosVtx[nCenterMiddle];
				vec2 = m_aPosVtx[nRightMiddle] - m_aPosVtx[nCenterMiddle];
				D3DXVec3Cross(&nor1, &vec1, &vec2);
				D3DXVec3Normalize(&nor1, &nor1);
				vec1 = m_aPosVtx[nLeftTop] - m_aPosVtx[nLeftMiddle];
				vec2 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nLeftMiddle];
				D3DXVec3Cross(&nor2, &vec1, &vec2);
				D3DXVec3Normalize(&nor2, &nor2);
				vec1 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nCenterTop];
				vec2 = m_aPosVtx[nLeftTop] - m_aPosVtx[nCenterTop];
				D3DXVec3Cross(&nor3, &vec1, &vec2);
				D3DXVec3Normalize(&nor3, &nor3);
				nor = (nor1 + nor2 + nor3) / 3.0f;
			}
			else if (nCntWidth == 0)
			{ // 左端
				vec1 = m_aPosVtx[nCenterTop] - m_aPosVtx[nCenterMiddle];
				vec2 = m_aPosVtx[nRightMiddle] - m_aPosVtx[nCenterMiddle];
				D3DXVec3Cross(&nor1, &vec1, &vec2);
				D3DXVec3Normalize(&nor1, &nor1);
				vec1 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nCenterDown];
				vec2 = m_aPosVtx[nRightDown] - m_aPosVtx[nCenterDown];
				D3DXVec3Cross(&nor2, &vec1, &vec2);
				D3DXVec3Normalize(&nor2, &nor2);
				vec1 = m_aPosVtx[nRightDown] - m_aPosVtx[nRightMiddle];
				vec2 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nRightMiddle];
				D3DXVec3Cross(&nor3, &vec1, &vec2);
				D3DXVec3Normalize(&nor3, &nor3);
				nor = (nor1 + nor2 + nor3) / 3.0f;
			}
			else if (nCntWidth == m_nDivision)
			{ // 右端
				vec1 = m_aPosVtx[nCenterDown] - m_aPosVtx[nCenterMiddle];
				vec2 = m_aPosVtx[nLeftMiddle] - m_aPosVtx[nCenterMiddle];
				D3DXVec3Cross(&nor1, &vec1, &vec2);
				D3DXVec3Normalize(&nor1, &nor1);
				vec1 = m_aPosVtx[nLeftTop] - m_aPosVtx[nLeftMiddle];
				vec2 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nLeftMiddle];
				D3DXVec3Cross(&nor2, &vec1, &vec2);
				D3DXVec3Normalize(&nor2, &nor2);
				vec1 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nCenterTop];
				vec2 = m_aPosVtx[nLeftTop] - m_aPosVtx[nCenterTop];
				D3DXVec3Cross(&nor3, &vec1, &vec2);
				D3DXVec3Normalize(&nor3, &nor3);
				nor = (nor1 + nor2 + nor3) / 3.0f;
			}
			else
			{ // 6つの法線を持つ頂点
				vec1 = m_aPosVtx[nCenterTop] - m_aPosVtx[nCenterMiddle];
				vec2 = m_aPosVtx[nRightMiddle] - m_aPosVtx[nCenterMiddle];
				D3DXVec3Cross(&nor1, &vec1, &vec2);
				D3DXVec3Normalize(&nor1, &nor1);
				vec1 = m_aPosVtx[nCenterDown] - m_aPosVtx[nCenterMiddle];
				vec2 = m_aPosVtx[nLeftMiddle] - m_aPosVtx[nCenterMiddle];
				D3DXVec3Cross(&nor2, &vec1, &vec2);
				D3DXVec3Normalize(&nor2, &nor2);
				vec1 = m_aPosVtx[nLeftTop] - m_aPosVtx[nLeftMiddle];
				vec2 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nLeftMiddle];
				D3DXVec3Cross(&nor3, &vec1, &vec2);
				D3DXVec3Normalize(&nor3, &nor3);
				vec1 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nCenterTop];
				vec2 = m_aPosVtx[nLeftTop] - m_aPosVtx[nCenterTop];
				D3DXVec3Cross(&nor4, &vec1, &vec2);
				D3DXVec3Normalize(&nor4, &nor4);
				vec1 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nCenterDown];
				vec2 = m_aPosVtx[nRightDown] - m_aPosVtx[nCenterDown];
				D3DXVec3Cross(&nor5, &vec1, &vec2);
				D3DXVec3Normalize(&nor5, &nor5);
				vec1 = m_aPosVtx[nRightDown] - m_aPosVtx[nRightMiddle];
				vec2 = m_aPosVtx[nCenterMiddle] - m_aPosVtx[nRightMiddle];
				D3DXVec3Cross(&nor6, &vec1, &vec2);
				D3DXVec3Normalize(&nor6, &nor6);
				nor = (nor1 + nor2 + nor3 + nor4 + nor5 + nor6) / 6.0f;
			}
			// 法線の正規化
			D3DXVec3Normalize(&nor, &nor);
			int nIdx = nCntDepth * m_nDivisionAdd1 + nCntWidth;
			m_aNor[nCntDepth * m_nDivisionAdd1 + nCntWidth] = nor;
		}
	}
}
