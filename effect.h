//==========================================
// 
// エフェクト処理[effect.h]
// Author : Kajino Hiyori _Create2026/05/18~
// 
//==========================================
#ifndef _EFFECT_H_
#define _EFFECT_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "objectBillboard.h"

//=============================
// マクロ定義
//=============================
#define EFFECT_SPEED (10.0f)	// エフェクト速度

//=============================
// 前方宣言
//=============================
class CField;	// フィールドクラス

//=============================
// エフェクトクラス
//=============================
class CEffect : public CBillboard
{
public:
	// テクスチャの種類
	typedef enum
	{
		TYPE_000 = 0,	// テクスチャ000
		TYPE_001,		// テクスチャ001
		TYPE_002,		// テクスチャ002
		TYPE_MAX
	}TYPE;

	// エフェクトの状態
	typedef enum
	{
		STATE_DEFAULT = 0,	// 通常更新
		STATE_BASEHOMING,	// 基点追尾
		STATE_MAX
	}STATE;

	CEffect(CObject::PRIORITY priority = CObject::PRIOTITY_ADDALPHA);
	~CEffect();
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, TYPE type, D3DXCOLOR col, STATE state, float fSpeed);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) {}
	static CEffect* Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, TYPE type, int nLife, bool bGravity = false, STATE state = STATE_DEFAULT, float fSpeed = 0.0f);
	int GetLife(void) { return m_nLife; }
	static int GetNumAll(void) { return m_nNumAll; }
	void SetSize(float fSize) { CBillboard::SetWidth(fSize); CBillboard::SetHeight(fSize); }

private:
	void UpdateTutorial(void);
	void UpdateGame(void);
	void CollisionField(CField* pMeshField);
	void UpdateDefault(CField* pMeshField);
	static const char* m_apFilenameType[TYPE_MAX];		// テクスチャ名
	D3DXVECTOR3 m_move;	// 移動量
	D3DXVECTOR3 m_posDest;	// 目的地
	float m_fSpeed;		// エフェクトの速度
	int m_nLife;		// エフェクトの寿命
	STATE m_state;		// エフェクトの状態
	bool m_bGravity;	// 重力の有無
	static int m_nNumAll;	// エフェクトの総数
};

#endif