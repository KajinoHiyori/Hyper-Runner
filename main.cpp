//========================================================================
// 
// メイン処理[main.cpp]
// Author : Kajino Hiyori _Create2026/05/08~
// 
//========================================================================
#include "main.h"
#include "manager.h"
#include "debugproc.h"

//=============================
// マクロ定義
//=============================
#define CLASS_NAME	"WindowClass"			// ウィンドウクラスの名前
#define END_TEXT	"QUIT GAME?"		// exe終了時のテキスト  
#ifdef _DEBUG
#define FULL_SCREEN	(TRUE)	// フルスクリーン状態を管理[TRUE : 通常 / FALSE : フルスクリーン]
#else
//#define FULL_SCREEN	(FALSE)	// フルスクリーン状態を管理[TRUE : 通常 / FALSE : フルスクリーン]
#define FULL_SCREEN	(TRUE)	// フルスクリーン状態を管理[TRUE : 通常 / FALSE : フルスクリーン]
#endif // _DEBUG

#define MAX_POLYGON	(2)	// オブジェクト最大数

//=============================
// プロトタイプ宣言
//=============================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // ウィンドウプロシージャ

//=============================
// グローバル変数宣言
//=============================
HWND g_hWnd;	// ウィンドウハンドル(識別子)

//========================================================================
// メイン関数
//========================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, LPSTR lpCmdLine, int nCmdShow)
{
	CManager* pManager = NULL;	// マネージャーのポインタを格納

	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }; // 画面サイズの構造体

	// ↓ ウィンドウクラスの構造体
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),					// WNDCLASSEXのメモリサイズ
		CS_CLASSDC,							// ウィンドウのスタイル
		WindowProc,							// ウィンドウプロシージャ
		0,									// 0にする(通常は使用しない)
		0,									// 0にする(通常は使用しない)
		hInstance,							// インスタントハンドル
		LoadIcon(NULL, IDI_APPLICATION),	// タスクバーのアイコン
		LoadCursor(NULL, IDC_ARROW),		// マウスカーソル
		(HBRUSH)(COLOR_WINDOW + 1),			// クライアント領域の背景色を白にする
		NULL,								// メニューバー
		CLASS_NAME,							// ウィンドウクラスの名前
		LoadIcon(NULL, IDI_APPLICATION),	// ファイルのアイコン
	};

	MSG msg;	// メッセージを格納する変数
	DWORD dwCurrentTime;	// 現在時刻
	DWORD dwExecLastTime;	// 最後に処理した時刻
	DWORD dwFrameCount;		// フレームカウント
	DWORD dwFPSLastTime;	// 最後にFPSを計測した時間
	int nCounterFPS = 0;	// FPSカウンター

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// クライアント領域を指定のサイズに指定
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウィンドウを生成
	g_hWnd = CreateWindowEx(0,		// 拡張ウィンドウスタイル
		CLASS_NAME,					// ウィンドウクラスの名前
		WINDOW_NAME,				// ウィンドウの名前
		WS_OVERLAPPEDWINDOW,		// ウィンドウスタイル
		CW_USEDEFAULT,				// ウィンドウの左上X座標
		CW_USEDEFAULT,				// ウィンドウの左上Y座標
		(rect.right - rect.left),	// ウィンドウの幅
		(rect.bottom - rect.top),	// ウィンドウの高さ
		NULL,						// 親ウィンドウのハンドル
		NULL,						// メニューハンドルまたは子ウィンドウのID
		hInstance,					// インスタンスハンドル
		NULL);						// ウィンドウ作成データ

	// 分解能の設定
	timeBeginPeriod(1);
	dwCurrentTime = 0;	// 初期化
	dwExecLastTime = timeGetTime();	// 現在時刻を取得して保存
	dwFrameCount = 0;		// 初期化
	dwFPSLastTime = timeGetTime();

	// マネージャー生成
	if (pManager == NULL)
	{
		pManager = new CManager;	// マネージャーインスタンス生成

		if (pManager != NULL)
		{ // インスタンス生成に成功
			// マネージャーの初期化
			pManager->Init(hInstance, g_hWnd, FULL_SCREEN);
		}
	}

	if (pManager == NULL)
	{ // マネージャーインスタンス生成に失敗
		int nID = MessageBox(g_hWnd, "マネージャー生成に失敗", WINDOW_NAME, MB_OK);
	}

	// ウィンドウの表示
	ShowWindow(g_hWnd, nCmdShow);	// ウィンドウの表示状態を設定
	UpdateWindow(g_hWnd);			// クライアント領域を設定

	// メッセージループ
	// メッセージキューからメッセージを取得→メッセージを取得しなかった場合0を返す
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{ // Windowsの処理
			if (msg.message == WM_QUIT)
			{ // WM_QUITメッセージを受け取ったらループを抜ける
				break;
			}
			else
			{
				// メッセージの設定
				TranslateMessage(&msg);	// 仮想キーメッセージを文字メッセージへ変換
				DispatchMessage(&msg);	// ウィンドウプロシージャへメッセージを送出
			}
		}
		else
		{ // DirectXの処理
			dwCurrentTime = timeGetTime(); // 現在時刻を取得
			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{ // 0.5秒経過
				// FPSを計測
				nCounterFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);
				dwFPSLastTime = dwCurrentTime;	// FPSを測定した時刻を保存
				dwFrameCount = 0;		// フレームレートのクリア
			}
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
				dwExecLastTime = dwCurrentTime;	// 処理開始の時刻(現在時刻)を保存
				
				CDebugProc* pDebugProc = CManager::GetDebugProc();	// デバッグ表示の設定
				pDebugProc->Print("FPS : %d\n", nCounterFPS);

				// マネージャー更新処理
				pManager->Update();

				// マネージャー描画処理
				pManager->Draw();

				dwFrameCount++;
			}
		}
	}

	// マネージャー破棄
	SAFE_UNINIT(pManager);

	// 分解能を戻す
	timeEndPeriod(1);

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	return (int)msg.wParam;
}

//========================================================================
// ウィンドウプロシージャ
//========================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nID;

	switch (uMsg)
	{
	case WM_DESTROY: // ウィンドウ破棄のメッセージ
		// WM_QUITメッセージを送る
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN: // キー押下のメッセージ
		switch (wParam)
		{
		case VK_ESCAPE: // ESCが押された場合
			nID = MessageBox(hWnd, END_TEXT, WINDOW_NAME, MB_YESNO);
			if (nID == IDYES)
			{
				// ウィンドウを破棄する→WM_DESTROYメッセージを送る
				DestroyWindow(hWnd);
			}
			break;
		}
		break;

	case WM_CLOSE: // ×ボタンを押された場合
		nID = MessageBox(hWnd, END_TEXT, WINDOW_NAME, MB_YESNO);
		if (nID == IDYES)
		{
			// ウィンドウを破棄する→WM_DESTROYメッセージを送る
			DestroyWindow(hWnd);
		}
		else
		{
			return 0;
		}
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam); // 既定の処理を返す
}

//========================================================================
// 角度の正規化
//========================================================================
float AngleNormalize(float fAngle)
{
	int nNum = 0;

	while (1)
	{
		if (fAngle > D3DX_PI)
		{
			fAngle -= D3DX_PI * 2;
		}
		if (fAngle < -D3DX_PI)
		{
			fAngle += D3DX_PI * 2;
		}

		if ((fAngle <= D3DX_PI && fAngle >= -D3DX_PI) || nNum >= 5)
		{ // 正規化に成功した場合処理を抜ける
			break;
		}
		nNum++;
	};
	return fAngle;
}


//=============================
// 外積判定
//=============================
bool CrossCollisionLeft(D3DXVECTOR3 pos, D3DXVECTOR3 posPoint, D3DXVECTOR3 vecLine)
{
	bool bCollision = false;	// 衝突判定

	D3DXVECTOR3 vecToPos = pos - posPoint;
	if (((vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z)) > 0.0f)
	{
		bCollision = true;
	}
	return bCollision;
}

//=============================
// 外積判定
//=============================
bool CrossCollisionRight(D3DXVECTOR3 pos, D3DXVECTOR3 posPoint, D3DXVECTOR3 vecLine)
{
	bool bCollision = false;	// 衝突判定

	D3DXVECTOR3 vecToPos = pos - posPoint;
	if (((vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z)) < 0.0f)
	{ // モデルの左奥基点
		bCollision = true;
	}
	return bCollision;
}

//=============================
// アプリケーション終了関数
//=============================
void DestroyApp(void)
{
	// ウィンドウを破棄する→WM_DESTROYメッセージを送る
	DestroyWindow(g_hWnd);
}
