//========================================================
// 
// Xファイル管理処理[fileX.h]
// Author : Kajino Hiyori _Create2026/10/22
//
//========================================================
#ifndef _X_FILE_H_
#define _X_FILE_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "texture.h"

//=============================
// マクロ定義
//=============================
#define NUM_FILEX	(256)	// 読み込むXファイルの最大数

//=============================
// Xファイルクラス
//=============================
class CFileX
{
public:
	CFileX();
	~CFileX();
	HRESULT Load(void);
	void Unload(void);
	int Register(const char* pFileName);
	LPD3DXMESH GetMesh(int nIdx) { return m_apMesh[nIdx]; }
	LPD3DXBUFFER GetBuffMat(int nIdx) { return m_apBuffMat[nIdx]; }
	DWORD GetNumMat(int nIdx) { return m_adwNumMat[nIdx]; }
	D3DXVECTOR3 GetVtxMin(int nIdx) { return m_aVtxMin[nIdx]; }
	D3DXVECTOR3 GetVtxMax(int nIdx) { return m_aVtxMax[nIdx]; }
	int GetIdxTexture(int nIdx, int nAddress) { return m_aIdxTexture[nIdx][nAddress]; }
	static int GetNumAll(void) { return m_nNumAll; }

private:
	char*			m_apFileName[NUM_FILEX];	// 相対パスのコピー
	LPD3DXMESH		m_apMesh[NUM_FILEX];		// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER	m_apBuffMat[NUM_FILEX];		// マテリアルへのポインタ
	DWORD			m_adwNumMat[NUM_FILEX];		// マテリアルの数
	D3DXVECTOR3		m_aVtxMin[NUM_FILEX];		// Xファイル最小値
	D3DXVECTOR3		m_aVtxMax[NUM_FILEX];		// Xファイル最大値
	int m_aIdxTexture[NUM_FILEX][NUM_TEXTURE];	// テクスチャインデックス
	static int m_nNumAll;	//  Xファイルの総数を格納
};
#endif // !_X_FILE_H_

