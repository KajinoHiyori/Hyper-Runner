//========================================================
// 
// テクスチャ管理処理[texture.h]
// Author : Kajino Hiyori _Create2026/06/01
//
//========================================================
#ifndef _TEXTURE_H_
#define _TEXTURE_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"

//=============================
// マクロ定義
//=============================
#define NUM_TEXTURE	(256)	// 読み込むテクスチャの最大数

//=============================
// テクスチャクラス
//=============================
class CTexture
{
public:
	CTexture();
	~CTexture();
	HRESULT Load(void);
	void Unload(void);
	int Register(const char* pFileName);
	LPDIRECT3DTEXTURE9 GetAddress(int nIdx);
	static int GetNumAll(void) { return m_nNumAll; }

private:
	char* m_apFileName[NUM_TEXTURE];			// 相対パスのコピー
	LPDIRECT3DTEXTURE9 m_apTexture[NUM_TEXTURE];	// テクスチャ格納
	static int m_nNumAll;	// テクスチャの総数を格納
};
#endif // !_TEXTURE_H_