//========================================================
// 
// モーション管理処理[motion.h]
// Author : Kajino Hiyori _Create2026/06/08
//
//========================================================
#ifndef _MOTION_H_
#define _MOTION_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "model.h"

//=============================
// マクロ定義
//=============================
#define MAX_KEY		(16)	// キーの最大数
#define MAX_MOTION	(16)	// モーションの最大数

//=============================
// モーションクラス
//=============================
class CMotion 
{
public:
	// キーの要素
	typedef struct
	{
		float fPosX;	// 位置X
		float fPosY;	// 位置Y
		float fPosZ;	// 位置Z
		float fRotX;	// 向きX
		float fRotY;	// 向きY
		float fRotZ;	// 向きZ
	}MOTIONKEY;

	// キー情報の構造体
	typedef struct
	{
		int nFrame;				// 再生フレーム
		MOTIONKEY aKey[MAX_MODEL];	// 各パーツごとのキー要素
	}KEY_INFO;

	// モーション情報の構造体
	typedef struct
	{
		bool bLoop;					// ループするかどうか[true : ループなし / false : ループあり]
		int nNumKey;				// キーの総数
		KEY_INFO aKeyInfo[MAX_KEY];	// キー情報
	}INFO;

	CMotion();
	~CMotion();
	void SetInfo(INFO info);
	void SetModel(CModel** ppModel, int nNumModel);
	void Set(int nType, bool bBlendMotion, int nFrameBlend);
	void Set(int nType, int nFrameBlend);
	void Update(void);
	int GetType(void) { return m_nType; }
	int GetBlendType(void) { return m_nTypeBlend; }
	bool IsFinish(void) { return m_bFinish; }
	void SetFrame(int nType, int nFrame);
	void ChangeFrame(int nType, int nFrame);

private:
	void ChangeFrame(void);
	void NormalUpdate(void);
	void MoveUpdate(void);
	static float SetBlendPos(float fRate, float fNext, float fNow, float fRateBlend, float fNextBlend, float fNowBlend);
	static float SetBlendRot(float fRate, float fNext, float fNow, float fRateBlend, float fNextBlend, float fNowBlend);
	static float SetPos(float fRate, float fNext, float fNow);
	static float SetRot(float fRate, float fNext, float fNow);
	static float SetMovePos(float fRate, float fDest, float fNow);
	static float SetMoveRot(float fRate, float fNext, float fNow);
	INFO m_aInfo[MAX_MOTION];	// モーション情報
	int m_nNumAll;	// モーション総数
	CModel** m_ppModel;	// モーションさせたいモデルへのポインタ
	int m_nNumModel;	// モデル総数
	int m_nType;		// 現在のモーションの種類
	bool m_bLoop;		// 現在のモーションのループ管理
	int m_nNumKey;		// 現在のモーションのキー数
	int m_nKey;			// 現在のキー数
	int m_nCounter;		// モーションカウンター
	bool	m_bFinish;		// モーションの終了フラグ
	// 通常時のブレンド=====================================
	bool	m_bBlend;		// ブレンド状態の管理
	int		m_nTypeBlend;	// ブレンドモーションの種類
	bool	m_bLoopBlend;	// ブレンドモーションのループを管理
	int		m_nNumKeyBlend;	// ブレンドモーションのキーの総数
	int		m_nKeyBlend;	// ブレンドモーションの実行されているキーの番号
	int		m_nCounterMotionBlend;	// ブレンドモーションのカウンター
	int		m_nFrameBlend;			// ブレンドフレーム数(何フレームかけてブレンドをするか)
	int		m_nCounterBlend;		// ブレンドカウンター(ブレンドフレームが何フレーム処理されているか)
	// 移動時のブレンド=====================================
	bool	m_bMoveChanger;			// 移動時のモーション管理
	int		m_nTypeMove;			// 移動時のモーション種類
	bool	m_bLoopMove;			// 移動モーションのループを管理
	int		m_nNumKeyMove;			// 移動モーションのキーの総数
	int		m_nKeyMove;				// 移動モーションの実行されているキーの番号
	int		m_nCounterMotionMove;	// 移動モーションのカウンター
	int		m_nFrameMove;			// 移動フレーム数(何フレームかけてブレンドをするか)
	int		m_nCounterMove;			// 移動カウンター(ブレンドフレームが何フレーム処理されているか)
	// フレーム変更=========================================
	bool	m_bFrameChanger;		// フレーム切り替えを管理
	int		m_nFrameChange;			// フレーム切り替え目的値
	int		m_nFrameChangeType;		// フレーム切り替えモーション種類
};

#endif // !_MOTION_H_