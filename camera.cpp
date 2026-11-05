//========================================================
// 
// カメラ処理[camera.cpp]
// Author : Kajino Hiyori _Create2026/05/31
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "camera.h"
#include "input.h"
#include "manager.h"
#include "debugproc.h"
#include "renderer.h"
#include "game.h"
#include "result.h"
#include "player.h"
#include "field.h"

//=============================
// マクロ定義
//=============================
#define ENEBLE_FOLLOW	// 追従
//#undef	ENEBLE_FOLLOW	// ENEBLE_FOLLOWを無効化
#define MAX_R			(350.0f)	// 注視点Yの制限
#define MAX_V			(1200.0f)	// 視点Yの制限
#define MOVE			(5.5f)		// 移動量
#define ROTATE			(0.005f)	// 回転量
#define DISTANCE		(300.0f)	// 視点と注視点の距離
#define FORWARD			(150.0f)		// プレイヤーの少し前に注視点置く
#define CORRECTION		(0.05f)		// カメラの位置補正
#define CORRECTION_ROT	(0.035f)	// カメラの回り込み補正
#define SPEED			(1.0f)		// 停止カメラになる速度の基準
#define CAMERA_ROLL		(120)		// カメラの回り込み間隔
#define CAMERA_ROLL_MIN	(0)			// カメラの回り込み間隔(最小)
#define CAMERA_ROLL_MAX	(360)		// カメラの回り込み間隔(最大)
#define	GAME_HEIGHT		(300.0f)	// カメラ高度
#define	RESULT_HEIGHT	(100.0f)	// カメラ高度
#define CAMERA_POS		(D3DXVECTOR3(0.0f, GAME_HEIGHT, -DISTANCE))	// カメラの位置
#define DEFAULT			(D3DXVECTOR3(0.0f, 0.0f, 0.0f))			// デフォルトの座標値
#define VEC_U			(D3DXVECTOR3(0.0f, 1.0f, 0.0f))			// カメラの向き
#define DEFAULT_POSR	(D3DXVECTOR3(0.0f, 0.0f, -143.0f))		// デフォルトのPOSR

//=============================
// コンストラクタ
//=============================
CCamera::CCamera()
{
	m_posV = INIT_D3DVEC3;			// 視点
	m_posR = INIT_D3DVEC3;			// 注視点
	m_vecU = INIT_D3DVEC3;			// 上方向ベクトル
	m_rot = INIT_D3DVEC3;			// 向き
	m_posVDest = INIT_D3DVEC3;		// 目的の視点
	m_posRDest = INIT_D3DVEC3;		// 目的の注視点
	m_rotDest = INIT_D3DVEC3;		// 目的の向き
	m_nCntCameraRoll = 0;			// カメラの回り込み間隔を設定
	m_bCameraType = true;			// カメラの操作方法を管理[trueで追従カメラ、falseで操作カメラ]
}

//=============================
// デストラクタ
//=============================
CCamera::~CCamera()
{

}

//=============================
// 初期化処理
//=============================
HRESULT CCamera::Init(void)
{
	m_posV = CAMERA_POS;
	m_posR = DEFAULT;
	m_posVDest = DEFAULT;
	m_posRDest = DEFAULT;
	m_vecU = VEC_U;
	m_rot = DEFAULT;
	m_rotDest = DEFAULT;
	m_nCntCameraRoll = CAMERA_ROLL;
	m_bCameraType = true;

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CCamera::Uninit(void)
{

}

//=============================
// 更新処理
//=============================
void CCamera::Update(void)
{
	// 入力情報の取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// シーン情報を取得
	CScene::MODE mode = CManager::GetMode();
	// シーンごとに処理を分割
	switch (mode)
	{
	//case CScene::MODE_TITLE:	// タイトル
	//	break;
	//
	//case CScene::MODE_TUTORIAL:	// チュートリアル
	//	UpdateTutorial();
	//	break;

	case CScene::MODE_GAME:		// ゲーム
		// ゲーム処理
		UpdateGame();
		break;

	//case CScene::MODE_RESULT:	// リザルト
	//	// リザルト処理
	//	UpdateResult();
	//	break;
	//
	//case CScene::MODE_CREDIT:	// クレジット
	//	break;
	}

	if (m_bCameraType == false)
	{ // カメラの管理(キーボード)
		UpdateOperate();
	}

	// デフォルトのカメラか操作カメラかを指定
//#ifdef _DEBUG
	if (pKeyboard->GetTrigger(DIK_V) == true)
	{
		m_bCameraType = m_bCameraType ^ 1;
	}
//#endif // _DEBUG

	// デバッグ表示
	CDebugProc* pDebugProc = CManager::GetDebugProc();
	pDebugProc->Print("posV : (%.2f, %.2f, %.2f)\n", m_posV.x, m_posV.y, m_posV.z);
	pDebugProc->Print("posR : (%.2f, %.2f, %.2f)\n", m_posR.x, m_posR.y, m_posR.z);
	pDebugProc->Print("rot  : (%.2f, %.2f, %.2f)\n", m_rot.x, m_rot.y, m_rot.z);
}

//=============================
// カメラの設定
//=============================
void CCamera::SetCamera(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

#ifdef CAMERATYPE_ORTHO	// 投影方法
	// プロジェクションマトリックスの作成[左手座標系の平行投影]
	D3DXMatrixOrthoLH(&m_mtxProjection,
		(float)SCREEN_WIDTH,	// ビューボリュームの幅
		(float)SCREEN_HEIGHT,	// ビューボリュームの高さ
		CAMERA_CLOSEST,			// ビューボリュームの近平面までの距離
		CAMERA_FARTHEST);		// ビューボリュームの遠平面までの距離
#else
	// プロジェクションマトリックスの作成[左手座標系の透視投影]
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
		D3DXToRadian(FOV_DEFAULT),					// 視野角
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,	// アスペクト比
		CAMERA_CLOSEST,								// カメラから描画できる距離(カメラに近い場合)
		CAMERA_FARTHEST);							// カメラから描画できる距離(カメラから離れている場合)
#endif

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	// ビューマトリックスを作成
	D3DXMatrixLookAtLH(&m_mtxView,
						&m_posV,		// 視点
						&m_posR,		// 注視点
						&m_vecU);	// 上方向ベクトル

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);
}

//=============================
// ゲーム処理
//=============================
void CCamera::UpdateGame(void)
{
	if (m_bCameraType == false)
	{
		return;
	}

	CPlayer* pPlayer = static_cast<CGame*>(CManager::GetScene())->GetPlayer();
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();
	
	m_posRDest.x = posPlayer.x;
	m_posRDest.y = posPlayer.y;
	m_posRDest.z = posPlayer.z + FORWARD;

	CDebugProc* pDebug = CManager::GetDebugProc();
	pDebug->Print("rotDest.y %f\n", m_rotDest.y);

	// 視点情報更新
	m_posR.x += (m_posRDest.x - m_posR.x) * CORRECTION_ROT;
	m_posR.y += (m_posRDest.y - m_posR.y) * CORRECTION_ROT;
	m_posR.z += (m_posRDest.z - m_posR.z) * CORRECTION_ROT;
	m_posV.x = m_posR.x - sinf(m_rot.y) * (DISTANCE);
	m_posV.y = GAME_HEIGHT;
	m_posV.z = m_posR.z - cosf(m_rot.y) * (DISTANCE);
}

//=============================
// リザルト処理
//=============================
void CCamera::UpdateResult(void)
{
	if (m_bCameraType == false)
	{
		return;
	}
}

//=============================
// シーンごとのカメラ初期位置
//=============================
void CCamera::SetSceneCamera(CScene::MODE mode, CScene* pScene)
{
	/*D3DXVECTOR3 posPlayer = INIT_D3DVEC3;
	CPlayer* pPlayer = NULL;
	float fSizePlayer = 0.0f;
	switch (mode)
	{
	case CScene::MODE_GAME:
		pPlayer = static_cast<CGame*>(pScene)->GetPlayer();
		posPlayer = pPlayer->GetPosition();
		m_posR.x = posPlayer.x;
		m_posR.y = posPlayer.y + fSizePlayer;
		m_posR.z = posPlayer.z;
		m_posV.y = GAME_HEIGHT;
		m_posV.x = m_posR.x - sinf(m_rot.y) * (DISTANCE + (10.0f * 5.0f));
		m_posV.z = m_posR.z - cosf(m_rot.y) * (DISTANCE + (10.0f * 5.0f)); 
		break;

	case CScene::MODE_TUTORIAL:
		pPlayer = static_cast<CTutorial*>(pScene)->GetPlayer();
		posPlayer = pPlayer->GetPosition();
		m_posR.x = posPlayer.x;
		m_posR.y = posPlayer.y + fSizePlayer;
		m_posR.z = posPlayer.z;
		m_posV.y = GAME_HEIGHT;
		m_posV.x = m_posR.x - sinf(m_rot.y) * (DISTANCE + (10.0f * 5.0f));
		m_posV.z = m_posR.z - cosf(m_rot.y) * (DISTANCE + (10.0f * 5.0f));
		break;

	default:
		m_posV = D3DXVECTOR3(0.0f, 240.0f, -740.0f);
		m_posR = D3DXVECTOR3(0.0f, 200.0f, -535.0f);
		break;
	}*/
}

//=============================
// 操作カメラの更新処理
//=============================
void CCamera::UpdateOperate(void)
{
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// 平行移動
	if (pKeyboard->GetPress(DIK_LEFTARROW) == true)	// 右に移動
	{
		if (pKeyboard->GetPress(DIK_UPARROW) == true)	// 奥に移動
		{
			m_posV.x -= cosf(m_rot.y + D3DX_PI / 4) * MOVE;
			m_posV.z += sinf(m_rot.y + D3DX_PI / 4) * MOVE;

			m_posR.x -= cosf(m_rot.y + D3DX_PI / 4) * MOVE;
			m_posR.z += sinf(m_rot.y + D3DX_PI / 4) * MOVE;
		}
		else if (pKeyboard->GetPress(DIK_DOWNARROW) == true)	// 手前に移動
		{
			m_posV.x -= cosf(m_rot.y - D3DX_PI / 4) * MOVE;
			m_posV.z += sinf(m_rot.y - D3DX_PI / 4) * MOVE;

			m_posR.x -= cosf(m_rot.y - D3DX_PI / 4) * MOVE;
			m_posR.z += sinf(m_rot.y - D3DX_PI / 4) * MOVE;
		}
		else
		{
			m_posV.x -= cosf(m_rot.y) * MOVE;
			m_posV.z += sinf(m_rot.y) * MOVE;

			m_posR.x -= cosf(m_rot.y) * MOVE;
			m_posR.z += sinf(m_rot.y) * MOVE;
		}
	}
	else if (pKeyboard->GetPress(DIK_RIGHTARROW) == true)	// 左に移動
	{
		if (pKeyboard->GetPress(DIK_UPARROW) == true)	// 奥に移動
		{
			m_posV.x += cosf(m_rot.y - D3DX_PI / 4) * MOVE;
			m_posV.z -= sinf(m_rot.y - D3DX_PI / 4) * MOVE;

			m_posR.x += cosf(m_rot.y - D3DX_PI / 4) * MOVE;
			m_posR.z -= sinf(m_rot.y - D3DX_PI / 4) * MOVE;
		}
		else if (pKeyboard->GetPress(DIK_DOWNARROW) == true)	// 手前に移動
		{
			m_posV.x += cosf(m_rot.y + D3DX_PI / 4) * MOVE;
			m_posV.z -= sinf(m_rot.y + D3DX_PI / 4) * MOVE;

			m_posR.x += cosf(m_rot.y + D3DX_PI / 4) * MOVE;
			m_posR.z -= sinf(m_rot.y + D3DX_PI / 4) * MOVE;
		}
		else
		{
			m_posV.x += cosf(m_rot.y) * MOVE;
			m_posV.z -= sinf(m_rot.y) * MOVE;

			m_posR.x += cosf(m_rot.y) * MOVE;
			m_posR.z -= sinf(m_rot.y) * MOVE;
		}
	}
	else if (pKeyboard->GetPress(DIK_UPARROW) == true)	// 奥に移動
	{
		m_posV.x += sinf(m_rot.y) * MOVE;
		m_posV.z += cosf(m_rot.y) * MOVE;

		m_posR.x += sinf(m_rot.y) * MOVE;
		m_posR.z += cosf(m_rot.y) * MOVE;
	}
	else if (pKeyboard->GetPress(DIK_DOWNARROW) == true)	// 手前に移動
	{
		m_posV.x -= sinf(m_rot.y) * MOVE;
		m_posV.z -= cosf(m_rot.y) * MOVE;

		m_posR.x -= sinf(m_rot.y) * MOVE;
		m_posR.z -= cosf(m_rot.y) * MOVE;
	}

	// 注視点を中心としたカメラ移動
	if (pKeyboard->GetPress(DIK_Z) == true || pJoypad->GetRightStick(CInputJoypad::JOYKEY_RSTICK_LEFT) == true)	// 時計回り
	{
		m_rot.y += ROTATE;
		if (m_rot.y > D3DX_PI)
		{
			m_rot.y -= D3DX_PI * 2;
		}
		if (m_rot.y < -D3DX_PI)
		{
			m_rot.y += D3DX_PI * 2;
		}
		m_posV.x = m_posR.x - sinf(m_rot.y) * DISTANCE;
		m_posV.z = m_posR.z - cosf(m_rot.y) * DISTANCE;
	}
	else if (pKeyboard->GetPress(DIK_C) == true || pJoypad->GetRightStick(CInputJoypad::JOYKEY_RSTICK_RIGHT) == true)	// 反時計回り
	{
		m_rot.y -= ROTATE;
		if (m_rot.y > D3DX_PI)
		{
			m_rot.y -= D3DX_PI * 2;
		}
		if (m_rot.y < -D3DX_PI)
		{
			m_rot.y += D3DX_PI * 2;
		}
		m_posV.x = m_posR.x - sinf(m_rot.y) * DISTANCE;
		m_posV.z = m_posR.z - cosf(m_rot.y) * DISTANCE;
	}
	if (pKeyboard->GetPress(DIK_Y) == true )	// 上昇
	{
		m_posV.y += ROTATE * DISTANCE;
		if (m_posV.y >= MAX_V)
		{
			m_posV.y = MAX_V;
		}
	}
	else if (pKeyboard->GetPress(DIK_N) == true)	// 下降
	{
		m_posV.y -= ROTATE * DISTANCE;
		m_posV.y -= ROTATE * DISTANCE;
		if (m_posV.y <= 0)
		{
			m_posV.y = 0;
		}
	}

	// 視点の起点としたカメラ移動(首振り)
	if (pKeyboard->GetPress(DIK_E) == true)
	{
		m_rot.y += ROTATE;
		if (m_rot.y > D3DX_PI)
		{
			m_rot.y -= D3DX_PI * 2;
		}
		if (m_rot.y < -D3DX_PI)
		{
			m_rot.y += D3DX_PI * 2;
		}
		m_posR.x = m_posV.x + sinf(m_rot.y) * DISTANCE;
		m_posR.z = m_posV.z + cosf(m_rot.y) * DISTANCE;
		}
	else if (pKeyboard->GetPress(DIK_Q) == true)
	{
		m_rot.y -= ROTATE;
		if (m_rot.y > D3DX_PI)
		{
			m_rot.y -= D3DX_PI * 2;
		}
		if (m_rot.y < -D3DX_PI)
		{
			m_rot.y += D3DX_PI * 2;
		}
		m_posR.x = m_posV.x + sinf(m_rot.y) * DISTANCE;
		m_posR.z = m_posV.z + cosf(m_rot.y) * DISTANCE;
	}
	if (pKeyboard->GetPress(DIK_T) == true || pJoypad->GetRightStick(CInputJoypad::JOYKEY_RSTICK_UP) == true)	// アオリ
	{
		m_posR.y += ROTATE * DISTANCE;
		if (m_posR.y >= MAX_R)
		{
			m_posR.y = MAX_R;
		}
	}
	else if (pKeyboard->GetPress(DIK_B) == true || pJoypad->GetRightStick(CInputJoypad::JOYKEY_RSTICK_DOWN) == true)	// フカン
	{
		m_posR.y -= ROTATE * DISTANCE;
		if (m_posR.y <= -MAX_R)
		{
			m_posR.y = -MAX_R;
		}
	}
}