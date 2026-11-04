//========================================================
//
// 入力処理 [input.h]
// Author : Kajino Hiyori _Create2025/07/10
// Update : 2026/05/15
// 
//========================================================
#ifndef _INPUT_H_
#define	_INPUT_H_
#include "main.h"

//===============================
// マクロ定義
//===============================
#define NUM_KEY_MAX			(256)		// キーの最大数

//===============================
// 入力クラス
//===============================
class CInput
{
public:
	CInput();
	virtual ~CInput();
	virtual HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void) = 0;
protected:
	static LPDIRECTINPUT8	m_pInput;	// DirectInputオブジェクトへのポインタ
	LPDIRECTINPUTDEVICE8	m_pDevice;	// 入力デバイスへのポインタ
};

//===============================
// キーボードクラス[親 :CInput]
//===============================
class CInputKeyboard : public CInput
{
public:
	CInputKeyboard();
	~CInputKeyboard();
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey);
	bool GetTrigger(int nKey);
	bool GetRelease(int nKey);
	bool GetRepeat(int nKey);
	bool GetWASDTrigger(void);
	bool GetWASDRelease(void);
	bool GetIJKLTrigger(void);
	bool GetIJKLRelease(void);
	bool GetAnyTrigger(void);

private:
	BYTE m_aKeyState[NUM_KEY_MAX];			// プレス情報
	BYTE m_aKeyStateOld[NUM_KEY_MAX];		// 1フレーム前の入力情報
	BYTE m_aKeyStateTrigger[NUM_KEY_MAX];	// トリガー情報
	BYTE m_aKeyStateRelease[NUM_KEY_MAX];	// リリース情報
	BYTE m_aKeyStateRepeat[NUM_KEY_MAX];	// リピート情報
	int m_aCountRepeat[NUM_KEY_MAX];		// リピート時間を計測
};

//===============================
// マウスクラス[親 :CInput]
//===============================
class CInputMouse : public CInput
{
public:
	typedef enum
	{
		BUTTON_LEFT = 0,	// マウス左クリック
		BUTTON_RIGHT,		// マウス右クリック
		BUTTON_CENTER,		// ホイールクリック
		BUTTON_MAX
	}BUTTON;

	CInputMouse();
	~CInputMouse();
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(BUTTON button);
	bool GetTrigger(BUTTON button);
	bool GetRelease(BUTTON button);
	bool GetRepeat(BUTTON button);
	D3DXVECTOR3 GetClientPos(void);

private:
	HWND m_hWnd;	// ウィンドウ情報
	DIMOUSESTATE2 m_mouseState;			// プレス情報
	DIMOUSESTATE2 m_mouseStateTrigger;	// トリガー情報
	DIMOUSESTATE2 m_mouseStateRelease;	// リリース情報
	DIMOUSESTATE2 m_mouseStateRepeat;	// リピート情報
	int m_aCountRepeat[BUTTON_MAX];		// リピート時間を計測
};

//===============================
// ジョイパッドクラス[親 :CInput]
//===============================
class CInputJoypad : public CInput
{
public:
	// ジョイパッドのキーの種類
	typedef enum
	{
		JOYKEY_UP = 0,			// [0]十字キー(上)
		JOYKEY_DOWN,			// [1]十字キー(下)
		JOYKEY_LEFT,			// [2]十字キー(左)
		JOYKEY_RIGHT,			// [3]十字キー(右)
		JOYKEY_START,			// [4]スタートボタン
		JOYKEY_BACK,			// [5]バックボタン
		JOYKEY_STICKLEFT,		// [6]スティック押し込み(左)
		JOYKEY_STICKRIGHT,		// [7]スティック押し込み(右)
		JOYKEY_LB,				// [8]LBボタン
		JOYKEY_RB,				// [9]RBボタン
		JOYKEY_LT,				// [10]LTボタン
		JOYKEY_RT,				// [11]RTボタン
		JOYKEY_A,				// [12]Aボタン
		JOYKEY_B,				// [13]Bボタン
		JOYKEY_X,				// [14]Xボタン
		JOYKEY_Y,				// [15]Yボタン
		JOYKEY_LSTICK_UP,		// [16]左スティックを上に倒す
		JOYKEY_LSTICK_DOWN,		// [17]左スティックを下に倒す
		JOYKEY_LSTICK_LEFT,		// [18]左スティックを左に倒す
		JOYKEY_LSTICK_RIGHT,	// [19]左スティックを右に倒す
		JOYKEY_RSTICK_UP,		// [20]右スティックを上に倒す
		JOYKEY_RSTICK_DOWN,		// [21]右スティックを下に倒す
		JOYKEY_RSTICK_LEFT,		// [22]右スティックを左に倒す
		JOYKEY_RSTICK_RIGHT,	// [23]右スティックを右に倒す
		JOYKEY_MAX
	}JOYKEY;

	CInputJoypad();
	~CInputJoypad();
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(JOYKEY key);	
	bool GetTrigger(JOYKEY key);
	bool GetRelease(JOYKEY key);
	bool GetRepeat(JOYKEY key);	
	bool GetAnyButton(void);

	bool GetLeftStick(JOYKEY key);
	bool GetLeftStickValue(float* pValueH, float* pValueV);
	bool GetLeftStickTrigger(JOYKEY key);
	bool GetLeftStickRelease(JOYKEY key);
	bool GetLeftStickRepeat(JOYKEY key);
	bool GetLeftStickAnyTrigger(void);
	bool GetLeftStickAnyRelease(void);

	bool GetRightStick(JOYKEY key);
	bool GetRightStickValue(float* pValueH, float* pValueV);
	bool GetRightStickTrigger(JOYKEY key);
	bool GetRightStickRelease(JOYKEY key);
	bool GetRightStickRepeat(JOYKEY key);
	bool GetRightStickAnyTrigger(void);
	bool GetRightStickAnyRelease(void);

	void SetVibration(int left, int right, int time);
	int GetVibrationLeft(void) { return m_joyKeyVibration.wLeftMotorSpeed; }
	int GetVibrationRight(void) { return m_joyKeyVibration.wRightMotorSpeed; }
	int GetVibrationTime(void) { return m_nVibrationTime; }
	void StopVibration(void);
	XINPUT_STATE GetJoypadState(void);

private:
	XINPUT_STATE	m_joyKeyState;			// プレス情報
	XINPUT_STATE	m_joyKeyStateOld;		// 1フレーム前のプレス情報
	XINPUT_STATE	m_joyKeyStateTrigger;	// トリガー情報
	XINPUT_STATE	m_joyKeyStateRelease;	// リリース情報
	XINPUT_VIBRATION	m_joyKeyVibration;	// バイブレーション情報
	int m_aCountRepeat[JOYKEY_MAX];		// リピート時間を計測
	int m_nVibrationTime = 0;				// バイブレーションの継続時間を計測
};

#endif