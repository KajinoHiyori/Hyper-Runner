//==========================================
// 
// フェード処理[fade.h]
// Author : Kajino Hiyori _Create2026/07/06~
// 
//==========================================
#ifndef _FADE_H_
#define _FADE_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "scene.h"

//=============================
// 前方宣言
//=============================
class CPolygon2D;	// 2Dポリゴンクラス[Object継承無し]

//=============================
// フェードクラス
//=============================
class CFade
{
public:
	// フェードの状態管理
	typedef enum
	{
		STATE_FADENONE = 0,	// フェード無し
		STATE_FADEIN,		// フェードイン
		STATE_FADEOUT,		// フェードアウト
	}STATE;

	// フェードの種類
	typedef enum
	{
		TYPE_NONE = 0,	// 初期化
		TYPE_ALPHA,		// アルファ値変更
		TYPE_SHUTTER,	// シャッター[Y方向]
		TYPE_MAX
	}TYPE;

	CFade();
	~CFade();
	static CFade* Create(TYPE type, D3DXVECTOR3 pos, D3DXCOLOR col);
	HRESULT Init(TYPE type, D3DXVECTOR3 pos, D3DXCOLOR col);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	STATE GetState(void) { return m_state; }
	void SetFade(TYPE type, D3DXVECTOR3 pos, D3DXCOLOR col, CScene::MODE mode, STATE state = STATE_FADEOUT);

private:
	CPolygon2D* m_pPolygon2D;	// ポリゴン2Dへのポインタ
	STATE m_state;	// フェード状態
	TYPE m_type;	// フェードの種類
	CScene::MODE m_modeNext;	// 次のモード
};

#endif // !_FADE_H_