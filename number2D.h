//==========================================
// 
// 数値処理[number2D.h]
// Author : Kajino Hiyori _Create2026/05/19~
// 
//==========================================
#ifndef _NUMBER2D_H_
#define _NUMBER2D_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"

//=============================
// マクロ定義
//=============================
#define NUMBER_WIDTH	(50.0f)	// 数値の幅
#define NUMBER_HEIGHT	(50.0f)	// 数値の高さ

//=============================
// 数値クラス
//=============================
class CNumber2D
{
public:
	CNumber2D();
	~CNumber2D();
	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos, int nNum);
	HRESULT Init(D3DXVECTOR3 pos, int nNum, float fSize);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos);
	static CNumber2D* Create(D3DXVECTOR3 pos, int nNum);
	static CNumber2D* Create(D3DXVECTOR3 pos, int nNum, float fSize);
	void SetNum(int nNum);
	void SetColor(D3DXCOLOR col) { m_col = col; SetVtx(); }
	static int GetNumAll(void){ return m_nNumAll; }
	static int SetNumberTecU(int nDigit, int nSource, int nCnt);

private:
	void SetVtx(void);
	int m_nIdxTexture;						// テクスチャインデックス
	static const char* m_pFilenameType;		// テクスチャファイル名
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファ
	int m_nNum;	// 表示する数字
	D3DXVECTOR3 m_pos;	// 表示位置
	D3DXCOLOR m_col;	// 表示色
	float m_fWidth;		// 横幅
	float m_fHeight;	// 縦幅
	static int m_nNumAll;	// 数字の総数
};

#endif // !_NUMBER_H_

