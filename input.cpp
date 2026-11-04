//==========================================================
// 
// 入力処理 [input.h]
// Author : Kajino Hiyori _Create2025/07/10
// Update : 2026/05/15
//
//==========================================================
#include "main.h"
#include "input.h"
#include "manager.h"
#include "debugproc.h"

//===============================
// マクロ定義
//===============================
#define REPEAT_INTERVAL			(20)		// リピートに入るまでのの間隔
#define TRIGGER_INTERVAL		(-15)		// トリガーからの間隔
#define STICK_DEADZONE			(3000)		// デッドゾーン
#define JOYSTICKVALUE_MAX		(32767)		// スティックの値の最大
#define JOYSTICKVALUE_MIN		(-32768)	// スティックの値の最小
#define JOYPAD_TRIGGER			(255)		// LTRTトリガーの値
#define JOYPAD_LEFT_TRIGGER		(0x0400)	// ZLトリガーのビット
#define JOYPAD_RIGHT_TRIGGER	(0x0800)	// ZRトリガーのビット

//===============================
// 静的メンバ変数宣言
//===============================
LPDIRECTINPUT8  CInput::m_pInput = NULL;	// DirectInputオブジェクトへのポインタをクリア

//==========================================================
// CInputクラス
//==========================================================
//===============================
// コンストラクタ
//===============================
CInput::CInput()
{
	m_pDevice = NULL;	// 入力デバイスへのポインタをクリア
}

//===============================
// デストラクタ
//===============================
CInput::~CInput()
{

}

//===============================
// 初期化処理
//===============================
HRESULT CInput::Init(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInput == NULL)
	{ // DirectInputオブジェクトが用意されていない
		// DirectInputオブジェクトの生成
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, NULL)))
		{ // 生成に失敗
			return E_FAIL;
		}
	}
	return S_OK;
}

//===============================
// 終了処理
//===============================
void CInput::Uninit(void)
{
	// デバイスの破棄
	if (m_pDevice != NULL)
	{
		m_pDevice->Unacquire();	// アクセス権を放棄
		m_pDevice->Release();
		m_pDevice = NULL;
	}
	// DirectInputオブジェクトの破棄
	SAFE_RELEASE(m_pInput);
}

//==========================================================
// CInputKeyboardクラス	[親 :CInput]
//==========================================================
//===============================
// コンストラクタ
//===============================
CInputKeyboard::CInputKeyboard()
{
	// キーボード情報のクリア
	memset(&m_aKeyState[0], 0, sizeof m_aKeyState);					// プレス情報
	memset(&m_aKeyStateOld[0], 0, sizeof m_aKeyStateOld);			// 過去のプレス情報
	memset(&m_aKeyStateTrigger[0], 0, sizeof m_aKeyStateTrigger);	// トリガー情報
	memset(&m_aKeyStateRelease[0], 0, sizeof m_aKeyStateRelease);	// リリース情報
	memset(&m_aKeyStateRepeat[0], 0, sizeof m_aKeyStateRepeat);		// リピート情報
	memset(&m_aCountRepeat[0], 0, sizeof(int) * NUM_KEY_MAX);		// 計測時間の初期化
}

//===============================
// デストラクタ
//===============================
CInputKeyboard::~CInputKeyboard()
{

}

//===============================
// 初期化処理
//===============================
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	// 入力の初期化処理
	if (FAILED(CInput::Init(hInstance, hWnd)))
	{ // 入力の初期化に失敗
		return E_FAIL;
	}

	// デバイスの生成
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	// データフォーマットを設定
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	// 協調モードを設定
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	// キーボードへのアクセス権を取得
	m_pDevice->Acquire();
	return S_OK;
}

//===============================
// 終了処理
//===============================
void CInputKeyboard::Uninit(void)
{
	// 入力の終了処理
	CInput::Uninit();
}

//===============================
// 更新処理
//===============================
void CInputKeyboard::Update(void)
{
	BYTE aKeyState[NUM_KEY_MAX];	// キーボードの入力情報

	// 入力デバイスからデータを取得
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			// 過去の入力情報を格納
			m_aKeyStateOld[nCntKey] = m_aKeyState[nCntKey];

			m_aKeyStateTrigger[nCntKey] = (~m_aKeyState[nCntKey]) & (aKeyState[nCntKey]);	// トリガー情報
			m_aKeyStateRelease[nCntKey] = (m_aKeyState[nCntKey] | aKeyState[nCntKey]) ^ aKeyState[nCntKey];	// リリース情報

			if (GetTrigger(nCntKey) == true)
			{ // トリガーからリピートに入るまで若干の余裕を持たせる
				m_aCountRepeat[nCntKey] = TRIGGER_INTERVAL;
				m_aKeyStateRepeat[nCntKey] = aKeyState[nCntKey];
			}
			if (GetPress(nCntKey) == true)
			{ // キーが押されている場合、時間を計測
				m_aCountRepeat[nCntKey]++;

				if (m_aCountRepeat[nCntKey] >= REPEAT_INTERVAL)
				{ // 一定時間経過するとリピート処理に移行
					m_aKeyStateRepeat[nCntKey] = aKeyState[nCntKey];
					m_aCountRepeat[nCntKey] = 0;
				}
				else
				{ // なにも情報を渡さない
					m_aKeyStateRepeat[nCntKey] = NULL;
				}
			}
			if (GetRelease(nCntKey) == true)
			{ // キーが話された場合カウンターを初期化
				m_aCountRepeat[nCntKey] = 0;
			}
			m_aKeyState[nCntKey] = aKeyState[nCntKey];	// キーボードのプレス情報を保存
		}
	}
	else
	{
		CDebugProc* pDebugProc = CManager::GetDebugProc();
		pDebugProc->Print("\nキーボードのアクセスを喪失\n");
		m_pDevice->Acquire();	// アクセス権取得
	}
}

//===============================
// プレス情報を取得
//===============================
bool CInputKeyboard::GetPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? true : false;
}

//===============================
// トリガー情報を取得
//===============================
bool CInputKeyboard::GetTrigger(int nKey)
{
	return (m_aKeyStateTrigger[nKey] & 0x80) ? true : false;
}

//===============================
// リリース情報を取得
//===============================
bool CInputKeyboard::GetRelease(int nKey)
{
	return (m_aKeyStateRelease[nKey] & 0x80) ? true : false;
}

//===============================
// リピート情報を取得
//===============================
bool CInputKeyboard::GetRepeat(int nKey)
{
	return (m_aKeyStateRepeat[nKey] & 0x80) ? true : false;
}

//===============================
// WASD移動のトリガー情報
//===============================
bool CInputKeyboard::GetWASDTrigger(void)
{
	// 前回の入力情報
	bool bWOld = (m_aKeyStateOld[DIK_W] & 0x80) ? true : false;
	bool bAOld = (m_aKeyStateOld[DIK_A] & 0x80) ? true : false;
	bool bSOld = (m_aKeyStateOld[DIK_S] & 0x80) ? true : false;
	bool bDOld = (m_aKeyStateOld[DIK_D] & 0x80) ? true : false;
	
	if (bWOld == false && bAOld == false && bSOld == false && bDOld == false)
	{ // 前回、どのキーも押されていない
	}
	else
	{ // 前回入力されている
		return false;
	}

	// 今回の入力情報
	bool bW = (m_aKeyState[DIK_W] & 0x80) ? true : false;
	bool bA = (m_aKeyState[DIK_A] & 0x80) ? true : false;
	bool bS = (m_aKeyState[DIK_S] & 0x80) ? true : false;
	bool bD = (m_aKeyState[DIK_D] & 0x80) ? true : false;

	if (bW == true)
	{ // Wが押された
		return true;
	}
	if (bA == true)
	{ // Aが押された
		return true;
	}
	if (bS == true)
	{ // Sが押された
		return true;
	}
	if (bD == true)
	{ // Dが押された
		return true;
	}

	return false;
}

//===============================
// WASD移動のリリース情報
//===============================
bool CInputKeyboard::GetWASDRelease(void)
{
	// 前回の入力情報
	bool bWOld = (m_aKeyStateOld[DIK_W] & 0x80) ? true : false;
	bool bAOld = (m_aKeyStateOld[DIK_A] & 0x80) ? true : false;
	bool bSOld = (m_aKeyStateOld[DIK_S] & 0x80) ? true : false;
	bool bDOld = (m_aKeyStateOld[DIK_D] & 0x80) ? true : false;

	if (bWOld == true || bAOld == true || bSOld == true || bDOld == true)
	{ // 前回入力されている
		// 今回の入力情報
		bool bW = (m_aKeyState[DIK_W] & 0x80) ? true : false;
		bool bA = (m_aKeyState[DIK_A] & 0x80) ? true : false;
		bool bS = (m_aKeyState[DIK_S] & 0x80) ? true : false;
		bool bD = (m_aKeyState[DIK_D] & 0x80) ? true : false;
		if (bW == false && bA == false && bS == false && bD == false)
		{
			return true;
		}
	}

	return false;
}

//===============================
// IJKL移動のトリガー情報
//===============================
bool CInputKeyboard::GetIJKLTrigger(void)
{
	// 前回の入力情報
	bool bIOld = (m_aKeyStateOld[DIK_I] & 0x80) ? true : false;
	bool bJOld = (m_aKeyStateOld[DIK_J] & 0x80) ? true : false;
	bool bKOld = (m_aKeyStateOld[DIK_K] & 0x80) ? true : false;
	bool bLOld = (m_aKeyStateOld[DIK_L] & 0x80) ? true : false;

	if (bIOld == false && bJOld == false && bKOld == false && bLOld == false)
	{ // 前回、どのキーも押されていない
	}
	else
	{ // 前回入力されている
		return false;
	}

	// 今回の入力情報
	bool bI = (m_aKeyState[DIK_I] & 0x80) ? true : false;
	bool bJ = (m_aKeyState[DIK_J] & 0x80) ? true : false;
	bool bK = (m_aKeyState[DIK_K] & 0x80) ? true : false;
	bool bL = (m_aKeyState[DIK_L] & 0x80) ? true : false;

	if (bI == true)
	{ // Wが押された
		return true;
	}
	if (bJ == true)
	{ // Aが押された
		return true;
	}
	if (bK == true)
	{ // Sが押された
		return true;
	}
	if (bL == true)
	{ // Dが押された
		return true;
	}

	return false;
}

//===============================
// IJKL移動のリリース情報
//===============================
bool CInputKeyboard::GetIJKLRelease(void)
{
	// 前回の入力情報
	bool bIOld = (m_aKeyStateOld[DIK_I] & 0x80) ? true : false;
	bool bJOld = (m_aKeyStateOld[DIK_J] & 0x80) ? true : false;
	bool bKOld = (m_aKeyStateOld[DIK_K] & 0x80) ? true : false;
	bool bLOld = (m_aKeyStateOld[DIK_L] & 0x80) ? true : false;

	if (bIOld == true || bJOld == true || bKOld == true || bLOld == true)
	{ // 前回入力されている
		// 今回の入力情報
		bool bI = (m_aKeyState[DIK_I] & 0x80) ? true : false;
		bool bJ = (m_aKeyState[DIK_J] & 0x80) ? true : false;
		bool bK = (m_aKeyState[DIK_K] & 0x80) ? true : false;
		bool bL = (m_aKeyState[DIK_L] & 0x80) ? true : false;
		if (bI == false && bJ == false && bK == false && bL == false)
		{
			return true;
		}
	}

	return false;
}

//===============================
// 任意のキートリガー情報
//===============================
bool CInputKeyboard::GetAnyTrigger(void)
{
	for (int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
	{
		if ((m_aKeyStateTrigger[nCntKey] & 0x80) ? true : false)
		{
			return true;
		}
	}

	return false;
}

//==========================================================
// CInputMouseクラス	[親 :CInput]
//==========================================================
//===============================
// コンストラクタ
//===============================
CInputMouse::CInputMouse()
{
	// マウス情報のクリア
	memset(&m_mouseState, 0, sizeof(DIMOUSESTATE2));		// プレス情報
	memset(&m_mouseStateTrigger, 0, sizeof(DIMOUSESTATE2));	// トリガー情報
	memset(&m_mouseStateRelease, 0, sizeof(DIMOUSESTATE2));	// リリース情報
	memset(&m_mouseStateRepeat, 0, sizeof(DIMOUSESTATE2));	// リピート情報
	memset(&m_aCountRepeat[0], 0, sizeof(int) * BUTTON_MAX);		// 計測時間の初期化
}

//===============================
// デストラクタ
//===============================
CInputMouse::~CInputMouse()
{

}

//===============================
// 初期化処理
//===============================
HRESULT CInputMouse::Init(HINSTANCE hInstance, HWND hWnd)
{
	// 入力の初期化処理
	if (FAILED(CInput::Init(hInstance, hWnd)))
	{ // 入力の初期化に失敗
		return E_FAIL;
	}

	// スクリーン情報を保存
	m_hWnd = hWnd;

	// デバイスの生成
	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	// データフォーマットを設定
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		return E_FAIL;
	}

	// 協調モードを設定
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	// アクセス権を取得
	if (FAILED(m_pDevice->Acquire()))
	{
	//	return E_FAIL;
	}
	return S_OK;
}

//===============================
// 終了処理
//===============================
void CInputMouse::Uninit(void)
{
	// 入力の終了処理
	CInput::Uninit();
}

//===============================
// 更新処理
//===============================
void CInputMouse::Update(void)
{
	DIMOUSESTATE2 mouseState;	// マウスの入力情報
	CDebugProc* pDebugProc = CManager::GetDebugProc();	// デバッグ表示の設定

	// 入力デバイスからデータを取得
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState)))
	{
		for (int nCntMouse = 0; nCntMouse < BUTTON_MAX; nCntMouse++)
		{
			m_mouseStateTrigger.rgbButtons[nCntMouse] = (~m_mouseState.rgbButtons[nCntMouse] & mouseState.rgbButtons[nCntMouse]);										// ジョイパッドのトリガー情報
			m_mouseStateRelease.rgbButtons[nCntMouse] = (m_mouseState.rgbButtons[nCntMouse] | mouseState.rgbButtons[nCntMouse]) ^ mouseState.rgbButtons[nCntMouse];			// ジョイパッドのリリース情報

			if ((mouseState.rgbButtons[nCntMouse] == m_mouseStateTrigger.rgbButtons[nCntMouse]) && (mouseState.rgbButtons[nCntMouse] != m_mouseState.rgbButtons[nCntMouse]))
			{ // トリガーからリピートに入るまで若干の余裕を持たせる
				m_aCountRepeat[nCntMouse] = TRIGGER_INTERVAL;
			}
			if (mouseState.rgbButtons[nCntMouse] == m_mouseState.rgbButtons[nCntMouse])
			{ // キーが押されている場合、時間を計測(今回の入力情報と前回の入力情報が同じ場合、ここに入る)
				m_aCountRepeat[nCntMouse]++;
				if (m_aCountRepeat[nCntMouse] >= REPEAT_INTERVAL)
				{ // 一定時間経過するとリピート処理に移行
					m_mouseStateRepeat = mouseState;
					m_aCountRepeat[nCntMouse] = 0;
				}
				else
				{ // なにも情報を渡さない
					m_mouseStateRepeat.rgbButtons[nCntMouse] = 0;
				}
			}
			if ((mouseState.rgbButtons[nCntMouse] != m_mouseStateTrigger.rgbButtons[nCntMouse]) && (mouseState.rgbButtons[nCntMouse] != m_mouseState.rgbButtons[nCntMouse]))
			{ // ボタンが離された場合、カウンターを初期化する
				m_aCountRepeat[nCntMouse] = 0;
			}
			m_mouseState.rgbButtons[nCntMouse] = mouseState.rgbButtons[nCntMouse];	// ジョイパッドのプレス状態を保存
		}
	}
	else
	{
		pDebugProc->Print("\nマウスのアクセスを喪失\n");
		m_pDevice->Acquire();	// アクセス権取得
	}

	// マウスの位置
	POINT mouse = { 0 };
	GetCursorPos(&mouse);
	pDebugProc->Print("スクリーン全体でのマウス座標 : %d, %d\n", mouse.x, mouse.y);

	// クライアント領域でのマウス位置
	D3DXVECTOR3 pos = GetClientPos();
	pDebugProc->Print("クライアント領域でのマウス座標 : %.2f, %.2f\n", pos.x, pos.y);
}

//===============================
// プレス情報を取得
//===============================
bool CInputMouse::GetPress(BUTTON button)
{
	return (m_mouseState.rgbButtons[button] & 0x80) ? true : false;
}

//===============================
// トリガー情報を取得
//===============================
bool CInputMouse::GetTrigger(BUTTON button)
{
	bool bTrigger = (m_mouseStateTrigger.rgbButtons[button] & 0x80) ? true : false;
	if (bTrigger == true)
	{
		return true;
	}
	return bTrigger;
}

//===============================
// リリース情報を取得
//===============================
bool CInputMouse::GetRelease(BUTTON button)
{
	return (m_mouseStateRelease.rgbButtons[button] & 0x80) ? true : false;
}

//===============================
// リピート情報を取得
//===============================
bool CInputMouse::GetRepeat(BUTTON button)
{
	return (m_mouseStateRepeat.rgbButtons[button] & 0x80) ? true : false;
}

//===============================
// クライアント領域での
// マウス座標を取得
//===============================
D3DXVECTOR3 CInputMouse::GetClientPos(void)
{
	POINT point = { 0 };
	// 画面全体でのマウスの位置
	GetCursorPos(&point);

	// クライアント領域でのマウス位置に変換
	ScreenToClient(m_hWnd, &point);

	if (point.x < 0)
	{ // 0以下
		point.x = 0;
	}
	else if (point.x > SCREEN_WIDTH)
	{ // 幅以上
		point.x = SCREEN_WIDTH;
	}

	if (point.y < 0)
	{ // 0以下
		point.y = 0;
	}
	else if (point.y > SCREEN_HEIGHT)
	{
		point.y = SCREEN_HEIGHT;
	}

	// D3DXVECTOR3型に変更
	D3DXVECTOR3 pos = INIT_D3DVEC3;
	pos.x = static_cast<float>(point.x);
	pos.y = static_cast<float>(point.y);
	pos.z = 0.0f;

	return pos;
}

//==========================================================
// CInputJoypadクラス	[親 :CInput]
//==========================================================
//===============================
// コンストラクタ
//===============================
CInputJoypad::CInputJoypad()
{
	// メモリのクリア
	memset(&m_joyKeyState, 0, sizeof(XINPUT_STATE));
	memset(&m_joyKeyStateOld, 0, sizeof(XINPUT_STATE));
	memset(&m_joyKeyStateTrigger, 0, sizeof(XINPUT_STATE));
	memset(&m_joyKeyStateRelease, 0, sizeof(XINPUT_STATE));
	memset(&m_aCountRepeat[0], 0, sizeof m_aCountRepeat);
	ZeroMemory(&m_joyKeyVibration, sizeof(XINPUT_VIBRATION));

	// バイブレーションの継続時間の初期化
	m_nVibrationTime = 0;
}

//===============================
// デストラクタ
//===============================
CInputJoypad::~CInputJoypad()
{

}

//===============================
// 初期化処理
//===============================
HRESULT CInputJoypad::Init(HINSTANCE hInstance, HWND hWnd)
{
	// 入力の初期化処理
	if (FAILED(CInput::Init(hInstance, hWnd)))
	{ // 入力の初期化に失敗
		return E_FAIL;
	}

	// XInputのステートを設定(有効にする)
	XInputEnable(true);

	return S_OK;
}

//===============================
// 終了処理
//===============================
void CInputJoypad::Uninit(void)
{
	// XInputのステートを設定(無効にする)
	XInputEnable(false);

	// 入力の終了処理
	CInput::Uninit();
}

//===============================
// 更新処理
//===============================
void CInputJoypad::Update(void)
{
	XINPUT_STATE joyKeyState;	// ジョイパッドの入力情報

	m_joyKeyStateOld = m_joyKeyState;	// 過去の入力情報を保存

	// ジョイパッドの状態を取得
	if (XInputGetState(0, &joyKeyState) == ERROR_SUCCESS)
	{
		// トリガー判定(LT/RTは入力値に補正を行う必要があるため)
		if (joyKeyState.Gamepad.bLeftTrigger >= JOYPAD_TRIGGER)
		{
			joyKeyState.Gamepad.wButtons += JOYPAD_LEFT_TRIGGER;
		}

		if (joyKeyState.Gamepad.bRightTrigger >= JOYPAD_TRIGGER)
		{
			joyKeyState.Gamepad.wButtons += JOYPAD_RIGHT_TRIGGER;
		}

		m_joyKeyStateTrigger.Gamepad.wButtons = (~m_joyKeyState.Gamepad.wButtons & joyKeyState.Gamepad.wButtons);										// ジョイパッドのトリガー情報
		m_joyKeyStateRelease.Gamepad.wButtons = (m_joyKeyState.Gamepad.wButtons | joyKeyState.Gamepad.wButtons) ^ joyKeyState.Gamepad.wButtons;			// ジョイパッドのリリース情報
		m_joyKeyState = joyKeyState;	// ジョイパッドのプレス状態を保存
	}
	m_nVibrationTime--;
	if (m_nVibrationTime <= 0)
	{
		StopVibration();	// バイブレーションの停止
	}
}

//===============================
// プレス情報を取得
//===============================
bool CInputJoypad::GetPress(JOYKEY key)
{
	return (m_joyKeyState.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//===============================
// トリガー情報を取得
//===============================
bool CInputJoypad::GetTrigger(JOYKEY key)
{
	return (m_joyKeyStateTrigger.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//===============================
// リリース情報を取得
//===============================
bool CInputJoypad::GetRelease(JOYKEY key)
{
	return (m_joyKeyStateRelease.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//===============================
// リピート情報を取得
//===============================
bool CInputJoypad::GetRepeat(JOYKEY key)
{
	m_aCountRepeat[key]++;	// リピート時間を加算

	if (GetTrigger(key) == true)
	{ // トリガーに入っていれば処理を続行
		m_aCountRepeat[key] = TRIGGER_INTERVAL;	// 少し間をあける
		return (m_joyKeyState.Gamepad.wButtons & (0x01 << key)) ? true : false;
	}
	if (m_aCountRepeat[key] >= REPEAT_INTERVAL)
	{ // 一定時間経過
		m_aCountRepeat[key] = 0;	// カウンターの初期化
		return (m_joyKeyState.Gamepad.wButtons & (0x01 << key)) ? true : false;
	}

	return false;
}

//===============================
// リピート情報を取得
//===============================
bool CInputJoypad::GetAnyButton(void)
{ 
	for (int key = 0; key <= JOYKEY_Y; key++)
	{
		if ((m_joyKeyStateTrigger.Gamepad.wButtons & (0x01 << key)) ? true : false)
		{
			return true;
		}
	}

	return false;
}

//===============================
// Lスティックの情報を取得
//===============================
bool CInputJoypad::GetLeftStick(JOYKEY key)
{
	if (key == JOYKEY_LSTICK_UP)	// 上
	{
		return (m_joyKeyState.Gamepad.sThumbLY >= STICK_DEADZONE) ? true : false;
	}
	else if (key == JOYKEY_LSTICK_DOWN)	// 下
	{
		return (m_joyKeyState.Gamepad.sThumbLY <= -STICK_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_LSTICK_LEFT)	// 左
	{
		return (m_joyKeyState.Gamepad.sThumbLX <= -STICK_DEADZONE) ? true : false;
	}
	else if (key == JOYKEY_LSTICK_RIGHT)	// 右
	{
		return (m_joyKeyState.Gamepad.sThumbLX >= STICK_DEADZONE) ? true : false;
	}
	else
	{
		return false;
	}
}

//===============================
// Lスティックの情報を取得
// 傾きによって正規化
//===============================
bool CInputJoypad::GetLeftStickValue(float* pValueH, float* pValueV)
{
	float fValueH = 0.0f;
	float fValueV = 0.0f;

	fValueH = m_joyKeyState.Gamepad.sThumbLX;
	fValueV = m_joyKeyState.Gamepad.sThumbLY;

	if ((sqrtf(fValueH * fValueH + fValueV * fValueV)) * 0.5f > STICK_DEADZONE)
	{
		fValueH = (fValueH) / (JOYSTICKVALUE_MAX - STICK_DEADZONE);
		fValueV = (fValueV) / (JOYSTICKVALUE_MAX - STICK_DEADZONE);

		// 値を渡す
		*pValueH = fValueH;
		*pValueV = fValueV;
		return true;
	}
	else
	{
		return false;
	}
}

//===============================
// Lスティックのトリガー情報を取得
//===============================
bool CInputJoypad::GetLeftStickTrigger(JOYKEY key)
{
	// 前回の入力情報を保存
	bool bLStickLeft = (m_joyKeyStateOld.Gamepad.sThumbLX <= -STICK_DEADZONE) ? true : false;
	bool bLStickRight = (m_joyKeyStateOld.Gamepad.sThumbLX >= STICK_DEADZONE) ? true : false;
	bool bLStickUp = (m_joyKeyStateOld.Gamepad.sThumbLY >= STICK_DEADZONE) ? true : false;
	bool bLStickDown = (m_joyKeyStateOld.Gamepad.sThumbLY <= -STICK_DEADZONE) ? true : false;

	if (bLStickLeft == false && (key == JOYKEY_LSTICK_LEFT) && (m_joyKeyState.Gamepad.sThumbLX <= -STICK_DEADZONE) == true)
	{ // 前回入力されていない && 左が入力された && deadzoneを越えている
		m_aCountRepeat[key] = TRIGGER_INTERVAL;
		return true;
	}
	if (bLStickRight == false && (key == JOYKEY_LSTICK_RIGHT) && (m_joyKeyState.Gamepad.sThumbLX >= STICK_DEADZONE) == true)
	{ // 前回入力されていない && 右が入力された && deadzoneを越えている
		m_aCountRepeat[key] = TRIGGER_INTERVAL;
		return  true;
	}
	if (bLStickUp == false && (key == JOYKEY_LSTICK_UP) && (m_joyKeyState.Gamepad.sThumbLY >= STICK_DEADZONE) == true)
	{ // 前回入力されていない && 上が入力された && deadzoneを越えている
		m_aCountRepeat[key] = TRIGGER_INTERVAL;
		return true;
	}
	if (bLStickDown == false && (key == JOYKEY_LSTICK_DOWN) && (m_joyKeyState.Gamepad.sThumbLY <= -STICK_DEADZONE) == true)
	{ // 前回入力されていない && 下が入力された && deadzoneを越えている
		m_aCountRepeat[key] = TRIGGER_INTERVAL;
		return true;
	}
	return false;
}

//===============================
// Lスティックのリリース情報を取得
//===============================
bool CInputJoypad::GetLeftStickRelease(JOYKEY key)
{
	// 前回の入力情報を保存
	bool bLStickLeft = (m_joyKeyStateOld.Gamepad.sThumbLX <= -STICK_DEADZONE) ? true : false;
	bool bLStickRight = (m_joyKeyStateOld.Gamepad.sThumbLX >= STICK_DEADZONE) ? true : false;
	bool bLStickUp = (m_joyKeyStateOld.Gamepad.sThumbLY >= STICK_DEADZONE) ? true : false;
	bool bLStickDown = (m_joyKeyStateOld.Gamepad.sThumbLY <= -STICK_DEADZONE) ? true : false;

	if (bLStickLeft == true && (key == JOYKEY_LSTICK_LEFT))
	{ // 前回入力されている && 左から離された && deadzoneを越えていない
		return (m_joyKeyState.Gamepad.sThumbLX >= -STICK_DEADZONE) ? true : false;
	}
	if (bLStickRight == true && (key == JOYKEY_LSTICK_RIGHT))
	{ // 前回入力されている && 右から離された && deadzoneを越えていない
		return (m_joyKeyState.Gamepad.sThumbLX <= STICK_DEADZONE) ? true : false;
	}
	if (bLStickUp == true && (key == JOYKEY_LSTICK_UP))
	{ // 前回入力されている && 上から離された && deadzoneを越えていない
		return (m_joyKeyState.Gamepad.sThumbLY <= STICK_DEADZONE) ? true : false;
	}
	if (bLStickDown == true && (key == JOYKEY_LSTICK_DOWN))
	{ // 前回入力されている && 下から離された && deadzoneを越えていない
		return (m_joyKeyState.Gamepad.sThumbLY >= -STICK_DEADZONE) ? true : false;
	}
	return false;
}

//===============================
// Lスティックのリピート情報を取得
//===============================
bool CInputJoypad::GetLeftStickRepeat(JOYKEY key)
{	
	m_aCountRepeat[key]++;	// リピート時間を加算

	if (GetLeftStickTrigger(key) == true)
	{ // トリガーに入っていれば処理を実行
		m_aCountRepeat[key] = TRIGGER_INTERVAL;	// 少し間をあける
		if (key == JOYKEY_LSTICK_UP)	// 上
		{
			return (m_joyKeyState.Gamepad.sThumbLY >= STICK_DEADZONE) ? true : false;
		}
		else if (key == JOYKEY_LSTICK_DOWN)	// 下
		{
			return (m_joyKeyState.Gamepad.sThumbLY <= -STICK_DEADZONE) ? true : false;
		}
		if (key == JOYKEY_LSTICK_LEFT)	// 左
		{
			return (m_joyKeyState.Gamepad.sThumbLX <= -STICK_DEADZONE) ? true : false;
		}
		else if (key == JOYKEY_LSTICK_RIGHT)	// 右
		{
			return (m_joyKeyState.Gamepad.sThumbLX >= STICK_DEADZONE) ? true : false;
		}
	}

	if (m_aCountRepeat[key] >= REPEAT_INTERVAL)
	{ // 一定時間経過
		m_aCountRepeat[key] = 0;	// カウンターの初期化

		if (key == JOYKEY_LSTICK_UP)	// 上
		{
			return (m_joyKeyState.Gamepad.sThumbLY >= STICK_DEADZONE) ? true : false;
		}
		else if (key == JOYKEY_LSTICK_DOWN)	// 下
		{
			return (m_joyKeyState.Gamepad.sThumbLY <= -STICK_DEADZONE) ? true : false;
		}
		if (key == JOYKEY_LSTICK_LEFT)	// 左
		{
			return (m_joyKeyState.Gamepad.sThumbLX <= -STICK_DEADZONE) ? true : false;
		}
		else if (key == JOYKEY_LSTICK_RIGHT)	// 右
		{
			return (m_joyKeyState.Gamepad.sThumbLX >= STICK_DEADZONE) ? true : false;
		}
	}
	return false;
}

//===============================
// 方向問わずLスティックの
// トリガー情報を取得
//===============================
bool CInputJoypad::GetLeftStickAnyTrigger(void)
{
	bool bLStickLeft = (m_joyKeyStateOld.Gamepad.sThumbLX <= -STICK_DEADZONE) ? true : false;
	bool bLStickRight = (m_joyKeyStateOld.Gamepad.sThumbLX >= STICK_DEADZONE) ? true : false;
	bool bLStickUp = (m_joyKeyStateOld.Gamepad.sThumbLY >= STICK_DEADZONE) ? true : false;
	bool bLStickDown = (m_joyKeyStateOld.Gamepad.sThumbLY <= -STICK_DEADZONE) ? true : false;
	if (bLStickLeft == false && bLStickRight == false && bLStickUp == false && bLStickDown == false)
	{ // 前回、どの方向にも倒されていない
	}
	else
	{ // 前回入力されている
		return false;
	}

	if ((m_joyKeyState.Gamepad.sThumbLX <= -STICK_DEADZONE) == true)
	{ // 左方向deadzoneを越えている
		return true;
	}
	if ((m_joyKeyState.Gamepad.sThumbLX >= STICK_DEADZONE) == true)
	{ // 右方向deadzoneを越えている
		return true;
	}
	if ((m_joyKeyState.Gamepad.sThumbLY >= STICK_DEADZONE) == true)
	{ // 上方向deadzoneを越えている
		return true;
	}
	if ((m_joyKeyState.Gamepad.sThumbLY <= -STICK_DEADZONE) == true)
	{ // 下方向deadzoneを越えている
		return true;
	}

	return false;
}

//===============================
// 方向問わずLスティックの
// リリース情報を取得
//===============================
bool  CInputJoypad::GetLeftStickAnyRelease(void)
{
	// 前回の入力情報を保存
	bool bLStickLeft = (m_joyKeyStateOld.Gamepad.sThumbLX <= -STICK_DEADZONE) ? true : false;
	bool bLStickRight = (m_joyKeyStateOld.Gamepad.sThumbLX >= STICK_DEADZONE) ? true : false;
	bool bLStickUp = (m_joyKeyStateOld.Gamepad.sThumbLY >= STICK_DEADZONE) ? true : false;
	bool bLStickDown = (m_joyKeyStateOld.Gamepad.sThumbLY <= -STICK_DEADZONE) ? true : false;

	if ((bLStickLeft == true || bLStickRight == true || bLStickUp == true || bLStickDown == true)		// 前回入力されている
		&& (m_joyKeyState.Gamepad.sThumbLX <= -STICK_DEADZONE == false		// 左が離された
		&& m_joyKeyState.Gamepad.sThumbLX >= STICK_DEADZONE == false		// 右が離された
		&& m_joyKeyState.Gamepad.sThumbLY >= STICK_DEADZONE == false		// 奥が離された
		&& m_joyKeyState.Gamepad.sThumbLY <= -STICK_DEADZONE == false))		// 手前が離された
	{
		return true;
	}

	return false;
}

//===============================
// Rスティックの情報を取得
//===============================
bool CInputJoypad::GetRightStick(JOYKEY key)
{
	if (key == JOYKEY_RSTICK_UP)	// 上
	{
		return (m_joyKeyState.Gamepad.sThumbRY >= STICK_DEADZONE) ? true : false;
	}
	else if (key == JOYKEY_RSTICK_DOWN)	// 下
	{
		return (m_joyKeyState.Gamepad.sThumbRY <= -STICK_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_RSTICK_LEFT)	// 左
	{
		return (m_joyKeyState.Gamepad.sThumbRX <= -STICK_DEADZONE) ? true : false;
	}
	else if (key == JOYKEY_RSTICK_RIGHT)	// 右
	{
		return (m_joyKeyState.Gamepad.sThumbRX >= STICK_DEADZONE) ? true : false;
	}
	else
	{
		return false;
	}
}

//===============================
// Rスティックの情報を取得
// 傾きによって正規化
//===============================
bool CInputJoypad::GetRightStickValue(float* pValueH, float* pValueV)
{
	float fValueH = 0.0f;
	float fValueV = 0.0f;

	fValueH = m_joyKeyState.Gamepad.sThumbRX;
	fValueV = m_joyKeyState.Gamepad.sThumbRY;

	if ((sqrtf(fValueH * fValueH + fValueV * fValueV)) * 0.5f > STICK_DEADZONE)
	{
		fValueH = (fValueH) / (JOYSTICKVALUE_MAX - STICK_DEADZONE);
		fValueV = (fValueV) / (JOYSTICKVALUE_MAX - STICK_DEADZONE);

		// 値を渡す
		*pValueH = fValueH;
		*pValueV = fValueV;
		return true;
	}
	else
	{
		return false;
	}
}

//===============================
// Rスティックのトリガー情報を取得
//===============================
bool CInputJoypad::GetRightStickTrigger(JOYKEY key)
{
	// 前回の入力情報を保存
	bool bRStickLeft = (m_joyKeyStateOld.Gamepad.sThumbRX <= -STICK_DEADZONE) ? true : false;
	bool bRStickRight = (m_joyKeyStateOld.Gamepad.sThumbRX >= STICK_DEADZONE) ? true : false;
	bool bRStickUp = (m_joyKeyStateOld.Gamepad.sThumbRY >= STICK_DEADZONE) ? true : false;
	bool bRStickDown = (m_joyKeyStateOld.Gamepad.sThumbRY <= -STICK_DEADZONE) ? true : false;

	if (bRStickLeft == false && (key == JOYKEY_RSTICK_LEFT))
	{ // 前回入力されていない && 左が入力された && deadzoneを越えている
		return (m_joyKeyState.Gamepad.sThumbRX <= -STICK_DEADZONE) ? true : false;
	}
	if (bRStickRight == false && (key == JOYKEY_RSTICK_RIGHT))
	{ // 前回入力されていない && 右が入力された && deadzoneを越えている
		return (m_joyKeyState.Gamepad.sThumbRX >= STICK_DEADZONE) ? true : false;
	}
	if (bRStickUp == false && (key == JOYKEY_RSTICK_UP))
	{ // 前回入力されていない && 上が入力された && deadzoneを越えている
		return (m_joyKeyState.Gamepad.sThumbRY >= STICK_DEADZONE) ? true : false;
	}
	if (bRStickDown == false && (key == JOYKEY_RSTICK_DOWN))
	{ // 前回入力されていない && 下が入力された && deadzoneを越えている
		return (m_joyKeyState.Gamepad.sThumbRY <= -STICK_DEADZONE) ? true : false;
	}
	return false;
}

//===============================
// Rスティックのリリース情報を取得
//===============================
bool CInputJoypad::GetRightStickRelease(JOYKEY key)
{
	// 前回の入力情報を保存
	bool bRStickReft = (m_joyKeyStateOld.Gamepad.sThumbRX <= -STICK_DEADZONE) ? true : false;
	bool bRStickRight = (m_joyKeyStateOld.Gamepad.sThumbRX >= STICK_DEADZONE) ? true : false;
	bool bRStickUp = (m_joyKeyStateOld.Gamepad.sThumbRY >= STICK_DEADZONE) ? true : false;
	bool bRStickDown = (m_joyKeyStateOld.Gamepad.sThumbRY <= -STICK_DEADZONE) ? true : false;

	if (bRStickReft == true && (key == JOYKEY_RSTICK_LEFT))
	{ // 前回入力されている && 左から離された && deadzoneを越えていない
		return (m_joyKeyState.Gamepad.sThumbRX >= -STICK_DEADZONE) ? true : false;
	}
	if (bRStickRight == true && (key == JOYKEY_RSTICK_RIGHT))
	{ // 前回入力されている && 右から離された && deadzoneを越えていない
		return (m_joyKeyState.Gamepad.sThumbRX <= STICK_DEADZONE) ? true : false;
	}
	if (bRStickUp == true && (key == JOYKEY_RSTICK_UP))
	{ // 前回入力されている && 上から離された && deadzoneを越えていない
		return (m_joyKeyState.Gamepad.sThumbRY <= STICK_DEADZONE) ? true : false;
	}
	if (bRStickDown == true && (key == JOYKEY_RSTICK_DOWN))
	{ // 前回入力されている && 下から離された && deadzoneを越えていない
		return (m_joyKeyState.Gamepad.sThumbRY >= -STICK_DEADZONE) ? true : false;
	}
	return false;
}

//===============================
// Rスティックのリピート情報を取得
//===============================
bool CInputJoypad::GetRightStickRepeat(JOYKEY key)
{
	m_aCountRepeat[key]++;	// リピート時間を加算

	if (GetRightStickTrigger(key) == true)
	{ // トリガーに入っていれば処理を実行
		m_aCountRepeat[key] = TRIGGER_INTERVAL;	// 少し間をあける
		if (key == JOYKEY_RSTICK_UP)	// 上
		{
			return (m_joyKeyState.Gamepad.sThumbRY >= STICK_DEADZONE) ? true : false;
		}
		else if (key == JOYKEY_RSTICK_DOWN)	// 下
		{
			return (m_joyKeyState.Gamepad.sThumbRY <= -STICK_DEADZONE) ? true : false;
		}
		if (key == JOYKEY_RSTICK_LEFT)	// 左
		{
			return (m_joyKeyState.Gamepad.sThumbRX <= -STICK_DEADZONE) ? true : false;
		}
		else if (key == JOYKEY_RSTICK_RIGHT)	// 右
		{
			return (m_joyKeyState.Gamepad.sThumbRX >= STICK_DEADZONE) ? true : false;
		}
	}

	if (m_aCountRepeat[key] >= REPEAT_INTERVAL)
	{ // 一定時間経過
		m_aCountRepeat[key] = 0;	// カウンターの初期化

		if (key == JOYKEY_RSTICK_UP)	// 上
		{
			return (m_joyKeyState.Gamepad.sThumbRY >= STICK_DEADZONE) ? true : false;
		}
		else if (key == JOYKEY_RSTICK_DOWN)	// 下
		{
			return (m_joyKeyState.Gamepad.sThumbRY <= -STICK_DEADZONE) ? true : false;
		}
		if (key == JOYKEY_RSTICK_LEFT)	// 左
		{
			return (m_joyKeyState.Gamepad.sThumbRX <= -STICK_DEADZONE) ? true : false;
		}
		else if (key == JOYKEY_RSTICK_RIGHT)	// 右
		{
			return (m_joyKeyState.Gamepad.sThumbRX >= STICK_DEADZONE) ? true : false;
		}
	}
	return false;
}

//===============================
// 方向問わずRスティックの
// トリガー情報を取得
//===============================
bool CInputJoypad::GetRightStickAnyTrigger(void)
{
	bool bRStickLeft = (m_joyKeyStateOld.Gamepad.sThumbRX <= -STICK_DEADZONE) ? true : false;
	bool bRStickRight = (m_joyKeyStateOld.Gamepad.sThumbRX >= STICK_DEADZONE) ? true : false;
	bool bRStickUp = (m_joyKeyStateOld.Gamepad.sThumbRY >= STICK_DEADZONE) ? true : false;
	bool bRStickDown = (m_joyKeyStateOld.Gamepad.sThumbRY <= -STICK_DEADZONE) ? true : false;
	if (bRStickLeft == false && bRStickRight == false && bRStickUp == false && bRStickDown == false)
	{ // 前回、どの方向にも倒されていない
	}
	else
	{ // 前回入力されている
		return false;
	}

	if ((m_joyKeyState.Gamepad.sThumbRX <= -STICK_DEADZONE) == true)
	{ // 左方向deadzoneを越えている
		return true;
	}
	if ((m_joyKeyState.Gamepad.sThumbRX >= STICK_DEADZONE) == true)
	{ // 右方向deadzoneを越えている
		return true;
	}
	if ((m_joyKeyState.Gamepad.sThumbRY >= STICK_DEADZONE) == true)
	{ // 上方向deadzoneを越えている
		return true;
	}
	if ((m_joyKeyState.Gamepad.sThumbRY <= -STICK_DEADZONE) == true)
	{ // 下方向deadzoneを越えている
		return true;
	}

	return false;
}

//===============================
// 方向問わずRスティックの
// リリース情報を取得
//===============================
bool CInputJoypad::GetRightStickAnyRelease(void)
{
	// 前回の入力情報を保存
	bool bRStickLeft = (m_joyKeyStateOld.Gamepad.sThumbRX <= -STICK_DEADZONE) ? true : false;
	bool bRStickRight = (m_joyKeyStateOld.Gamepad.sThumbRX >= STICK_DEADZONE) ? true : false;
	bool bRStickUp = (m_joyKeyStateOld.Gamepad.sThumbRY >= STICK_DEADZONE) ? true : false;
	bool bRStickDown = (m_joyKeyStateOld.Gamepad.sThumbRY <= -STICK_DEADZONE) ? true : false;

	if ((bRStickLeft == true	|| bRStickRight == true || bRStickUp == true || bRStickDown == true)		// 前回入力されている
		&& (m_joyKeyState.Gamepad.sThumbRX <= -STICK_DEADZONE == false
		&& m_joyKeyState.Gamepad.sThumbRX >= STICK_DEADZONE == false
		&& m_joyKeyState.Gamepad.sThumbRY >= STICK_DEADZONE == false
		&& m_joyKeyState.Gamepad.sThumbRY <= -STICK_DEADZONE == false))
	{
		return true;
	}

	return false;
}

//===============================
// バイブレーションを設定
//===============================
void CInputJoypad::SetVibration(int left, int right, int time)
{
	m_joyKeyVibration.wLeftMotorSpeed = left;
	m_joyKeyVibration.wRightMotorSpeed = right;
	m_nVibrationTime = time;
	XInputSetState(0, &m_joyKeyVibration);
}

//===============================
// バイブレーションを停止
//===============================
void CInputJoypad::StopVibration(void)
{
	m_joyKeyVibration.wLeftMotorSpeed = 0;
	m_joyKeyVibration.wRightMotorSpeed = 0;
	m_nVibrationTime = 0;
	XInputSetState(0, &m_joyKeyVibration);
}

//===============================
// ジョイパッドの情報を取得
//===============================
XINPUT_STATE CInputJoypad::GetJoypadState(void)
{
	return m_joyKeyState;
}