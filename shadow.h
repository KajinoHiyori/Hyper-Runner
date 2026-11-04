//========================================================
// 
// 影処理[shadow.h]
// Author : Kajino Hiyori _Create2026/07/09
//
//========================================================
#ifndef _SHADOW_H_
#define	_SHADOW_H_
//=============================
// インクルードファイル
//=============================
#include "object3D.h"

//=============================
// マクロ定義
//=============================

//=============================
// 前方宣言
//=============================
class CField;	// メッシュフィールドクラス
class CPlayer;	// プレイヤークラス

//=============================
// 影クラス
//=============================
class CShadow : public CObject3D
{
public:
	// 影の種類
	typedef enum
	{
		TYPE_CIRCLE = 0,	// 円形の影
		TYPE_SQUARE,		// 四角の影
		TYPE_MAX
	}TYPE;
	static CShadow* Create(TYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth);
	CShadow(CObject::PRIORITY priority = PRIOTITY_DISALPHA);
	~CShadow();
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(TYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos, D3DXVECTOR3 rot, CField* pField);
	static int GetNumAll(void) { return m_nNumAll; }

private:
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	TYPE m_type;			// 影の種類
	float m_fWidth;			// 大きさ[幅]
	float m_fDepth;			// 大きさ[奥行き]
	int m_nIdxTexture;		// テクスチャインデックス
	static const char* m_apFilename[TYPE_MAX];	// 共有テクスチャ
	static int m_nNumAll;	// 総数
};
#endif