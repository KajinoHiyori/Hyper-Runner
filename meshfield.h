//========================================================
// 
// メッシュフィールド処理[meshfield.h]
// Author : Kajino Hiyori _Create2026/06/12
//
//========================================================
#ifndef _MESHFIELD_H_
#define	_MESHFIELD_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// マクロ定義
//=============================
#define DIVISION_W		(10)	// 横の分割数
#define DIVISION_D		(10)	// 奥行の分割数
#define WIDTH_SIZE		(DIVISION_W + 1)	// 横幅の大きさ
#define DEPTH_SIZE		(DIVISION_D + 1)	// 奥行きの大きさ
#define VERTEX			(WIDTH_SIZE * DEPTH_SIZE)	// 頂点数
#define FIELD_SIZE		(500.0f)	// 大きさ
#define FIELD_SIZE_HALF	(FIELD_SIZE * 0.5f)	// 半分の大きさ
#define MAX_VERTEX		(16384)	// 頂点の最大数

//=============================
// メッシュフィールドクラス
//=============================
class CMeshField : public CObject
{
public:
	// 面の種類
	typedef enum
	{
		TYPE_FLAT = 0,	// 平面
		TYPE_UNEVEN,	// 凹凸有
		TYPE_MAX
	}TYPE;

	CMeshField(CObject::PRIORITY priority = CObject::PRIORITY_FIELD);
	~CMeshField();
	static CMeshField* Create(D3DXVECTOR3 pos, TYPE type);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(D3DXVECTOR3 pos, TYPE type);
	HRESULT Init(D3DXVECTOR3 pos, TYPE type, float fWidth, float fDepth, int nDivision, int nMaxHeight, float fHeightValue, float fTexSize);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetTextureIdx(int nIdx) { m_nIdxTexture = nIdx; }
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; }
	float GetHeight(D3DXVECTOR3 pos, D3DXVECTOR3* pNor = NULL);
	float GetWidth(void) {return m_fWidth * 0.5f;}
	float GetDepth(void) {return m_fDepth * 0.5f;}
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetNormalize(void) { return m_nor; }
	void SetReferencePoint(D3DXVECTOR3 pos) { m_posReference = pos; }
	void SetReferenceRadius(float fRadius) { m_fReferenceRadius = fRadius; }
	void Dent(D3DXVECTOR3 pos, float fSize, float fDepth);
	void Bump(D3DXVECTOR3 pos, float fSize, float fPeak);
	bool GetUnevenness(void) { if (m_bBump == true) return true; else return false; }

private:
	void SetVtxBuff(TYPE type);
	void SetVtxBuff(void);
	void SetIdxBuff(void);
	void SetNormalize(void);
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;	// インデックスバッファへのポインタ
	D3DXMATRIX m_mtxWorld;				// ワールドマトリックス
	D3DXVECTOR3 m_pos;					// メッシュフィールドの位置
	D3DXVECTOR3 m_rot;					// メッシュフィールドの向き
	D3DXVECTOR3 m_nor;					// メッシュフィールドの法線方向
	D3DXVECTOR3 m_posReference;			// 判定基準値
	float m_fReferenceRadius;			// 判定範囲
	float m_fWidth;			// メッシュフィールドの幅
	float m_fDepth;			// メッシュフィールドの奥行き
	int m_nMaxHeight;		// メッシュフィールドの最高高度
	float m_fHeightValue;	// メッシュフィールドの高度乗算値
	int m_nDivision;		// 分割数
	int m_nDivisionAdd1;	// 分割数+1
	int m_nNumVertex;		// 頂点数
	int m_nNumPrimitive;	// プリミティブ数
	int m_nNumIdx;			// インデックス数
	float m_fLoadTexRange;	// テクスチャ読み込み幅
	int m_nIdxTexture;					// テクスチャへのインデックス
	int m_nCntFrame;		// 凹凸管理カウンター
	bool m_bBump;			// 凸中
	bool m_bDent;			// 凹中
	const char* m_pFileName;			// ファイル名
	D3DXVECTOR3 m_aPosVtx[MAX_VERTEX];	// 頂点座標
	D3DXVECTOR3 m_aPosDestVtx[MAX_VERTEX];	// 頂点座標
	D3DXVECTOR3 m_aNor[MAX_VERTEX];		// 法線情報
};

#endif
