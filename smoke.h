//==========================================
// 
// スモーク処理[smoke.h]
// Author : Kajino Hiyori _Create2026/08/24~
// 
//==========================================
#ifndef _SMOKE_H_
#define _SMOKE_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "objectBillboardAnim.h"

//=============================
// 前方宣言
//=============================
class CMeshField;	// メッシュフィールドクラス

//=============================
// スモーククラス
//=============================
class CSmoke : public CBillboardAnim
{
public:
	CSmoke(CObject::PRIORITY priority = CObject::PRIORITY_SMOKE);
	~CSmoke();
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fSize);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) {}
	static CSmoke* Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fSize);
	static int GetNumAll(void) { return m_nNumAll; }

private:
	static const char* m_pFilename;		// テクスチャ名
	D3DXVECTOR3 m_move;	// 移動量
	D3DXCOLOR m_col;		// スモークの状態
	static int m_nNumAll;	// スモークの総数
};

#endif