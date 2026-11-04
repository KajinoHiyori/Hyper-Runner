//========================================================
// 
// デバッグ表示[debugproc.h]
// Author : Kajino Hiyori _Create2026/05/15
// 
//========================================================
#ifndef _DEBUG_PROC_H_
#define	_DEBUG_PROC_H_

//=============================
// マクロ定義
//=============================
#define MAX_DEBUGTEXT	(4096)	// デバッグ表示に格納できる最大数

//=============================
// デバッグ表示クラス
//=============================
class CDebugProc
{
public:
	CDebugProc();
	~CDebugProc();
	void Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Print(const char* fmt, ...);

private:
	LPD3DXFONT m_pFont;	// フォントへのポインタ
	char m_aStr[MAX_DEBUGTEXT];		// 画面に表示する文字列を格納
	bool m_bDisp;			// 表示状態を管理
};


#endif