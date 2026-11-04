//========================================================
// 
// 数値処理[number3D.h]
// Author : Kajino Hiyori _Create2026/06/01
//
//========================================================
#ifndef _NUMBER3D_H_
#define	_NUMBER3D_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"

//=============================
// マクロ定義
//=============================
#define NUM_VTX3D	(4)	// 頂点数

//=============================
// 3Dポリゴンクラス
//=============================
class CNumber3D
{
public:
	CNumber3D();
	~CNumber3D();
	static CNumber3D* Create(D3DXVECTOR3 pos, D3DXCOLOR col, float fWidth, float fHeight, int nNum);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXCOLOR col, float fWidth, float fHeight, int nNum);
	void SetTextureIdx(int nIdx) { m_nIdxTexture = nIdx; }
	void Uninit(void);
	void Update(void);
	void Draw(D3DXMATRIX mtxParent);
	void SetNum(int nNum) { m_nNum = nNum; SetVtx(); }
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetColor(D3DXCOLOR col) { m_col = col; SetVtxColor(); }
	static int SetTecU(int nDigit, int nSource, int nCnt);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXCOLOR GetColor(void) { return m_col; }

private:
	void SetVtx(void);
	void SetVtxColor(void);
	D3DXVECTOR3 m_pos;	// 表示位置
	D3DXVECTOR3 m_rot;	// 向き
	D3DXCOLOR m_col;	// 色
	float m_fWidth;		// 横幅
	float m_fHeight;	// 縦幅
	int m_nNum;			// 数値
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファ
	int m_nIdxTexture;		// テクスチャのインデックスを格納
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	static const char* m_pFilename;	// 共有ファイル名
};
#endif