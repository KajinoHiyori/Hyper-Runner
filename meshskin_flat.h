//========================================================
// 
// メッシュスキン[平面]処理[meshskin_flat.h]
// Author : Kajino Hiyori _Create2026/11/11
//
//========================================================
#ifndef _MESHSKIN_FLAT_H_
#define	_MESHSKIN_FLAT_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// マクロ定義
//=============================
#define MAX_FLATBONE	(2)		// ボーン最大数
#define DIVISION_W		(10)	// 横の分割数
#define DIVISION_D		(10)	// 奥行の分割数
#define WIDTH_SIZE		(DIVISION_W + 1)	// 横幅の大きさ
#define DEPTH_SIZE		(DIVISION_D + 1)	// 奥行きの大きさ
#define VERTEX			(WIDTH_SIZE * DEPTH_SIZE)	// 頂点数
#define SKIN_SIZE		(500.0f)	// 大きさ
#define SKIN_SIZE_HALF	(SKIN_SIZE * 0.5f)	// 半分の大きさ
#define MAX_VERTEX		(10)	// 頂点の最大数

//=============================
// メッシュスキンクラス
//=============================
class CMeshSkinFlat : public CObject
{
public:
	// ボーン情報
	typedef struct
	{
		D3DXVECTOR3 pos;	// ボーンの位置
		D3DXVECTOR3 rot;	// ボーンの向き
	}BONE;

	CMeshSkinFlat(CObject::PRIORITY priority = CObject::PRIORITY_MESH);
	~CMeshSkinFlat();
	static CMeshSkinFlat* Create(D3DXVECTOR3 pos);
	static CMeshSkinFlat* Create(const char* pFilePath);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(D3DXVECTOR3 pos);
	HRESULT Init(const char* pFilePath);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetTextureIdx(int nIdx) { m_nIdxTexture = nIdx; }
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }

private:
	void SetVtxBuff(void);
	void SetDrawVtxBuff(void);
	bool LoadScripts(const char* pFilePath);
	bool LoadSkinInfo(const char* pFilePath);
	bool LoadBoneInfo(const char* pFilePath);
	bool LoadVertexInfo(const char* pFilePath);
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	D3DXMATRIX m_mtxWorld;				// ワールドマトリックス
	D3DXVECTOR3 m_pos;					// メッシュスキンの位置
	D3DXVECTOR3 m_rot;					// メッシュスキンの向き
	D3DXVECTOR3 m_nor;					// メッシュスキンの法線方向
	int m_nIdxTexture;		// テクスチャへのインデックス
	int m_nNumVtx;			// 頂点数
	int m_nNumBone;			// ボーン数
	float m_fLoadTexRange;	// テクスチャ読み込み幅
	BONE m_aBone[MAX_FLATBONE];	// ボーン情報を格納
	D3DXVECTOR3 m_aPosVtx[MAX_VERTEX];	// 頂点座標
	D3DXVECTOR3 m_aPosOffset[MAX_FLATBONE][MAX_VERTEX];	// 頂点座標
	float m_aWeight[MAX_FLATBONE][MAX_VERTEX];	// ボーンごとの影響力
	const char* m_pFileName;			// ファイル名
};

#endif
