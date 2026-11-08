//========================================================
// 
// Player処理[player.h]
// Author : Kajino Hiyori _Create2026/11/04
//
//========================================================
#ifndef _PLAYER_H_
#define	_PLAYER_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"
#include "motion.h"
#include "loadscripts.h"

//=============================
// マクロ定義
//=============================

//=============================
// 前方宣言
//=============================
class CModel;	// モデルクラス
class CShadow;	// 影クラス
class CField;	// フィールドクラス

//=============================
// プレイヤークラス
//=============================
class CPlayer : public CObject
{
public:
	// モデルの種類
	typedef enum
	{
		MODEL_00LOWERBACK,		// [00] 腰
		MODEL_01LEFTFOOT,		// [01] 左腿
		MODEL_02LEFTFOOT,		// [02] 左脛
		MODEL_03LEFTFOOT,		// [03] 左足
		MODEL_04RIGHTFOOT,		// [04] 右腿
		MODEL_05RIGHTFOOT,		// [05] 右脛
		MODEL_06RIGHTFOOT,		// [06] 右足
		MODEL_07BODY,			// [07] 胴体
		MODEL_08HEAD,			// [08] 顎
		MODEL_09HEAD,			// [09] 頭
		MODEL_10LEFTARM,		// [10] 左上腕
		MODEL_11LEFTARM,		// [11] 左下腕
		MODEL_12RIGHTARM,		// [12] 右上腕
		MODEL_13RIGHTARM,		// [13] 右下腕
		MODEL_MAX
	}MODEL;

	// モーションの種類
	typedef enum
	{
		MOTIONTYPE_NUTORAL = 0,	// 待機
		MOTIONTYPE_MOVE,		// 移動
		MOTIONTYPE_MAX
	}MOTIONTYPE;

	// 移動しているラインを選択
	typedef enum
	{
		LINE_LEFT = 0,	// 移動ライン[左]
		LINE_CENTER,	// 移動ライン[中央]
		LINE_RIGHT,		// 移動ライン[右]
		LINE_MAX
	}LINE;

	CPlayer(CObject::PRIORITY priority = CObject::PRIORITY_CHARACTOR);
	~CPlayer();
	static CPlayer* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	void SetRotationDest(D3DXVECTOR3 rot) { m_rotDest = rot; }
	void SetMove(D3DXVECTOR3 move) { m_move = move; }
	D3DXVECTOR3 GetMove(void) { return m_move; }
	MOTIONTYPE GetMotionType(void) { return static_cast<MOTIONTYPE>(m_pMotion->GetType()); }
	void SetMotion(MOTIONTYPE motionType, bool bBrend, int nFrame) { m_pMotion->Set(motionType, bBrend, nFrame); }
	D3DXMATRIX GetMatrix(void) { return m_mtxWorld; }

private:
	void UpdateTitle(void);
	void UpdateTutorial(void);
	void UpdateGame(void);
	void UpdateResult(void);
	void Move(void);
	void ChangeLine(int nChangeDir);
	void CollisionField(CField* pField);
	HRESULT LoadScripts(const char* pFilePath);
	bool LoadCharactor(const char* pFilePath);
	bool LoadModelFile(const char* pFilePath);
	bool LoadModelSetting(const char* pFilePath);
	bool LoadMotion(const char* pFilePath);
	D3DXVECTOR3	m_pos;		// モデルの位置
	D3DXVECTOR3	m_posOld;	// モデルの1フレーム前の位置
	D3DXVECTOR3	m_move;		// モデルの移動量
	D3DXVECTOR3	m_rot;		// モデルの向き
	D3DXVECTOR3	m_rotDest;	// モデルの目的の向き
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	CModel* m_apModel[MODEL_MAX];	// モデルのポインタ
	int m_nNumModel;		// モデル総数
	CMotion* m_pMotion;		// モーション情報
	CShadow* m_pShadow;		// 影情報
	char m_aFileName[MODEL_MAX][MAX_STRING];	// ローカルで文字列を格納[コピー]
	float m_fSpeed;	// 移動速度
	bool m_bJump;	// ジャンプ
	int m_nLine;	// 移動しているラインを取得
};

#endif