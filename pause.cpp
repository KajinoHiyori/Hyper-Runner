//==========================================
// 
// ポーズ処理[pause.cpp]
// Author : Kajino Hiyori _Create2026/07/16~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "pause.h"
#include "manager.h"
#include "polygon2D.h"
#include "texture.h"
#include "input.h"
#include "fade.h"
#include "scene.h"
#include "game.h"
#include "ranking.h"
#include "sound.h"

//=============================
// マクロ定義
//=============================
#define MENU_SIZE	(0.6f)		// 文字の大きさを変更
#define PAUSE_SIZE	(1.2f)		// 文字の大きさを変更[PAUSEのみ拡大]
#define MENU_HEIGHT	(50.0f * MENU_SIZE)		// 文字の高さ
#define MENU_WIDTH	(400.0f * MENU_SIZE)	// 文字の幅
#define GEAAR_SIZE	(2.0f)		// 歯車の拡大率
#define GEAR1_SIZE	(208.0f * GEAAR_SIZE)	// 歯車1のサイズ
#define GEAR2_SIZE	(120.0f * GEAAR_SIZE)	// 歯車2のサイズ
#define GEAR3_SIZE	(160.0f * GEAAR_SIZE)	// 歯車3のサイズ
#define GEAR4_SIZE	(216.0f * GEAAR_SIZE)	// 歯車4のサイズ
#define PAUSE_POS		(D3DXVECTOR3(SCREEN_CENTER.x, 180.0f, 0.0f))	// PAUSEの位置
#define CONTINUE_POS	(SCREEN_CENTER)							// CONTINUEの位置
#define RETRY_POS		(D3DXVECTOR3(250.0f, 540.0f, 0.0f))		// RETRYの位置
#define QUIT_POS		(D3DXVECTOR3(1030.0f, 540.0f, 0.0f))	// QUITの位置
#define GUIR1_POS		(D3DXVECTOR3(1200.0f, 40.0f, 0.0f))	// gear1の位置
#define GUIR2_POS		(D3DXVECTOR3(1100.0f, 400.0f, 0.0f))	// gear2の位置
#define GUIR3_POS		(D3DXVECTOR3(30.0f, 250.0f, 0.0f))	// gear3の位置
#define GUIR4_POS		(D3DXVECTOR3(120.0f, 650.0f, 0.0f))	// gear4の位置
#define ROTATE_GEAR		(0.02f)	// ギアの回転速度
#define VIBRATION		(3000)	// 振動値
#define VIBRATION_TIME	(1)	// 振動時間
#define DECIDE_VIBTIME	(10)	// 選択時の振動時間
#define STICK_ROT		(D3DX_PI / 3.0f)	// スティックの選択値の基準

//=============================
// 静的メンバ変数宣言
//=============================
const char* CPause::m_apFilename[CPause::TYPE_MAX] =
{
	"data\\TEXTURE\\pause\\pause_000.png",	// 背景
	"data\\TEXTURE\\pause\\gear1.png",	// 歯車1
	"data\\TEXTURE\\pause\\gear2.png",	// 歯車2
	"data\\TEXTURE\\pause\\gear3.png",	// 歯車3
	"data\\TEXTURE\\pause\\gear4.png",	// 歯車4
	"data\\TEXTURE\\pause\\pause_001.png",	// pause
	"data\\TEXTURE\\pause\\pause_002.png",	// continue
	"data\\TEXTURE\\pause\\pause_003.png",	// retry
	"data\\TEXTURE\\pause\\pause_004.png"	// quit
};

//=============================
// コンストラクタ
//=============================
CPause::CPause(CObject::PRIORITY priority) : CObject(priority)
{
	// 値のクリア
	m_mousePos = INIT_D3DVEC3;
	m_mousePosOld = INIT_D3DVEC3;
	m_menu = MENU_CONTINUE;	// 現在選択されているメニュー
	m_bPause = false;	// ポーズ状態[true : ポーズ中 / false : ポーズ中でない]
	memset(&m_apPolygon[0], NULL, sizeof m_apPolygon);	// ポリゴン情報[object継承無]
	m_bDisp = true;	// 表示状態
}

//=============================
// デストラクタ
//=============================
CPause::~CPause()
{

}

//=============================
// ポーズ生成処理
//=============================
CPause* CPause::Create(void)
{
	CPause* pPause = NULL;
	if (pPause == NULL)
	{ // クリアに成功
		pPause = new CPause;
		if (pPause != NULL)
		{ // インスタンス生成に成功
			pPause->Init();
			pPause->SetType(CObject::TYPE_PAUSE);
			return pPause;
		}
	}
	// 生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CPause::Init(void)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// ポリゴンの用意
	m_apPolygon[CPause::TYPE_BG] = CPolygon2D::Create(SCREEN_CENTER, (float)SCREEN_WIDTH * 0.5f, (float)SCREEN_HEIGHT * 0.5f, COLOR_WHITE);
	m_apPolygon[CPause::TYPE_PAUSE] = CPolygon2D::Create(PAUSE_POS, MENU_WIDTH * PAUSE_SIZE, MENU_HEIGHT * PAUSE_SIZE, COLOR_CYAN);
	m_apPolygon[CPause::TYPE_GEAR1] = CPolygon2D::Create(GUIR1_POS, GEAR1_SIZE, GEAR1_SIZE, COLOR_WHITE, CPolygon2D::ROTATE);
	m_apPolygon[CPause::TYPE_GEAR2] = CPolygon2D::Create(GUIR2_POS, GEAR2_SIZE, GEAR2_SIZE, COLOR_WHITE, CPolygon2D::ROTATE);
	m_apPolygon[CPause::TYPE_GEAR3] = CPolygon2D::Create(GUIR3_POS, GEAR3_SIZE, GEAR3_SIZE, COLOR_WHITE, CPolygon2D::ROTATE);
	m_apPolygon[CPause::TYPE_GEAR4] = CPolygon2D::Create(GUIR4_POS, GEAR4_SIZE, GEAR4_SIZE, COLOR_WHITE, CPolygon2D::ROTATE);
	m_apPolygon[CPause::TYPE_CONTINUE] = CPolygon2D::Create(CONTINUE_POS, MENU_WIDTH, MENU_HEIGHT, COLOR_WHITE);
	m_apPolygon[CPause::TYPE_RETRY] = CPolygon2D::Create(RETRY_POS, MENU_WIDTH, MENU_HEIGHT, COLOR_WHITE);
	m_apPolygon[CPause::TYPE_QUIT] = CPolygon2D::Create(QUIT_POS, MENU_WIDTH, MENU_HEIGHT, COLOR_WHITE);

	// テクスチャ指定
	for (int nCntPause = 0; nCntPause < CPause::TYPE_MAX; nCntPause++)
	{
		int nIdx = pTexture->Register(m_apFilename[nCntPause]);
		m_apPolygon[nCntPause]->SetTextureIdx(nIdx);
	}

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CPause::Uninit(void)
{
	for (int nCntPause = 0; nCntPause < CPause::TYPE_MAX; nCntPause++)
	{
		if (m_apPolygon[nCntPause] != NULL)
		{ // 使用されていた場合
			m_apPolygon[nCntPause]->Uninit();
			m_apPolygon[nCntPause] = NULL;
		}
	}
}

//=============================
// 更新処理
//=============================
void CPause::Update(void)
{
	// 入力情報
	CInputJoypad* pJoypad = CManager::GetInputJoypad();
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// フェード情報
	CFade* pFade = CManager::GetFade();

	// ポーズ中でなければ初期化
	if (m_bPause == false)
	{
		if (pJoypad->GetTrigger(CInputJoypad::JOYKEY_START) == true
			|| pKeyboard->GetTrigger(DIK_P) == true)
		{
			m_bPause = true;
			CManager::GetSound()->Play(CSound::SE_ENTER);

			int nLeft = pJoypad->GetVibrationLeft();
			int nRight = pJoypad->GetVibrationRight();
			nLeft += VIBRATION;
			nRight += VIBRATION;
			pJoypad->SetVibration(nLeft, nRight, VIBRATION_TIME);

		}
		m_bDisp = true;
	}
	else
	{
		if (pJoypad->GetTrigger(CInputJoypad::JOYKEY_START) == true
			|| pKeyboard->GetTrigger(DIK_P) == true)
		{ // ポーズ解除
			m_bPause = false;
			m_menu = MENU_CONTINUE;// continueに合わせる
			CManager::GetSound()->Play(CSound::SE_ENTER);
			int nLeft = pJoypad->GetVibrationLeft();
			int nRight = pJoypad->GetVibrationRight();
			nLeft += VIBRATION;
			nRight += VIBRATION;
			pJoypad->SetVibration(nLeft, nRight, VIBRATION_TIME);
		}
		if (pKeyboard->GetTrigger(DIK_F2) == true)
		{ // 表示切替
			m_bDisp = m_bDisp ^ 1;
		}

		// 決定処理
		DecisionMenu();
		
		// 通常時の選択処理
		NormalSelect();

		// マウス選択処理
		MouseSelect();

		// スティック選択処理
		StickSelect();

		// ポリゴンの色を変更
		switch (m_menu)
		{
		case MENU_CONTINUE:	// continue
			m_apPolygon[CPause::TYPE_CONTINUE]->SetColor(COLOR_ORANGE);
			m_apPolygon[CPause::TYPE_RETRY]->SetColor(COLOR_WHITE_ALPHA);
			m_apPolygon[CPause::TYPE_QUIT]->SetColor(COLOR_WHITE_ALPHA);
			break;

		case MENU_RETRY:	// retry
			m_apPolygon[CPause::TYPE_CONTINUE]->SetColor(COLOR_WHITE_ALPHA);
			m_apPolygon[CPause::TYPE_RETRY]->SetColor(COLOR_ORANGE);
			m_apPolygon[CPause::TYPE_QUIT]->SetColor(COLOR_WHITE_ALPHA);
			break;

		case MENU_QUIT:	// quit
			m_apPolygon[CPause::TYPE_CONTINUE]->SetColor(COLOR_WHITE_ALPHA);
			m_apPolygon[CPause::TYPE_RETRY]->SetColor(COLOR_WHITE_ALPHA);
			m_apPolygon[CPause::TYPE_QUIT]->SetColor(COLOR_ORANGE);
			break;
		}
	}

	// 歯車の回転演出
	for (int nCntGear = CPause::TYPE_GEAR1; nCntGear <= CPause::TYPE_GEAR4; nCntGear++)
	{
		D3DXVECTOR3 rot = m_apPolygon[nCntGear]->GetRotation();
		rot.z = AngleNormalize(rot.z + ROTATE_GEAR);
		m_apPolygon[nCntGear]->SetRotation(rot);
	}
}

//=============================
// 描画処理
//=============================
void CPause::Draw(void)
{
	if (m_bPause == true && m_bDisp == true)
	{ // ポーズ中のみ描画
		for (int nCntPause = 0; nCntPause < CPause::TYPE_MAX; nCntPause++)
		{
			if (m_apPolygon[nCntPause] != NULL)
			{ // ポリゴンの描画
				m_apPolygon[nCntPause]->Draw();
			}
		}
	}
}

//=============================
// マウス選択処理
//=============================
void CPause::MouseSelect(void)
{
	// 入力情報取得
	CInputMouse* pMouse = CManager::GetInputMouse();
	m_mousePosOld = m_mousePos;
	m_mousePos = pMouse->GetClientPos();

	if (m_mousePos == m_mousePosOld)
	{ // マウス操作が行われていない場合早期リターン
		return;
	}
	if (m_mousePos.x >= m_apPolygon[TYPE_CONTINUE]->GetPosition().x - MENU_WIDTH
		&& m_mousePos.x <= m_apPolygon[TYPE_CONTINUE]->GetPosition().x + MENU_WIDTH
		&& m_mousePos.y >= m_apPolygon[TYPE_CONTINUE]->GetPosition().y - MENU_HEIGHT
		&& m_mousePos.y <= m_apPolygon[TYPE_CONTINUE]->GetPosition().y + MENU_HEIGHT)
	{ // TYPE_CONTINUEを選択
		if (m_menu != MENU_CONTINUE)
		{
			m_menu = MENU_CONTINUE;
			CManager::GetSound()->Play(CSound::SE_SELECT);
		}
	}
	else if (m_mousePos.x >= m_apPolygon[TYPE_RETRY]->GetPosition().x - MENU_WIDTH
		&& m_mousePos.x <= m_apPolygon[TYPE_RETRY]->GetPosition().x + MENU_WIDTH
		&& m_mousePos.y >= m_apPolygon[TYPE_RETRY]->GetPosition().y - MENU_HEIGHT
		&& m_mousePos.y <= m_apPolygon[TYPE_RETRY]->GetPosition().y + MENU_HEIGHT)
	{ // TYPE_RETRYを選択
		if (m_menu != MENU_RETRY)
		{
			m_menu = MENU_RETRY;
			CManager::GetSound()->Play(CSound::SE_SELECT);
		}
	}
	else if (m_mousePos.x >= m_apPolygon[TYPE_QUIT]->GetPosition().x - MENU_WIDTH
		&& m_mousePos.x <= m_apPolygon[TYPE_QUIT]->GetPosition().x + MENU_WIDTH
		&& m_mousePos.y >= m_apPolygon[TYPE_QUIT]->GetPosition().y - MENU_HEIGHT
		&& m_mousePos.y <= m_apPolygon[TYPE_QUIT]->GetPosition().y + MENU_HEIGHT)
	{ // TYPE_QUITを選択
		if (m_menu != MENU_QUIT)
		{
			m_menu = MENU_QUIT;
			CManager::GetSound()->Play(CSound::SE_SELECT);
		}
	}
}

//=============================
// スティック選択処理
//=============================
void CPause::StickSelect(void)
{
	// 入力情報
	CInputJoypad* pJoypad = CManager::GetInputJoypad();
	float fValueX, fValueY = 0.0f;
	int nSelect = 0;
	float fRot = 0.0f;

	if (pJoypad->GetLeftStickValue(&fValueX, &fValueY) == true)
	{
	}
	else if (pJoypad->GetRightStickValue(&fValueX, &fValueY) == true)
	{
	}
	else
	{ // 入力が行われていない場合は早期リターン
		return;
	}

	// 角度ごとにモード切り替え
	fRot = atan2f(fValueX, fValueY);
	if (fRot < -STICK_ROT)
	{ // MENU_RETRY
		nSelect = MENU_RETRY;
	}
	else if (fRot > STICK_ROT)
	{ // MENU_QUIT
		nSelect = MENU_QUIT;
	}
	else if (fRot >= -STICK_ROT && fRot <= STICK_ROT)
	{ // MENU_CONTINUE
		nSelect = MENU_CONTINUE;
	}

	// 異なるモードを選んだ際にのみ処理を実行
	if (m_menu != nSelect)
	{
		m_menu = (MENU)nSelect;
		CManager::GetSound()->Play(CSound::SE_SELECT);
	}
}

//=============================
// 通常選択処理
//=============================
void CPause::NormalSelect(void)
{
	// 入力情報取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	switch (m_menu)
	{
	case MENU_CONTINUE:	// continue
		if (pKeyboard->GetRepeat(DIK_UPARROW) == true
			|| pKeyboard->GetRepeat(DIK_W) == true
			|| pKeyboard->GetRepeat(DIK_RIGHTARROW) == true
			|| pKeyboard->GetRepeat(DIK_D) == true	// 上/右[keyboard]
			|| pJoypad->GetRepeat(CInputJoypad::JOYKEY_UP) == true
			|| pJoypad->GetRepeat(CInputJoypad::JOYKEY_RIGHT) == true)	// 上/右[joypad]
		{
			m_menu = MENU_QUIT;
			CManager::GetSound()->Play(CSound::SE_SELECT);
		}
		else if (pKeyboard->GetRepeat(DIK_DOWNARROW) == true
			|| pKeyboard->GetRepeat(DIK_S) == true
			|| pKeyboard->GetRepeat(DIK_LEFTARROW) == true
			|| pKeyboard->GetRepeat(DIK_A) == true	// 下/左[keyboard]
			|| pJoypad->GetRepeat(CInputJoypad::JOYKEY_DOWN) == true
			|| pJoypad->GetRepeat(CInputJoypad::JOYKEY_LEFT) == true)	// 下/左[joypad]
		{
			m_menu = MENU_RETRY;
			CManager::GetSound()->Play(CSound::SE_SELECT);
		}
		break;

	case MENU_RETRY:	// retry
		if (pKeyboard->GetRepeat(DIK_DOWNARROW) == true
			|| pKeyboard->GetRepeat(DIK_W) == true
			|| pKeyboard->GetRepeat(DIK_RIGHTARROW) == true
			|| pKeyboard->GetRepeat(DIK_D) == true	// 下/右[keyboard]
			|| pJoypad->GetRepeat(CInputJoypad::JOYKEY_DOWN) == true
			|| pJoypad->GetRepeat(CInputJoypad::JOYKEY_RIGHT) == true)	// 下/右[joypad]
		{
			m_menu = MENU_QUIT;
			CManager::GetSound()->Play(CSound::SE_SELECT);
		}
		else if (pKeyboard->GetRepeat(DIK_UPARROW) == true
			|| pKeyboard->GetRepeat(DIK_S) == true
			|| pKeyboard->GetRepeat(DIK_LEFTARROW) == true
			|| pKeyboard->GetRepeat(DIK_A) == true	// 上/左[keyboard]
			|| pJoypad->GetRepeat(CInputJoypad::JOYKEY_UP) == true
			|| pJoypad->GetRepeat(CInputJoypad::JOYKEY_LEFT) == true)	// 上/左[joypad]
		{
			m_menu = MENU_CONTINUE;
			CManager::GetSound()->Play(CSound::SE_SELECT);
		}
		break;

	case MENU_QUIT:	// quit
		if (pKeyboard->GetRepeat(DIK_UPARROW) == true
			|| pKeyboard->GetRepeat(DIK_W) == true
			|| pKeyboard->GetRepeat(DIK_RIGHTARROW) == true
			|| pKeyboard->GetRepeat(DIK_D) == true	// 上/右[keyboard]
			|| pJoypad->GetRepeat(CInputJoypad::JOYKEY_UP) == true
			|| pJoypad->GetRepeat(CInputJoypad::JOYKEY_RIGHT) == true)	// 上/右[joypad]
		{
			m_menu = MENU_CONTINUE;
			CManager::GetSound()->Play(CSound::SE_SELECT);
		}
		else if (pKeyboard->GetRepeat(DIK_DOWNARROW) == true
			|| pKeyboard->GetRepeat(DIK_S) == true
			|| pKeyboard->GetRepeat(DIK_LEFTARROW) == true
			|| pKeyboard->GetRepeat(DIK_A) == true	// 下/左[keyboard]
			|| pJoypad->GetRepeat(CInputJoypad::JOYKEY_DOWN) == true
			|| pJoypad->GetRepeat(CInputJoypad::JOYKEY_LEFT) == true)	// 下/左[joypad]
		{
			m_menu = MENU_RETRY;
			CManager::GetSound()->Play(CSound::SE_SELECT);
		}
		break;
	}
}

//=============================
// 決定処理
//=============================
void CPause::DecisionMenu(void)
{
	// 入力情報取得
	CInputMouse* pMouse = CManager::GetInputMouse();
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();
	int nLeft = pJoypad->GetVibrationLeft();
	int nRight = pJoypad->GetVibrationRight();

	if (CManager::GetFadeState() != CFade::STATE_FADENONE)
	{ // フェードをしていない場合のみ処理可能
		return;
	}

	// マウスクリックでの判定
	if (pMouse->GetTrigger(CInputMouse::BUTTON_LEFT) == true)
	{
		D3DXVECTOR3 posMouse = pMouse->GetClientPos();
		if (posMouse.x >= m_apPolygon[TYPE_CONTINUE]->GetPosition().x - MENU_WIDTH
			&& posMouse.x <= m_apPolygon[TYPE_CONTINUE]->GetPosition().x + MENU_WIDTH
			&& posMouse.y >= m_apPolygon[TYPE_CONTINUE]->GetPosition().y - MENU_HEIGHT
			&& posMouse.y <= m_apPolygon[TYPE_CONTINUE]->GetPosition().y + MENU_HEIGHT)
		{ // CONTINUEを選択
			m_bPause = false;
			m_menu = MENU_CONTINUE;// continueに合わせる
			CManager::GetSound()->Play(CSound::SE_ENTER);
			nLeft += VIBRATION;
			nRight += VIBRATION;
		}
		else if (posMouse.x >= m_apPolygon[TYPE_RETRY]->GetPosition().x - MENU_WIDTH
			&& posMouse.x <= m_apPolygon[TYPE_RETRY]->GetPosition().x + MENU_WIDTH
			&& posMouse.y >= m_apPolygon[TYPE_RETRY]->GetPosition().y - MENU_HEIGHT
			&& posMouse.y <= m_apPolygon[TYPE_RETRY]->GetPosition().y + MENU_HEIGHT)
		{ // RETRYを選択
			CManager::GetFade()->SetFade(CFade::TYPE_ALPHA, SCREEN_CENTER, COLOR_BLACK, CScene::MODE_GAME);
			CManager::GetSound()->Play(CSound::SE_ENTER);
			nLeft += VIBRATION;
			nRight += VIBRATION;
		}
		else if (posMouse.x >= m_apPolygon[TYPE_QUIT]->GetPosition().x - MENU_WIDTH
			&& posMouse.x <= m_apPolygon[TYPE_QUIT]->GetPosition().x + MENU_WIDTH
			&& posMouse.y >= m_apPolygon[TYPE_QUIT]->GetPosition().y - MENU_HEIGHT
			&& posMouse.y <= m_apPolygon[TYPE_QUIT]->GetPosition().y + MENU_HEIGHT)
		{ // QUITを選択
			CManager::GetFade()->SetFade(CFade::TYPE_ALPHA, SCREEN_CENTER, COLOR_BLACK, CScene::MODE_TITLE);
			CManager::GetSound()->Play(CSound::SE_ENTER);
			nLeft += VIBRATION;
			nRight += VIBRATION;
		}
	}

	// それ以外での判定
	if (pKeyboard->GetTrigger(DIK_RETURN) == true || pJoypad->GetTrigger(CInputJoypad::JOYKEY_A) == true)
	{
		switch (m_menu)
		{
		case MENU_CONTINUE:	// continue
			m_bPause = false;
			m_menu = MENU_CONTINUE;// continueに合わせる
			CManager::GetSound()->Play(CSound::SE_ENTER);
			break;

		case MENU_RETRY:	// retry
			CManager::GetFade()->SetFade(CFade::TYPE_ALPHA, SCREEN_CENTER, COLOR_BLACK, CScene::MODE_GAME);
			break;

		case MENU_QUIT:	// quit
			CManager::GetFade()->SetFade(CFade::TYPE_ALPHA, SCREEN_CENTER, COLOR_BLACK, CScene::MODE_TITLE);
			break;
		}
		CManager::GetSound()->Play(CSound::SE_ENTER);
		nLeft += VIBRATION;
		nRight += VIBRATION;
	}

	// 振動設定
	pJoypad->SetVibration(nLeft, nRight, DECIDE_VIBTIME);
}