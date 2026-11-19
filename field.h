//==========================================
// 
// フィールド処理[field.h]
// Author : Kajino Hiyori _Create2026/09/28~
// 
//==========================================
#ifndef _FIELD_H_
#define _FIELD_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"
#include "object3D.h"
#include "meshfield.h"

//=============================
// マクロ定義
//=============================
#define NUM_FIELDTEXTURE	(2)	// フィールド関連のテクスチャ数
#define ENABLE_BLOCKFIELD	// 地面をブロックポリゴンで描画
//#undef	ENABLE_BLOCKFIELD	// ENABLE_BLOCKFIELDを無効化

//=============================
// 前方宣言
//=============================
class CPolygonField;	// 地面ポリゴン
class CPolygon3D;		// 壁ポリゴン

#ifdef ENABLE_BLOCKFIELD
//=============================
// フィールドクラス
//=============================
class CField : public CObject
{
public:
	// フィールドの状態
	typedef enum
	{
		STATE_NONE = 0,	// 通常
		STATE_WARNING,	// 注意喚起
		STATE_BREALING,	// 破壊中
		STATE_DEGITAL,	// デジタルの床[当たり判定無]
		STATE_MAX
	}STATE;

	// 注意喚起UIの状態
	typedef enum
	{
		WARNING_ADD = 0,	// 不透明化
		WARNING_DIS,		// 透明化
		WARNING_MAX
	}WARNING;

	// 側面の種類
	typedef enum
	{
		SIDE_FRONT = 0,	// 前面
		SIDE_LEFT,		// 左面
		SIDE_BACK,		// 後面
		SIDE_RIGHT,		// 右面
		SIDE_MAX
	}SIDE;

	// 蓋面の種類
	typedef enum
	{
		LID_TOP = 0,	// 上面
		LID_BUTTON,		// 下面
		LID_MAX
	}LID;

	CField(CObject::PRIORITY priority = CObject::PRIORITY_FIELD);
	~CField();
	static CField* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; }
	float GetHeight(D3DXVECTOR3 pos);
	float GetWidth(void) { return m_fWidth; }
	float GetDepth(void) { return m_fDepth; }
	int GetIndex(void) { return m_nIdx; }
	bool CollisionField(D3DXVECTOR3 pos);
	void SetState(STATE state);
	STATE GetState(void) { return m_state; }
	static int GetNumAll(void) { return m_nNumAll; }
	static int GetIndex(D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }

private:
	void UpdateWarning(void);
	void UpdateBreaking(void);
	void UpdateDisScaling(void);
	void UpdateAddScaling(void);
	void SetResize(void);
	CPolygonField* m_apLid[LID_MAX];	// 上下面
	CPolygon3D* m_apSide[SIDE_MAX];	// 側面
	CPolygonField* m_pWarning;	// 注意用ポリゴン
	D3DXVECTOR3 m_pos;	// 自身の位置
	D3DXVECTOR3 m_rot;	// 自身の角度
	float m_fWidth;		// 幅
	float m_fDepth;		// 奥行
	float m_fHeight;	// 高さ
	float m_fWidthDef;	// 基準の幅
	float m_fDepthDef;	// 基準の奥行
	float m_fHeightDef;	// 基準の高さ
	STATE m_state;	// 現在の状態
	WARNING m_warning;	// 注意喚起管理
	float m_fColorAlpha;	// 色
	int m_nStateCounter;	// ステート管理
	int m_nIdx;	// インデックス
	bool m_bDegital;	// デジタル判定
	static int m_nNumAll;	// フィールドの総数
	static const char* m_apFilePath[NUM_FIELDTEXTURE];	// テクスチャパス
};

#else
//=============================
// フィールドクラス
//=============================
class CField : public CObject3D
{
public:
	// フィールドの状態
	typedef enum
	{
		STATE_NONE = 0,	// 通常
		STATE_WARNING,	// 注意喚起
		STATE_MAX
	}STATE;

	// 注意喚起UIの状態
	typedef enum
	{
		WARNING_ADD = 0,	// 不透明化
		WARNING_DIS,		// 透明化
		WARNING_MAX
	}WARNING;

	CField(CObject::PRIORITY priority = CObject::PRIORITY_FIELD);
	~CField();
	static CField* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth);
	HRESULT Init(void) { return E_FAIL; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { CObject3D::SetPosition(pos); }
	float GetHeight(D3DXVECTOR3 pos);
	float GetWidth(void) { return CObject3D::GetWidth(); }
	float GetDepth(void) { return CObject3D::GetDepth(); }
	void SetState(STATE state);
	STATE GetState(void) { return m_state; }
	static int GetNumAll(void) { return m_nNumAll; }
	static int GetIndex(D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosition(void) { return CObject3D::GetPosition(); }

private:
	void UpdateWarning(void);
	CPolygonField* m_pWarning;	// 注意用ポリゴン
	STATE m_state;	// 現在の状態
	WARNING m_warning;	// 注意喚起管理
	float m_fColorAlpha;	// 色
	int m_nStateCounter;	// ステート管理
	int m_nIdx;	// インデックス
	static int m_nNumAll;	// フィールドの総数
	static const char* m_apFilePath[NUM_FIELDTEXTURE];	// テクスチャパス
};
#endif
#endif