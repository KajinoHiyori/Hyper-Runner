//========================================================
// 
// メッシュスキン[平面]処理[meshskin_flat.cpp]
// Author : Kajino Hiyori _Create2026/11/11
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "meshskin_flat.h"
#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "debugproc.h"
#include "input.h"
#include "loadscripts.h"

//=============================
// マクロ定義
//=============================
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
// ファイル読み込み=======================================================
#define MESHSKIN_INFO		("MESHSKIN_INFO")		// MESHSKIN_INFO
#define END_MESHSKIN_INFO	("END_MESHSKIN_INFO")	// END_MESHSKIN_INFO
#define NUM_VERTEX			("NUM_VERTEX")			// NUM_VERTEX
#define BONE_INFO		("BONE_INFO")		// BONE_INFO
#define END_BONE_INFO	("END_BONE_INFO")	// END_BONE_INFO
#define NUM_BONE		("NUM_BONE")		// NUM_BONE
#define BONESET			("BONESET")			// BONESET
#define END_BONESET		("END_BONESET")		// END_BONESET
#define VERTEX_INFO		("VERTEX_INFO")		// VERTEX_INFO
#define END_VERTEX_INFO	("END_VERTEX_INFO")	// END_VERTEX_INFO
#define SETVERTEX		("SETVERTEX")		// SETVERTEX
#define END_SETVERTEX	("END_SETVERTEX")	// END_SETVERTEX
#define WEIGHT			("WEIGHT")			// WEIGHT

//=============================
// コンストラクタ
//=============================
CMeshSkinFlat::CMeshSkinFlat(CObject::PRIORITY priority): CObject(priority)
{
	m_pVtxBuff = NULL;		// 頂点バッファへのポインタ
	m_pos = INIT_D3DVEC3;	// メッシュスキン[平面]の位置
	m_rot = INIT_D3DVEC3;	// メッシュスキン[平面]の向き
	m_nor = INIT_D3DVEC3;	// メッシュスキン[平面]の法線方向
	m_nIdxTexture = -1;		// テクスチャへのインデックス
	for (int nCntBone = 0; nCntBone < MAX_FLATBONE; nCntBone++)
	{
		m_aBone[nCntBone] = {};
		for (int nCntVtx = 0; nCntVtx < MAX_FLATVERTEX; nCntVtx++)
		{
			m_aPosOffset[nCntBone][nCntVtx] = INIT_D3DVEC3;	// 頂点座標
			m_aWeight[nCntBone][nCntVtx] = 0.0f;			// ボーンごとの影響力
			m_aPosVtx[nCntVtx] = INIT_D3DVEC3;				// 頂点座標
		}
	}
	m_pFileName = NULL;			// ファイル名
}

//=============================
// デストラクタ
//=============================
CMeshSkinFlat::~CMeshSkinFlat()
{

}

//=============================
// メッシュスキン[平面]生成処理
//=============================
CMeshSkinFlat* CMeshSkinFlat::Create(D3DXVECTOR3 pos)
{
	CMeshSkinFlat* pMeshField = NULL;	// オブジェクトへのポインタを用意
	if (pMeshField == NULL)
	{ // NULLが設定できている場合
		pMeshField = new CMeshSkinFlat;
		if (pMeshField != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pMeshField->Init(pos);
			return pMeshField;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// メッシュスキン[平面]生成処理
//=============================
CMeshSkinFlat* CMeshSkinFlat::Create(const char* pFilePath)
{
	CMeshSkinFlat* pMeshField = NULL;	// オブジェクトへのポインタを用意
	if (pMeshField == NULL)
	{ // NULLが設定できている場合
		pMeshField = new CMeshSkinFlat;
		if (pMeshField != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pMeshField->Init(pFilePath);
			return pMeshField;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CMeshSkinFlat::Init(D3DXVECTOR3 pos)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 初期化
	m_pos = pos;
	m_rot = INIT_D3DVEC3;
	m_pVtxBuff = NULL;
	m_fLoadTexRange = 1.0f;
	m_nNumVtx = MAX_FLATVERTEX;
	m_nNumBone = MAX_FLATBONE;

	// テクスチャの読み込み
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = -1;

	// ボーン情報設定
	m_aBone[0].pos = INIT_D3DVEC3;
	m_aBone[0].rot = INIT_D3DVEC3;
	m_aBone[1].pos = D3DXVECTOR3(0.0f, 30.0f, 0.0f);
	m_aBone[1].rot = INIT_D3DVEC3;

	// ボーンからのオフセットを算出
	m_aPosOffset[0][0] = D3DXVECTOR3(-10.0f, 0.0f, 0.0f);
	m_aPosOffset[0][1] = D3DXVECTOR3(10.0f, 0.0f, 0.0f);
	m_aPosOffset[0][2] = D3DXVECTOR3(-10.0f, 30.0f, 0.0f);
	m_aPosOffset[0][3] = D3DXVECTOR3(10.0f, 30.0f, 0.0f);
	m_aPosOffset[0][4] = D3DXVECTOR3(-10.0f, 60.0f, 0.0f);
	m_aPosOffset[0][5] = D3DXVECTOR3(10.0f, 60.0f, 0.0f);
	m_aPosOffset[0][6] = D3DXVECTOR3(-10.0f, 90.0f, 0.0f);
	m_aPosOffset[0][7] = D3DXVECTOR3(10.0f, 90.0f, 0.0f);
	m_aPosOffset[0][8] = D3DXVECTOR3(-10.0f, 120.0f, 0.0f);
	m_aPosOffset[0][9] = D3DXVECTOR3(10.0f, 120.0f, 0.0f);

	m_aPosOffset[1][0] = D3DXVECTOR3(-10.0f, -30.0f, 0.0f);
	m_aPosOffset[1][1] = D3DXVECTOR3(10.0f, -30.0f, 0.0f);
	m_aPosOffset[1][2] = D3DXVECTOR3(-10.0f, 0.0f, 0.0f);
	m_aPosOffset[1][3] = D3DXVECTOR3(10.0f, 0.0f, 0.0f);
	m_aPosOffset[1][4] = D3DXVECTOR3(-10.0f, 30.0f, 0.0f);
	m_aPosOffset[1][5] = D3DXVECTOR3(10.0f, 30.0f, 0.0f);
	m_aPosOffset[1][6] = D3DXVECTOR3(-10.0f, 60.0f, 0.0f);
	m_aPosOffset[1][7] = D3DXVECTOR3(10.0f, 60.0f, 0.0f);
	m_aPosOffset[1][8] = D3DXVECTOR3(-10.0f, 90.0f, 0.0f);
	m_aPosOffset[1][9] = D3DXVECTOR3(10.0f, 90.0f, 0.0f);

	// ボーンからの影響力を設定
	m_aWeight[0][0] = 1.0f;
	m_aWeight[0][1] = 1.0f;
	m_aWeight[0][2] = 0.75f;
	m_aWeight[0][3] = 0.75f;
	m_aWeight[0][4] = 0.5f;
	m_aWeight[0][5] = 0.5f;
	m_aWeight[0][6] = 0.25f;
	m_aWeight[0][7] = 0.25f;
	m_aWeight[0][8] = 0.0f;
	m_aWeight[0][9] = 0.0f;

	for (int nCntVtx = 0; nCntVtx < m_nNumVtx; nCntVtx++)
	{
		m_aWeight[1][nCntVtx] = 1.0f - m_aWeight[0][nCntVtx];
	}


	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVtx,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,									// 頂点フォーマット
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点バッファを生成
	SetVtxBuff();

	return S_OK;
}

//=============================
// 初期化処理
//=============================
HRESULT CMeshSkinFlat::Init(const char* pFilePath)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 外部ファイル読み込み
	if (LoadScripts(pFilePath) == false) return E_FAIL;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVtx,	// 確保するバッファのサイズ
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,									// 頂点フォーマット
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点バッファを生成
	SetVtxBuff();

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CMeshSkinFlat::Uninit(void)
{
	// テクスチャの破棄
	m_nIdxTexture = -1;
	
	// 頂点バッファの破棄
	SAFE_RELEASE(m_pVtxBuff);
}

//=============================
// 更新処理
//=============================
void CMeshSkinFlat::Update(void)
{
	if (CManager::GetInputKeyboard()->GetPress(DIK_LSHIFT) == true)
	{
		m_aBone[1].rot.z += 0.02f;
		m_aBone[1].rot.z = AngleNormalize(m_aBone[1].rot.z);
	}
	if (CManager::GetInputKeyboard()->GetPress(DIK_RSHIFT) == true)
	{
		m_aBone[1].rot.z = 0.0f;
	}
}

//=============================
// 描画処理
//=============================
void CMeshSkinFlat::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャの読み込み
	CTexture* pTexture = CManager::GetTexture();

	// カリングをオフにする
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	D3DXMATRIX mtxRot, mtxTrans, mtxParent;	// 計算用マトリックス

	// ワールドマトリックスの初期化(デフォルトの値にする)
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// 親マトリックスを指定
	mtxParent = m_mtxWorld;

	for (int nCntVtx = 0; nCntVtx < m_nNumVtx; nCntVtx++)
	{
		m_aPosVtx[nCntVtx] = INIT_D3DVEC3;
	}

	for (int nCntBone = 0; nCntBone < m_nNumBone; nCntBone++)
	{
		// ポリゴンの形状を設定
		D3DXMATRIX mtxBone;

		// ワールドマトリックスの初期化(デフォルトの値にする)
		D3DXMatrixIdentity(&mtxBone);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, m_aBone[nCntBone].rot.y, m_aBone[nCntBone].rot.x, m_aBone[nCntBone].rot.z);
		D3DXMatrixMultiply(&mtxBone, &mtxBone, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, m_aBone[nCntBone].pos.x, m_aBone[nCntBone].pos.y, m_aBone[nCntBone].pos.z);
		D3DXMatrixMultiply(&mtxBone, &mtxBone, &mtxTrans);

		// 親マトリックスと掛け合わせる
		D3DXMatrixMultiply(&mtxParent, &mtxParent, &mtxBone);

		for (int nCntVtx = 0; nCntVtx < m_nNumVtx; nCntVtx++)
		{// 全頂点データの算出
			D3DXVECTOR3 posVtx = INIT_D3DVEC3;
			D3DXVec3TransformCoord(&posVtx, &m_aPosOffset[nCntBone][nCntVtx], &mtxBone);
			m_aPosVtx[nCntVtx] += posVtx * m_aWeight[nCntBone][nCntVtx];
		}
	}

	// 描画用頂点情報の設定
	SetDrawVtxBuff();

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// 描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, m_nNumVtx -2);

	// カリングを元に戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//=============================
// 頂点バッファ設定
//=============================
void CMeshSkinFlat::SetVtxBuff(void)
{
	VERTEX_3D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	D3DXCOLOR aCol[MAX_FLATVERTEX];
	aCol[0] = COLOR_RED;
	aCol[1] = COLOR_RED;
	aCol[2] = COLOR_YELLOW;
	aCol[3] = COLOR_YELLOW;
	aCol[4] = COLOR_GREEN;
	aCol[5] = COLOR_GREEN;
	aCol[6] = COLOR_CYAN;
	aCol[7] = COLOR_CYAN;
	aCol[8] = COLOR_BLUE;
	aCol[9] = COLOR_BLUE;

	// 頂点バッファの設定
	for (int nCntVtx = 0; nCntVtx < m_nNumVtx; nCntVtx++)
	{
		// 頂点座標の設定
		pVtx[nCntVtx].pos = m_aPosOffset[0][nCntVtx];

		// 法線の設定
		pVtx[nCntVtx].nor = NORMAL;

		// 頂点カラーの設定
		pVtx[nCntVtx].col = aCol[nCntVtx];
	}

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//=============================
// 描画用頂点バッファ設定
//=============================
void CMeshSkinFlat::SetDrawVtxBuff(void)
{
	VERTEX_3D* pVtx;
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点バッファの設定
	for (int nCntVtx = 0; nCntVtx < m_nNumVtx; nCntVtx++)
	{
		// 頂点座標の設定
		pVtx[nCntVtx].pos = m_aPosVtx[nCntVtx];
	}

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//=============================
// 外部ファイル読み込み処理
//=============================
bool CMeshSkinFlat::LoadScripts(const char* pFilePath)
{
	// 基本情報読み込み
	if (LoadSkinInfo(pFilePath) == false) return false;

	// ボーン情報読み込み
	if (LoadBoneInfo(pFilePath) == false) return false;

	// 頂点情報読み込み
	if (LoadVertexInfo(pFilePath) == false) return false;

	return true;
}

//=============================
// 基本情報読み込み処理
//=============================
bool CMeshSkinFlat::LoadSkinInfo(const char* pFilePath)
{
	FILE* pFile = fopen(pFilePath, "r");

	if (pFile == NULL) return false;

	// 外部ファイル読み取り処理を生成
	CLoadScripts* pLoadScripts = new CLoadScripts();

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]

	do
	{ // 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

		if (strstr(&aStringCopy[0], MESHSKIN_INFO) != NULL)
		{ // MESHSKIN_INFOを読み込んだ
			while (1)
			{ // 文字列のクリア
				memset(&aString[0], 0, sizeof aString);
				memset(&aStringCopy[0], 0, sizeof aStringCopy);

				// 読み込んだ行の文字列判定
				fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
				pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

				if (strstr(&aStringCopy[0], NUM_VERTEX) != NULL)
				{ // NUM_VERTEXを読み込んだ
					m_nNumVtx = pLoadScripts->LoadInt(&aStringCopy[0]);
				}

				if (strstr(&aStringCopy[0], POS) != NULL)
				{ // POSを読み込んだ
					m_pos = pLoadScripts->LoadVector3(&aStringCopy[0]);
				}

				if (strstr(&aStringCopy[0], ROT) != NULL)
				{ // POSを読み込んだ
					m_rot = pLoadScripts->LoadVector3(&aStringCopy[0]);
				}

				if (strstr(&aStringCopy[0], END_MESHSKIN_INFO) != NULL)
				{ // END_MESHSKIN_INFOを読み込んだ
					bLoadEnd = true;
					break;
				}
			}
		}
	} while (bLoadEnd == false);

	// ファイルを閉じる
	fclose(pFile);

	// 外部ファイル読み取り処理を破棄
	SAFE_DELETE_SINGULAR(pLoadScripts);
	return true;
}

//=============================
// ボーン情報読み込み処理
//=============================
bool CMeshSkinFlat::LoadBoneInfo(const char* pFilePath)
{
	FILE* pFile = fopen(pFilePath, "r");

	if (pFile == NULL) return false;

	// 外部ファイル読み取り処理を生成
	CLoadScripts* pLoadScripts = new CLoadScripts();

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]
	int nCntBone = 0;
	do
	{ // 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

		if (strstr(&aStringCopy[0], BONE_INFO) != NULL)
		{ // BONE_INFOを読み込んだ
			while (1)
			{ // 文字列のクリア
				memset(&aString[0], 0, sizeof aString);
				memset(&aStringCopy[0], 0, sizeof aStringCopy);

				// 読み込んだ行の文字列判定
				fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
				pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

				if (strstr(&aStringCopy[0], NUM_BONE) != NULL)
				{ // NUM_BONEを読み込んだ
					m_nNumBone = pLoadScripts->LoadInt(&aStringCopy[0]);
				}

				if (strstr(&aStringCopy[0], BONESET) != NULL)
				{ // BONESETを読み込んだ
					while (1)
					{ // 文字列のクリア
						memset(&aString[0], 0, sizeof aString);
						memset(&aStringCopy[0], 0, sizeof aStringCopy);
						// 読み込んだ行の文字列判定
						fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
						pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

						if (strstr(&aStringCopy[0], POS) != NULL)
						{ // POSを読み込んだ
							m_aBone[nCntBone].pos = pLoadScripts->LoadVector3(&aStringCopy[0]);
						}
						if (strstr(&aStringCopy[0], ROT) != NULL)
						{ // ROTを読み込んだ
							m_aBone[nCntBone].rot = pLoadScripts->LoadVector3(&aStringCopy[0]);
						}
						if (strstr(&aStringCopy[0], END_BONESET) != NULL)
						{ // END_BONESETを読み込んだ
							nCntBone++;
							break;
						}
					}
				}

				if (strstr(&aStringCopy[0], END_BONE_INFO) != NULL)
				{ // END_BONE_INFOを読み込んだ
					bLoadEnd = true;
					break;
				}
			}
		}
	} while (bLoadEnd == false);

	// ファイルを閉じる
	fclose(pFile);

	// 外部ファイル読み取り処理を破棄
	SAFE_DELETE_SINGULAR(pLoadScripts);

	return true;
}

//=============================
// 頂点情報読み込み処理
// ボーン0を基点に設定
//=============================
bool CMeshSkinFlat::LoadVertexInfo(const char* pFilePath)
{
	FILE* pFile = fopen(pFilePath, "r");

	if (pFile == NULL) return false;

	// 外部ファイル読み取り処理を生成
	CLoadScripts* pLoadScripts = new CLoadScripts();

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]
	int nCntVtx = 0;
	do
	{ // 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

		if (strstr(&aStringCopy[0], VERTEX_INFO) != NULL)
		{ // VERTEX_INFOを読み込んだ
			while (1)
			{ // 文字列のクリア
				memset(&aString[0], 0, sizeof aString);
				memset(&aStringCopy[0], 0, sizeof aStringCopy);

				// 読み込んだ行の文字列判定
				fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
				pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

				if (strstr(&aStringCopy[0], NUM_BONE) != NULL)
				{ // NUM_BONEを読み込んだ
					m_nNumBone = pLoadScripts->LoadInt(&aStringCopy[0]);
				}

				if (strstr(&aStringCopy[0], SETVERTEX) != NULL)
				{ // BONESETを読み込んだ
					while (1)
					{ // 文字列のクリア
						memset(&aString[0], 0, sizeof aString);
						memset(&aStringCopy[0], 0, sizeof aStringCopy);
						// 読み込んだ行の文字列判定
						fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
						pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

						if (strstr(&aStringCopy[0], POS) != NULL)
						{ // POSを読み込んだ
							m_aPosOffset[0][nCntVtx] = pLoadScripts->LoadVector3(&aStringCopy[0]);;
						}
						if (strstr(&aStringCopy[0], WEIGHT) != NULL)
						{ // WEIGHTを読み込んだ
							m_aWeight[0][nCntVtx] = pLoadScripts->LoadFloat(&aStringCopy[0]);;
						}
						if (strstr(&aStringCopy[0], END_SETVERTEX) != NULL)
						{ // END_SETVERTEXを読み込んだ
							nCntVtx++;
							break;
						}
					}
				}

				if (strstr(&aStringCopy[0], END_VERTEX_INFO) != NULL)
				{ // END_VERTEX_INFOを読み込んだ
					bLoadEnd = true;
					break;
				}
			}
		}
	} while (bLoadEnd == false);

	// ファイルを閉じる
	fclose(pFile);

	// 外部ファイル読み取り処理を破棄
	SAFE_DELETE_SINGULAR(pLoadScripts);

	// 他のボーンのオフセットを設定
	for (int nCntBone = 1; nCntBone < m_nNumBone; nCntBone++)
	{
		for (int nCntVtx1 = 0; nCntVtx1 < m_nNumVtx; nCntVtx1++)
		{
			m_aPosOffset[nCntBone][nCntVtx1] = m_aPosOffset[0][nCntVtx1] - m_aBone[nCntBone].pos;
			m_aWeight[nCntBone][nCntVtx1] = 1.0f - m_aWeight[nCntBone-1][nCntVtx1];
		}
	}

	for (int nCntVtx1 = 0; nCntVtx1 < m_nNumVtx; nCntVtx1++)
	{
		m_aPosVtx[nCntVtx1] = m_aPosOffset[0][nCntVtx1];
	}

	return true;
}