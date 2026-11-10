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
#define MOTIONTYPE_MAX	(16)	// モーション情報
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
		MODEL_00LOWERBACK,		// [00] 腰		# 下半身モーション
		MODEL_01LEFTFOOT,		// [01] 左腿
		MODEL_02LEFTFOOT,		// [02] 左脛
		MODEL_03LEFTFOOT,		// [03] 左足
		MODEL_04RIGHTFOOT,		// [04] 右腿
		MODEL_05RIGHTFOOT,		// [05] 右脛
		MODEL_06RIGHTFOOT,		// [06] 右足
		MODEL_07BODY,			// [07] 胴体		# 上半身モーション
		MODEL_08HEAD,			// [08] 顎
		MODEL_09HEAD,			// [09] 頭
		MODEL_10LEFTARM,		// [10] 左上腕
		MODEL_11LEFTARM,		// [11] 左下腕
		MODEL_12RIGHTARM,		// [12] 右上腕
		MODEL_13RIGHTARM,		// [13] 右下腕
		MODEL_MAX
	}MODEL;

	// モーションの分割数
	typedef enum
	{
		DIV_MOTION_UNDER = 0,	// 下半身モーション
		DIV_MOTION_UPPER,		// 上半身モーション
		DIV_MOTION_MAX
	}DIV_MOTION;

	// モーションの種類
	typedef enum
	{
		MOTIONUNDER_WALK = 0,		// 歩き
		MOTIONUNDER_RUN,			// 走る
		MOTIONUNDER_JUMP,			// ジャンプ
		MOTIONUNDER_MAX
	}MOTIONUNDER;

	// モーションの種類
	typedef enum
	{
		MOTIONUPPER_NUTORAL = 0,	// 待機
		MOTIONUPPER_MAX
	}MOTIONUPPER;

	// 移動しているラインを選択
	typedef enum
	{
		LINE_LEFT = 0,	// 移動ライン[左]
		LINE_CENTER,	// 移動ライン[中央]
		LINE_RIGHT,		// 移動ライン[右]
		LINE_MAX
	}LINE;

	// 速度管理
	typedef enum
	{
		SPEED_SLOWWALK = 1,		// 遅く歩く
		SPEED_FASTWALK,			// 早く歩く
		SPEED_SLOWRUN = 4,		// 遅く走る
		SPEED_FASTRUN = 7,		// 早く走る
		SPEED_FASTESTRUN = 10,	// 最速で走る
	}SPEED;

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
	MOTIONUNDER GetUnderMotionType(void) { return static_cast<MOTIONUNDER>(m_apMotion[DIV_MOTION_UNDER]->GetType()); }
	MOTIONUPPER GetUpperMotionType(void) { return static_cast<MOTIONUPPER>(m_apMotion[DIV_MOTION_UPPER]->GetType()); }
	D3DXMATRIX GetMatrix(void) { return m_mtxWorld; }
	float GetRadius(void) { return m_fRadius; }
	SPEED GetSpeed(void) { return m_speed; }
	void ChangeSpeed(bool bChangeUP);

private:
	void UpdateTitle(void);
	void UpdateTutorial(void);
	void UpdateGame(void);
	void UpdateResult(void);
	void Move(bool bMove);
	void ChangeLine(int nChangeDir);
	void CollisionField(CField* pField);
	bool CollisionBlock(void);
	void SpeedUp(void);
	void SpeedDown(void);
	void SpeedManager(void);
	HRESULT LoadScripts(void);
	bool LoadCharactor(void);
	bool LoadModelFile(void);
	bool LoadModelSetting(void);
	bool LoadMotion(void);
	bool LoadMotion(int nNumModel, int nModelStart, const char* pFileName, DIV_MOTION div, int nNumMotion);
	D3DXVECTOR3	m_pos;		// モデルの位置
	D3DXVECTOR3	m_posOld;	// モデルの1フレーム前の位置
	D3DXVECTOR3	m_move;		// モデルの移動量
	D3DXVECTOR3	m_rot;		// モデルの向き
	D3DXVECTOR3	m_rotDest;	// モデルの目的の向き
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	CModel* m_apModel[MODEL_MAX];	// モデルのポインタ
	int m_nNumModel;		// モデル総数
	CMotion* m_apMotion[DIV_MOTION_MAX];		// モーション情報
	CShadow* m_pShadow;		// 影情報
	char m_aFileName[MODEL_MAX][MAX_STRING];	// ローカルで文字列を格納[コピー]
	SPEED m_speed;	// 速度管理
	float m_fSpeed;	// 移動速度
	bool m_bJump;	// ジャンプ
	bool m_bJumpOld;	// ジャンプ
	float m_fRadius;	// 当たり判定半径
	int m_nLine;	// 移動しているラインを取得
};

#endif