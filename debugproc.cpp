//========================================================
// 
// デバッグ表示[debugproc.cpp]
// Author : Kajino Hiyori _Create2026/05/15
// 
//========================================================
#include "main.h"
#include "debugproc.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"

//=============================
// コンストラクタ
//=============================
CDebugProc::CDebugProc()
{
	m_pFont = NULL;	// フォントへのポインタ
	memset(&m_aStr[0], 0, sizeof m_aStr);		// 画面に表示する文字列を格納
#ifdef _DEBUG
	m_bDisp = true;	// 表示状態を管理
#else
	m_bDisp = false;
#endif
}

//=============================
// デストラクタ
//=============================
CDebugProc::~CDebugProc()
{

}

//=============================
// 初期化処理
//=============================
void CDebugProc::Init(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// デバッグ表示用のフォントの生成
	D3DXCreateFont(pDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
		"PixelMplus12", &m_pFont);
}

//=============================
// デバッグ表示の終了処理
//=============================
void CDebugProc::Uninit(void)
{
	// デバッグ用のフォントの破棄
	SAFE_RELEASE(m_pFont);
}

//=============================
// デバッグ表示の更新処理
//=============================
void CDebugProc::Update(void)
{
	// 入力情報の取得
	CInputKeyboard* pInputLeyboard = CManager::GetInputKeyboard();

	// 表示切替
	if (pInputLeyboard->GetTrigger(DIK_F1) == true)
	{ // F1キーで表示切替
		m_bDisp = m_bDisp ^ 1 ? true : false;
	}
}

//=============================
// デバッグ表示の描画処理
//=============================
void CDebugProc::Draw(void)
{
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	if (m_bDisp == true)
	{
		// テキストの描画
		m_pFont->DrawText(NULL, &m_aStr[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 0, 0, 255));
	}
	// デバッグ情報バッファのクリア
	memset(&m_aStr[0], 0, sizeof m_aStr);
}

//=============================
// デバッグ情報を登録
//=============================
void CDebugProc::Print(const char* fmt, ...)
{
	char aStr[256] = {};

	// 可変長引数リストの変数を定義する
	va_list args;

	// リストに受けとった文字列を代入
	va_start(args, fmt);

	// リストからバッファへ文字列を代入
	vsprintf(&aStr[0], fmt, args);

	// 終了
	va_end(args);

	// デバッグ表示バッファに受け取った文字列を連結
	strcat(&m_aStr[0], &aStr[0]);
}