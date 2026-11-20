//==========================================
// 
// フィールド処理[field.h]
// Author : Kajino Hiyori _Create2026/09/28~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "input.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "scene.h"
#include "game.h"
#include "object3D.h"
#include "polygonField.h"
#include "polygon3D.h"
#include "field.h"
#include "camera.h"
#include "debugproc.h"
#include "loadscripts.h"
#include "effect.h"

//=============================
// マクロ定義
//=============================
#define CHANGE_COLOR	(0.05f)		// 注意喚起の色変更
#define BREAK_TIME		(300)		// 注意喚起が出てから壊れるまでの時間
#define SHAKE_TIME		(1.0f)		// 画面振動幅
#define SHAKE_TIME		(120)		// 振動時間
#define WARNING_Y		(1.0f)		// 注意表示UIの高度
#define HEIGHT_DEFAULT	(10.0f)		// ブロックの高さ
#define FIELD_WIDTH		(100.0f)	// フィールド幅
#define FIELD_SPACE		(FIELD_WIDTH * 4.0f)	// フィールド同士の間隔

//=============================
// 静的メンバ変数宣言
//=============================
int CField::m_nNumAll = 0;
const char* CField::m_apFilePath[NUM_FIELDTEXTURE] =
{
	"data\\TEXTURE\\field\\field.jpg",	// 通常時の地面
	"data\\TEXTURE\\field\\field_warning.jpg",	// 注意表示の地面
};

//=============================
// コンストラクタ
//=============================
CField::CField(CObject::PRIORITY priority) : CObject(priority)
{
	m_pWarning = NULL;		// 注意喚起管理
	m_pos = INIT_D3DVEC3;	// 自身の位置
	m_rot = INIT_D3DVEC3;	// 自身の角度
	m_fWidth = 0.0f;		// 幅
	m_fDepth = 0.0f;		// 奥行
	m_fHeight = 0.0f;		// 高さ
	m_fWidthDef = 0.0f;		// 基準の幅
	m_fDepthDef = 0.0f;		// 基準の奥行
	m_fHeightDef = 0.0f;	// 基準の高さ
	m_state = STATE_NONE;	// 現在の状態
	m_fColorAlpha = 0.0f;	// 色
	m_nStateCounter = 0;	// ステート管理
	m_bDegital = false;		// デジタル判定
	m_nIdx = -1;			// インデックス
	for (int nCntSide = 0; nCntSide < SIDE_MAX; nCntSide++)
	{
		m_apSide[nCntSide] = NULL;	// 側面
	}
	for (int nCntLib = 0; nCntLib < LID_MAX; nCntLib++)
	{
		m_apLid[nCntLib] = NULL;	// 上下面
	}
	m_warning = WARNING_ADD;
	m_nNumAll++;
}

//=============================
// デストラクタ
//=============================
CField::~CField()
{

}

//=============================
// フィールド生成処理
//=============================
CField* CField::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth)
{
	CField* pField = NULL;

	if (pField == NULL)
	{
		pField = new CField;	// インスタンス生成

		if (pField != NULL)
		{ // インスタンス生成に成功
			// フィールドの初期化処理
			pField->Init(pos, rot, fWidth, fDepth);

			return pField;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CField::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth)
{
	// 各情報を設定
	m_pos = pos;
	m_rot = rot;
	m_fWidth = fWidth;
	m_fDepth = fDepth;
	m_fHeight = HEIGHT_DEFAULT;
	m_fWidthDef = m_fWidth;		// 基準の幅
	m_fDepthDef = m_fDepth;		// 基準の奥行
	m_fHeightDef = m_fHeight;	// 基準の高さ

	// 側面のポリゴンを生成
	m_apSide[SIDE_FRONT] = CPolygon3D::Create(D3DXVECTOR3(0.0f, 0.0f, -m_fDepth), INIT_D3DVEC3, COLOR_BLACK, fWidth, m_fHeight);
	m_apSide[SIDE_LEFT] = CPolygon3D::Create(D3DXVECTOR3(-m_fWidth, 0.0f, 0.0f), D3DXVECTOR3(0.0f, HALF_D3DXPI, 0.0f), COLOR_CYAN, m_fDepth, m_fHeight);
	m_apSide[SIDE_BACK] = CPolygon3D::Create(D3DXVECTOR3(0.0f, 0.0f, m_fDepth), D3DXVECTOR3(0.0f, D3DX_PI, 0.0f), COLOR_BLACK, fWidth, m_fHeight);
	m_apSide[SIDE_RIGHT] = CPolygon3D::Create(D3DXVECTOR3(m_fWidth, 0.0f, 0.0f), D3DXVECTOR3(0.0f, -HALF_D3DXPI, 0.0f), COLOR_CYAN, m_fDepth, m_fHeight);

	// 蓋ポリゴンを生成
	m_apLid[LID_TOP] = CPolygonField::Create(D3DXVECTOR3(0.0f, m_fHeight, 0.0f), INIT_D3DVEC3, COLOR_GREEN, m_fWidth, m_fDepth);
	m_apLid[LID_BUTTON] = CPolygonField::Create(D3DXVECTOR3(0.0f, -m_fHeight, 0.0f), D3DXVECTOR3(D3DX_PI, 0.0f, 0.0f), COLOR_GREEN, m_fWidth, m_fDepth);

	// 注意表示ポリゴン生成
	m_pWarning = CPolygonField::Create(D3DXVECTOR3(pos.x, pos.y + WARNING_Y + m_fHeight, pos.z), rot, COLOR_WHITE, fWidth, m_fDepth);
	m_pWarning->SetTextureIdx(CManager::GetTexture()->Register(m_apFilePath[1]));
	m_pWarning->SetColor(COLOR_WHITE_ZERO);

	// オブジェクトにタイプを指定
	CObject::SetType(CObject::TYPE_FIELD);

	// インデックスを指定
	m_nIdx = m_nNumAll - 1;

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CField::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// 注意表示ポリゴンの破棄
	SAFE_UNINIT(m_pWarning);

	for (int nCntSide = 0; nCntSide < SIDE_MAX; nCntSide++)
	{ // 側面の破棄
		SAFE_UNINIT(m_apSide[nCntSide]);
	}

	for (int nCntLib = 0; nCntLib < LID_MAX; nCntLib++)
	{ // 上下面の破棄
		SAFE_UNINIT(m_apLid[nCntLib]);	// 上下面
	}
}

//=============================
// 更新処理
//=============================
void CField::Update(void)
{
	if (CManager::GetInputKeyboard()->GetTrigger(DIK_0) == true && m_state == STATE_NONE)
	{
		SetState(STATE_WARNING);
	}

	switch (m_state)
	{
	case STATE_WARNING:	// 注意喚起
		UpdateWarning();
		break;

	case STATE_BREALING:	// 破壊中
		UpdateBreaking();
		break;

	default:	// 通常||デジタル
		break;
	}

#ifndef ENABLE_STAGESET
	// カメラ位置に応じて削除
	DeleteLine();
#endif // !ENABLE_STAGESET
}

//=============================
// 描画処理
//=============================
void CField::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();
	D3DXMATRIX mtxWorld, mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxGauge;	// ゲージ用マトリックス

	// ワールドマトリックスの初期化(デフォルトの値にする)
	D3DXMatrixIdentity(&mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	for (int nCntSide = 0; nCntSide < SIDE_MAX; nCntSide++)
	{ // 側面
		if (m_apSide[nCntSide] == NULL)continue;
		m_apSide[nCntSide]->Draw(mtxWorld);
	}

	for (int nCntLib = 0; nCntLib < LID_MAX; nCntLib++)
	{ // 上下面
		if (m_apLid[nCntLib] == NULL)continue;
		m_apLid[nCntLib]->Draw(mtxWorld);
	}

	if (m_pWarning == NULL || m_state != STATE_WARNING) return;
	m_pWarning->Draw();
}

//=============================
// 高度取得処理
//=============================
float CField::GetHeight(D3DXVECTOR3 pos)
{
	return m_fHeight + m_pos.y;
}

//=============================
// 行動範囲処理
//=============================
bool CField::OnField(D3DXVECTOR3 pos, float fRadius)
{
	D3DXVECTOR3 posLocal = pos;
	// オブジェクト情報の獲得
	float fAngle = atan2f(m_fWidth, m_fDepth);
	float fLength = sqrtf(m_fWidth * m_fWidth + m_fDepth * m_fDepth);

	// 各頂点の座標計算
	D3DXVECTOR3 posLeftBack = INIT_D3DVEC3;		// モデル左奥の座標
	posLeftBack.x = m_pos.x - sinf(-m_rot.y + fAngle) * (fLength + fRadius);
	posLeftBack.y = 0.0f;
	posLeftBack.z = m_pos.z + cosf(-m_rot.y + fAngle) * (fLength + fRadius);
	D3DXVECTOR3 posRightBack = INIT_D3DVEC3;	// モデル右奥の座標
	posRightBack.x = m_pos.x + sinf(m_rot.y + fAngle) * (fLength + fRadius);
	posRightBack.y = 0.0f;
	posRightBack.z = m_pos.z + cosf(m_rot.y + fAngle) * (fLength + fRadius);
	D3DXVECTOR3 posLeftFront = INIT_D3DVEC3;	// モデル左手前の座標
	posLeftFront.x = m_pos.x - sinf(m_rot.y + fAngle) * (fLength + fRadius);
	posLeftFront.y = 0.0f;
	posLeftFront.z = m_pos.z - cosf(m_rot.y + fAngle) * (fLength + fRadius);
	D3DXVECTOR3 posRightFront = INIT_D3DVEC3;	// モデル右手前の座標
	posRightFront.x = m_pos.x + sinf(-m_rot.y + fAngle) * (fLength + fRadius);
	posRightFront.y = 0.0f;
	posRightFront.z = m_pos.z - cosf(-m_rot.y + fAngle) * (fLength + fRadius);

	D3DXVECTOR3 vecLineLBRB = posRightBack - posLeftBack;	// 左奥→右奥
	D3DXVec3Normalize(&vecLineLBRB, &vecLineLBRB);
	D3DXVECTOR3 vecLineRBRF = posRightFront - posRightBack;	// 右奥→右手前
	D3DXVec3Normalize(&vecLineRBRF, &vecLineRBRF);
	D3DXVECTOR3 vecLineRFLF = posLeftFront - posRightFront;	// 右手前→左手前
	D3DXVec3Normalize(&vecLineRFLF, &vecLineRFLF);
	D3DXVECTOR3 vecLineLFLB = posLeftBack - posLeftFront;	// 左手前→左奥
	D3DXVec3Normalize(&vecLineLFLB, &vecLineLFLB);

	// オブジェクトとの当たり判定[XZ]
	if ((CrossCollisionLeft(posLocal, posLeftBack, vecLineLBRB) == true)		// 左奥基準
		&& (CrossCollisionLeft(posLocal, posRightFront, vecLineRFLF) == true)	// 右手前基準
		&& (CrossCollisionLeft(posLocal, posLeftFront, vecLineLFLB) == true)	// 左手前基準
		&& (CrossCollisionLeft(posLocal, posRightBack, vecLineRBRF) == true))	// 右奥基準
	{ // オブジェクトにめり込んでいる
		return true;
	}
	return false;
}

//=============================
// ステート更新処理
//=============================
void CField::SetState(STATE state)
{
	m_state = state;
	m_nStateCounter = 0;
}

#define STAGESET		("STAGESET")		// STAGESET
#define END_STAGESET	("END_STAGESET")	// END_STAGESET
#define LINE			("LINE")			// LINE
#define DEPTH			("DEPTH")			// DEPTH

//=============================
// ステージ配置処理
//=============================
bool CField::SetStage(const char* pFilePath)
{
	FILE* pFile = fopen(pFilePath, "r");

	if (pFile == NULL)
	{ // 読み込みに失敗
		return false;
	}

	// 外部ファイル読み取り処理を生成
	CLoadScripts* pLoadScripts = new CLoadScripts();

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]
	D3DXVECTOR3 pos = INIT_D3DVEC3;	// 位置
	float fDepth = 0.0f;			// 奥行き

	do
	{ // 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

		if (strstr(&aStringCopy[0], STAGESET) != NULL)
		{ // STAGESETを読み込んだ
			while (1)
			{
				// 文字列のクリア
				memset(&aString[0], 0, sizeof aString);
				memset(&aStringCopy[0], 0, sizeof aStringCopy);

				// 読み込んだ行の文字列判定
				fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
				pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

				if (strstr(&aStringCopy[0], POS) != NULL)
				{ // POSを読み込んだ
					pos = pLoadScripts->LoadVector3(&aStringCopy[0]);
				}
				if (strstr(&aStringCopy[0], LINE) != NULL)
				{ // LINEを読み込んだ
					int nLine = pLoadScripts->LoadInt(&aStringCopy[0]);
					switch (nLine)
					{
					case 0:	// 左
						pos.x = -FIELD_SPACE;
						break;

					case 1:	// 中央
						pos.x = 0.0f;
						break;

					case 2:	// 右
						pos.x = FIELD_SPACE;
						break;

					default:	// 例外は中央に集約
						pos.x = 0.0f;
						break;
					}
				}
				if (strstr(&aStringCopy[0], DEPTH) != NULL)
				{ // DEPTHを読み込んだ
					fDepth = pLoadScripts->LoadFloat(&aStringCopy[0]);
					
					// フィールドを生成
					CField::Create(pos, INIT_D3DVEC3, FIELD_WIDTH, fDepth);
				}
				if (strstr(&aStringCopy[0], END_STAGESET) != NULL)
				{ // END_STAGESETを読み込んだ
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
// 注意喚起更新処理
//=============================
void CField::UpdateWarning(void)
{
	// カウンターを加算して状態を管理
	m_nStateCounter++;
	if (m_nStateCounter >= BREAK_TIME)
	{ // 破壊状態に遷移
		// 位置を更新
		m_state = STATE_BREALING;
		CManager::GetRenderer()->SetShake(SHAKE_TIME, SHAKE_TIME);
	}

	// 注意喚起UIの更新処理
	if (m_pWarning == NULL) return;
	D3DXCOLOR col = m_pWarning->GetColor();
	switch (m_warning)
	{
	case WARNING_ADD:	// 不透明化
		col.a += CHANGE_COLOR;
		if (col.a >= 1.0f)
		{
			col.a = 1.0f;
			m_warning = WARNING_DIS;
		}
		break;

	case WARNING_DIS:	// 透明化
		col.a -= CHANGE_COLOR;
		if (col.a <= 0.0f)
		{
			col.a = 0.0f;
			m_warning = WARNING_ADD;
		}
		break;
	}
	m_pWarning->SetColor(col);
}

//=============================
// 破壊更新処理
//=============================
void CField::UpdateBreaking(void)
{
	if (m_bDegital == false)
	{ // 収縮処理
		UpdateDisScaling();
	}
	else
	{ // 拡大処理
		UpdateAddScaling();
	}
}

#define CHANGE_SCALE	(0.5f)	// 収縮速度
//=============================
// 収縮処理
//=============================
void CField::UpdateDisScaling(void)
{
	// 横幅を基準に拡大率を変更
	m_fWidth -= CHANGE_SCALE;
	m_fDepth -= CHANGE_SCALE;
	m_fHeight -= CHANGE_SCALE;
	
	// サイズ更新処理
	SetResize();

	if (m_fWidth <= 0.0f || m_fDepth <= 0.0f || m_fHeight <= 0.0f)
	{ // デジタル化
		m_bDegital = true;
		m_fWidth = 0.0f;
		m_fDepth = 0.0f;
		m_fHeight = 0.0f;
		m_nIdx = -1;
	}
}

//=============================
// 拡大処理
//=============================
void CField::UpdateAddScaling(void)
{
	// 横幅を基準に拡大率を変更
	m_fWidth += CHANGE_SCALE;
	m_fDepth += CHANGE_SCALE;
	m_fHeight += CHANGE_SCALE;

	// サイズ更新処理
	SetResize();

	if (m_fWidth >= m_fWidthDef)
	{
		m_fWidth = m_fWidthDef;
	}
	if (m_fDepth >= m_fDepthDef)
	{
		m_fDepth = m_fDepthDef;
	}
	if (m_fHeight >= m_fHeightDef)
	{
		m_fHeight = m_fHeightDef;
	}

	if (m_fWidth == m_fWidthDef && m_fDepth == m_fDepthDef && m_fHeight == m_fHeightDef)
	{ // デジタル化
		m_state = STATE_DEGITAL;
	}
}

//=============================
// サイズ更新処理
//=============================
void CField::SetResize(void)
{
	// 側面のポリゴンを生成
	m_apSide[SIDE_FRONT]->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -m_fDepth));
	m_apSide[SIDE_FRONT]->SetWidth(m_fWidth);
	m_apSide[SIDE_FRONT]->SetHeight(m_fHeight);

	m_apSide[SIDE_LEFT]->SetPosition(D3DXVECTOR3(-m_fWidth, 0.0f, 0.0f));
	m_apSide[SIDE_LEFT]->SetWidth(m_fDepth);
	m_apSide[SIDE_LEFT]->SetHeight(m_fHeight);

	m_apSide[SIDE_BACK]->SetPosition(D3DXVECTOR3(0.0f, 0.0f, m_fDepth));
	m_apSide[SIDE_BACK]->SetWidth(m_fWidth);
	m_apSide[SIDE_BACK]->SetHeight(m_fHeight);

	m_apSide[SIDE_RIGHT]->SetPosition(D3DXVECTOR3(m_fWidth, 0.0f, 0.0f));
	m_apSide[SIDE_RIGHT]->SetWidth(m_fDepth);
	m_apSide[SIDE_RIGHT]->SetHeight(m_fHeight);

	// 蓋ポリゴンを生成
	m_apLid[LID_TOP]->SetPosition(D3DXVECTOR3(0.0f, m_fHeight, 0.0f));
	m_apLid[LID_TOP]->SetWidth(m_fWidth);
	m_apLid[LID_TOP]->SetDepth(m_fDepth);
	m_apLid[LID_BUTTON]->SetPosition(D3DXVECTOR3(0.0f, -m_fHeight, 0.0f));
	m_apLid[LID_BUTTON]->SetWidth(m_fWidth);
	m_apLid[LID_BUTTON]->SetDepth(m_fDepth);
}

//=============================
// カメラ位置に応じて削除
//=============================
void CField::DeleteLine(void)
{
	if (CManager::GetCamera()->GetPosV().z < m_pos.z + m_fDepth) return;
	// カメラより手前に行ったら削除
	Release();
}