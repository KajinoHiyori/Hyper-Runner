//==========================================
// 
// 画面管理処理[mode.h]
// Author : Kajino Hiyori _Create2026/06/29
// 
//==========================================
#ifndef _MODE_H_
#define _MODE_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// シーンクラス
//=============================
class CScene : public CObject
{
public:
	typedef enum
	{
		//MODE_TITLE = 0,	// タイトル画面
		//MODE_TUTORIAL,	// チュートリアル画面
		MODE_GAME = 0,		// ゲーム画面
		//MODE_RESULT,	// リザルト画面
		//MODE_RANKING,		// ランキング画面
		MODE_MAX
	}MODE;
	static CScene* Create(MODE mode, bool bClear = false);
	CScene(MODE mode);
	~CScene();
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	MODE GetMode(void) { return m_mode; }
	void SetPosition(D3DXVECTOR3 pos) {}

private:
	MODE m_mode;	// 現在のモードを格納
};

#endif // !_MODE_H_