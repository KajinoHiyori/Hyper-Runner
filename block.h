//========================================================
// 
// ブロック処理[block.h]
// Author : Kajino Hiyori _Create2026/06/02
//
//========================================================
#ifndef _BLOCK_H_
#define	_BLOCK_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "objectX.h"
#include "loadscripts.h"

//=============================
// マクロ定義
//=============================
#define MAX_BLOCK	(16)	// ブロックの種類の最大数

//=============================
// 前方宣言
//=============================
class CField;	// フィールドクラス

//=============================
// ブロッククラス
//=============================
class CBlock : public CObjectX
{
public:
	static CBlock* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int type);
	CBlock(CObject::PRIORITY priority = CObject::PRIORITY_EVENTS);
	~CBlock();
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int type);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool Collision(D3DXVECTOR3* pos, D3DXVECTOR3 posOld, float fRadius, float fMargin);
	bool CollisionHeight(D3DXVECTOR3 pos, D3DXVECTOR3 posOld, float fHeight, float fMargin, bool bCollision);
	bool SetCollision(D3DXVECTOR3* pPos, float fRadius);
	static int GetNumAll(void) { return m_nNumAll; }
	static void LoadInfo(void);
	static void LoadSetting(const char* pFileName);

private:
	void CollisionField(CField* pField);
	void UpdateGame(void);
	static int m_nNumAll;	// 総数
	int m_nType;		// 種類
	D3DXVECTOR3 m_move;	// 移動量
	float m_fWidth;		// 横幅
	float m_fDepth;		// 奥行き
	float m_fLength;	// 対角線の長さ
	float m_fAngle;		// 対角線の角度
	int m_nCntFrame;		// 凹凸管理カウンター
	bool m_bBump;			// 凸中
	bool m_bDent;			// 凹中
	D3DXVECTOR3 m_scl;	// 拡大率
	static char m_apModelName[MAX_BLOCK][MAX_STRING];	// モデル名
};

#endif