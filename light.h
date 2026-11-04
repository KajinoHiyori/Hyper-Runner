//========================================================
// 
// 光源処理[light.h]
// Author : Kajino Hiyori _Create2026/06/01
//
//========================================================
#ifndef _LIGHT_H_
#define _LIGHT_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"

//=============================
// マクロ定義
//=============================
#define NUM_LIGHT			(3)	// ライトの最大

//=============================
// 前方宣言
//=============================
class CScene;	// シーン情報

//=============================
// ライトクラス
//=============================
class CLight
{
public:
	CLight();
	~CLight();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Set(CScene* pScene);
	void Clear(void);
	void Over(void);

private:
	void Default(void);
	D3DLIGHT9 m_aLight[NUM_LIGHT];	// ライトの情報
};

#endif