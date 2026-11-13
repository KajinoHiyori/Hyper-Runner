//========================================================
// 
// モーション管理処理[motion.cpp]
// Author : Kajino Hiyori _Create2026/06/08
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "motion.h"
#include "debugproc.h"
#include "manager.h"

//=============================
// コンストラクタ
//=============================
CMotion::CMotion()
{
	// 値のクリア
	memset(&m_aInfo[0], 0, sizeof m_aInfo);	// モーション情報
	m_nNumAll = 0;		// モーション総数
	m_ppModel = NULL;	// モーションさせたいモデルへのポインタ
	m_nNumModel = 0;	// モデル総数
	m_nType = 0;		// 現在のモーションの種類
	m_bLoop = false;	// 現在のモーションのループ管理
	m_nNumKey = 0;		// 現在のモーションのキー数
	m_nKey = 0;			// 現在のキー数
	m_nCounter = 0;		// モーションカウンター
	m_bFinish = false;	// モーションの終了フラグ
	m_bBlend = false;	// ブレンド状態の管理
	m_nTypeBlend = 0;			// ブレンドモーションの種類
	m_bLoopBlend = false;		// ブレンドモーションのループを管理
	m_nNumKeyBlend = 0;			// ブレンドモーションのキーの総数
	m_nKeyBlend = 0;			// ブレンドモーションの実行されているキーの番号
	m_nCounterMotionBlend = 0;	// ブレンドモーションのカウンター
	m_nFrameBlend = 0;			// ブレンドフレーム数(何フレームかけてブレンドをするか)
	m_nCounterBlend = 0;		// ブレンドカウンター(ブレンドフレームが何フレーム処理されているか)	
	m_bMoveChanger = false;		// 移動時のモーション管理
	m_nTypeMove = 0;			// 移動時のモーション種類
	m_bLoopMove = false;		// 移動モーションのループを管理
	m_nNumKeyMove = 0;			// 移動モーションのキーの総数
	m_nKeyMove = 0;				// 移動モーションの実行されているキーの番号
	m_nCounterMotionMove = 0;	// 移動モーションのカウンター
	m_nFrameMove = 0;			// 移動フレーム数(何フレームかけてブレンドをするか)
	m_nCounterMove = 0;			// 移動カウンター(ブレンドフレームが何フレーム処理されているか)
	m_bFrameChanger = false;	// フレーム切り替えを管理
	m_nFrameChange = 0;			// フレーム切り替え目的値
	m_nFrameChangeType = 0;		// フレーム切り替えモーション種類
}

//=============================
// デストラクタ
//=============================
CMotion::~CMotion()
{
	m_ppModel = NULL;
}

//=============================
// モーション情報の設定
//=============================
void CMotion::SetInfo(INFO info)
{
	m_aInfo[m_nNumAll] = info;	// モーション情報の設定
	m_nNumAll++;				// モーションの総数を加算
}

//=============================
// モデル情報の設定
//=============================
void CMotion::SetModel(CModel** ppModel, int nNumModel)
{
	m_ppModel = ppModel;		// 操作したいモデルの先頭アドレスを設定
	m_nNumModel = nNumModel;	// モデルの総数を設定
}

//=============================
// モーションの設定
//=============================
void CMotion::Set(int nType, bool bBlendMotion, int nFrameBlend)
{
	m_bFinish = false;
	m_bBlend = bBlendMotion;	// ブレンドの有無を管理
	if (m_bBlend == true)
	{
		m_nTypeBlend = nType;							// ブレンドモーションの種類
		m_nFrameBlend = nFrameBlend;					// ブレンドモーションのフレーム
		m_nNumKeyBlend = m_aInfo[m_nTypeBlend].nNumKey;	// ブレンドモーションのキー
		m_nKeyBlend = 0;								// ブレンドモーションのキーを初期化
		m_bLoopBlend = m_aInfo[m_nTypeBlend].bLoop;		// ブレンド先のモーションのループ状態
		m_nCounterMotionBlend = 0;						// ブレンドモーションカウンターのクリア
		m_nCounterBlend = 0;							// ブレンドカウンターのクリア
	}
	else
	{
		m_nType = nType;							// モーションの種類を設定
		m_nNumKey = m_aInfo[m_nType].nNumKey;		// モーションのキーを設定
		m_nNumKeyBlend = m_aInfo[m_nType].nNumKey;	// ブレンドモーションのキーを統一
		m_bLoop = m_aInfo[m_nType].bLoop;			// ループ管理
		m_nKey = 0;									// 現在のキーを初期化
		m_nCounter = 0;								// モーションカウンターのクリア
	}
}

//=============================
// モーションの設定
//=============================
void CMotion::Set(int nType, int nFrameBlend)
{
	m_nTypeMove = nType;
	m_bMoveChanger = true;
	m_bLoopMove = m_aInfo[m_nTypeMove].bLoop;			// 移動モーションのループを管理
	m_nNumKeyMove = m_aInfo[m_nTypeMove].nNumKey;			// 移動モーションのキーの総数
	m_nKeyMove = m_nKey;				// 移動モーションの実行されているキーの番号
	m_nCounterMotionMove = m_aInfo[m_nTypeMove].aKeyInfo[m_nKeyMove].nFrame;	// 移動モーションのカウンター
	m_nFrameMove = nFrameBlend;			// 移動フレーム数(何フレームかけてブレンドをするか)
	m_nCounterMove = m_nCounter;			// 移動カウンター(ブレンドフレームが何フレーム処理されているか)
}

//=============================
// モーションの更新処理
//=============================
void CMotion::Update(void)
{
	if (m_bMoveChanger == true)
	{ // 移動時の更新処理
		MoveUpdate();
	}
	else
	{ // 通常時のモーション更新
		NormalUpdate();
	}
}

//=============================
// モーションのフレーム設定
//=============================
void CMotion::SetFrame(int nType, int nFrame)
{
	m_bFrameChanger = true;
	m_nFrameChange = nFrame;
	m_nFrameChangeType = nType;
}

//=============================
// モーションのフレーム一括変更
//=============================
void CMotion::ChangeFrame(int nType, int nFrame)
{ 
	for (int nCntKey = 0; nCntKey < m_aInfo[nType].nNumKey; nCntKey++)
	{
		m_aInfo[nType].aKeyInfo[nCntKey].nFrame = nFrame;
	}
}

//=============================
// モーションのフレーム一括変更
//=============================
void CMotion::ChangeFrame(void)
{
	for (int nCntKey = 0; nCntKey < m_aInfo[m_nFrameChangeType].nNumKey; nCntKey++)
	{
		m_aInfo[m_nFrameChangeType].aKeyInfo[nCntKey].nFrame = m_nFrameChange;
	}
	m_bFrameChanger = false;
}

//=============================
// 通常時の更新処理
//=============================
void CMotion::NormalUpdate(void)
{
	// 全モデル(パーツ)ごとに位置、向きを更新
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		MOTIONKEY Key = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel];											// 現在のキー
		MOTIONKEY KeyNext = m_aInfo[m_nType].aKeyInfo[(m_nKey + 1) % m_nNumKey].aKey[nCntModel];						// 次のフレームのキー
		MOTIONKEY KeyBlend = m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].aKey[nCntModel];								// ブレンドモーションの現在のキー
		MOTIONKEY KeyNextBlend = m_aInfo[m_nTypeBlend].aKeyInfo[(m_nKeyBlend + 1) % m_nNumKeyBlend].aKey[nCntModel];	// ブレンドモーションの次のキー
		float fRateKey = (float)m_nCounter / (float)m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame;					// 現在のモーションカウンター / 現在の再生フレーム
		float fRateKeyBlend = (float)m_nCounterMotionBlend / (float)m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].nFrame;	// ブレンドモーションカウンター / ブレンドモーションフレーム
		float fRateBlend = (float)m_nCounterBlend / (float)m_nFrameBlend;	// ブレンドフレームを処理している比率
		float fPosX = 0.0f;	// 求める値[X座標の位置]
		float fPosY = 0.0f; // 求める値[Y座標の位置]
		float fPosZ = 0.0f; // 求める値[Z座標の位置]
		float fRotX = 0.0f; // 求める値[X座標の角度]
		float fRotY = 0.0f; // 求める値[Y座標の角度]
		float fRotZ = 0.0f; // 求める値[Z座標の角度]

		if (m_bBlend == true)	// ブレンド有の場合
		{
			// 位置の更新
			fPosX = SetBlendPos(fRateKey, KeyNext.fPosX, Key.fPosX, fRateBlend, KeyNextBlend.fPosX, KeyBlend.fPosX);
			fPosY = SetBlendPos(fRateKey, KeyNext.fPosY, Key.fPosY, fRateBlend, KeyNextBlend.fPosY, KeyBlend.fPosY);
			fPosZ = SetBlendPos(fRateKey, KeyNext.fPosZ, Key.fPosZ, fRateBlend, KeyNextBlend.fPosZ, KeyBlend.fPosZ);

			// 角度の更新
			fRotX = SetBlendRot(fRateKey, KeyNext.fRotX, Key.fRotX, fRateBlend, KeyNextBlend.fRotX, KeyBlend.fRotX);
			fRotY = SetBlendRot(fRateKey, KeyNext.fRotY, Key.fRotY, fRateBlend, KeyNextBlend.fRotY, KeyBlend.fRotY);
			fRotZ = SetBlendRot(fRateKey, KeyNext.fRotZ, Key.fRotZ, fRateBlend, KeyNextBlend.fRotZ, KeyBlend.fRotZ);
		}
		else // ブレンド無の場合
		{
			// 位置の更新
			fPosX = SetPos(fRateKey, KeyNext.fPosX, Key.fPosX);
			fPosY = SetPos(fRateKey, KeyNext.fPosY, Key.fPosY);
			fPosZ = SetPos(fRateKey, KeyNext.fPosZ, Key.fPosZ);

			// 角度の更新
			fRotX = SetRot(fRateKey, KeyNext.fRotX, Key.fRotX);
			fRotY = SetRot(fRateKey, KeyNext.fRotY, Key.fRotY);
			fRotZ = SetRot(fRateKey, KeyNext.fRotZ, Key.fRotZ);
		}

		// モデルの位置を更新
		D3DXVECTOR3 posOffset = m_ppModel[nCntModel]->GetPosOffset();
		m_ppModel[nCntModel]->SetPosition(D3DXVECTOR3(fPosX + posOffset.x, fPosY + posOffset.y, fPosZ + posOffset.z));

		// モデルの角度を更新
		D3DXVECTOR3 rotOffset = m_ppModel[nCntModel]->GetRotOffset();
		m_ppModel[nCntModel]->SetRotation(D3DXVECTOR3(fRotX + rotOffset.x, fRotY + rotOffset.y, fRotZ + rotOffset.z));
	}

	if (m_bBlend == true)
	{
		m_nCounterMotionBlend++;

		if (m_nCounterMotionBlend >= m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].nFrame)
		{
			m_nKeyBlend++;
			m_nCounterMotionBlend = 0;
			m_nKeyBlend = m_nKeyBlend % m_nNumKeyBlend;
			if (m_bFrameChanger == true)
			{
				ChangeFrame();
				m_bFrameChanger = false;
			}
		}

		m_nCounterBlend++;	// ブレンドカウンターを加算
		if (m_nCounterBlend >= m_nFrameBlend)
		{
			m_bBlend = false;
			m_bFinish = false;
			m_nCounterBlend = 0;
			m_nType = m_nTypeBlend;
			m_bLoop = m_aInfo[m_nType].bLoop;
			m_nNumKey = (int)m_aInfo[m_nType].nNumKey;
			m_nKey = m_nKeyBlend;
			m_nCounter = m_nCounterMotionBlend;
		}
	}
	else
	{	// ブレンド無
		m_nCounter++;	// ブレンドモーションカウンターを加算

		if (m_nCounter >= m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame)
		{
			m_nCounter = 0;
			m_nKey++;
			if (m_bFrameChanger == true)
			{
				ChangeFrame();
			}
			if (m_bLoop == false && m_nKey + 1 >= m_nNumKey)
			{
				m_bFinish = true;
			}
			m_nKey = m_nKey % m_nNumKey;
		}
	}
}

//=============================
// 移動時の更新処理
//=============================
void CMotion::MoveUpdate(void)
{
	// 全モデル(パーツ)ごとに位置、向きを更新
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
#if 1
		MOTIONKEY Key = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel];											// 現在のキー
		MOTIONKEY KeyNext = m_aInfo[m_nType].aKeyInfo[(m_nKey + 1) % m_nNumKey].aKey[nCntModel];				// 次のフレームのキー

		MOTIONKEY KeyMove = m_aInfo[m_nTypeMove].aKeyInfo[m_nKey].aKey[nCntModel];								// ブレンドモーションの現在のキー
		MOTIONKEY KeyNextMove = m_aInfo[m_nTypeMove].aKeyInfo[(m_nKey + 1) % m_nNumKeyBlend].aKey[nCntModel];	// ブレンドモーションの次のキー
		
		float fRateKey = (float)m_nCounter / (float)m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame;					// 現在のモーションカウンター / 現在の再生フレーム
		float fRateMoveKey = (float)m_nCounter / (float)m_nFrameMove;					// 現在のモーションカウンター / 現在の再生フレーム
		float fPosX = 0.0f;	// 求める値[X座標の位置]
		float fPosY = 0.0f; // 求める値[Y座標の位置]
		float fPosZ = 0.0f; // 求める値[Z座標の位置]
		float fRotX = 0.0f; // 求める値[X座標の角度]
		float fRotY = 0.0f; // 求める値[Y座標の角度]
		float fRotZ = 0.0f; // 求める値[Z座標の角度]

		// 位置の更新
		fPosX = SetBlendPos(fRateKey, KeyNext.fPosX, Key.fPosX, fRateMoveKey, KeyNextMove.fPosX, KeyMove.fPosX);
		fPosY = SetBlendPos(fRateKey, KeyNext.fPosY, Key.fPosY, fRateMoveKey, KeyNextMove.fPosY, KeyMove.fPosY);
		fPosZ = SetBlendPos(fRateKey, KeyNext.fPosZ, Key.fPosZ, fRateMoveKey, KeyNextMove.fPosZ, KeyMove.fPosZ);

		// 角度の更新
		fRotX = SetBlendRot(fRateKey, KeyNext.fRotX, Key.fRotX, fRateMoveKey, KeyNextMove.fRotX, KeyMove.fRotX);
		fRotY = SetBlendRot(fRateKey, KeyNext.fRotY, Key.fRotY, fRateMoveKey, KeyNextMove.fRotY, KeyMove.fRotY);
		fRotZ = SetBlendRot(fRateKey, KeyNext.fRotZ, Key.fRotZ, fRateMoveKey, KeyNextMove.fRotZ, KeyMove.fRotZ);

		// モデルの位置を更新
		D3DXVECTOR3 posOffset = m_ppModel[nCntModel]->GetPosOffset();
		m_ppModel[nCntModel]->SetPosition(D3DXVECTOR3(fPosX + posOffset.x, fPosY + posOffset.y, fPosZ + posOffset.z));

		// モデルの角度を更新
		D3DXVECTOR3 rotOffset = m_ppModel[nCntModel]->GetRotOffset();
		m_ppModel[nCntModel]->SetRotation(D3DXVECTOR3(fRotX + rotOffset.x, fRotY + rotOffset.y, fRotZ + rotOffset.z));
#elif 0
		MOTIONKEY Key = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel];											// 現在のキー
		MOTIONKEY KeyNext = m_aInfo[m_nTypeMove].aKeyInfo[(m_nKey + 1) % m_nNumKey].aKey[nCntModel];				// 次のフレームのキー

		float fRateKey = (float)m_nCounter / (float)m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame;					// 現在のモーションカウンター / 現在の再生フレーム
		float fPosX = 0.0f;	// 求める値[X座標の位置]
		float fPosY = 0.0f; // 求める値[Y座標の位置]
		float fPosZ = 0.0f; // 求める値[Z座標の位置]
		float fRotX = 0.0f; // 求める値[X座標の角度]
		float fRotY = 0.0f; // 求める値[Y座標の角度]
		float fRotZ = 0.0f; // 求める値[Z座標の角度]

		// ここでの割合計算に失敗しており急に動いて見える===============================

		// 位置の更新
		fPosX = SetMovePos(fRateKey, KeyNext.fPosX, Key.fPosX);
		fPosY = SetMovePos(fRateKey, KeyNext.fPosY, Key.fPosY);
		fPosZ = SetMovePos(fRateKey, KeyNext.fPosZ, Key.fPosZ);

		// 角度の更新
		fRotX = SetRot(fRateKey, KeyNext.fRotX, Key.fRotX);
		fRotY = SetRot(fRateKey, KeyNext.fRotY, Key.fRotY);
		fRotZ = SetRot(fRateKey, KeyNext.fRotZ, Key.fRotZ);

		// モデルの位置を更新
		D3DXVECTOR3 posOffset = m_ppModel[nCntModel]->GetPosOffset();
		m_ppModel[nCntModel]->SetPosition(D3DXVECTOR3(fPosX + posOffset.x, fPosY + posOffset.y, fPosZ + posOffset.z));

		// モデルの角度を更新
		D3DXVECTOR3 rotOffset = m_ppModel[nCntModel]->GetRotOffset();
		m_ppModel[nCntModel]->SetRotation(D3DXVECTOR3(fRotX + rotOffset.x, fRotY + rotOffset.y, fRotZ + rotOffset.z));
#else
		D3DXVECTOR3 pos = m_ppModel[nCntModel]->GetPosition();											// 現在のキー
		D3DXVECTOR3 rot = m_ppModel[nCntModel]->GetRotation();											// 現在のキー
		MOTIONKEY KeyNext = m_aInfo[m_nTypeMove].aKeyInfo[(m_nKey + 1) % m_nNumKey].aKey[nCntModel];				// 次のフレームのキー

		float fRateKey = (float)m_nCounter / (float)m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame;					// 現在のモーションカウンター / 現在の再生フレーム
		float fPosX = 0.0f;	// 求める値[X座標の位置]
		float fPosY = 0.0f; // 求める値[Y座標の位置]
		float fPosZ = 0.0f; // 求める値[Z座標の位置]
		float fRotX = 0.0f; // 求める値[X座標の角度]
		float fRotY = 0.0f; // 求める値[Y座標の角度]
		float fRotZ = 0.0f; // 求める値[Z座標の角度]

		// ここでの割合計算に失敗しており急に動いて見える===============================
		// 位置の更新
		fPosX = SetMovePos(fRateKey, KeyNext.fPosX, pos.x);
		fPosY = SetMovePos(fRateKey, KeyNext.fPosY, pos.y);
		fPosZ = SetMovePos(fRateKey, KeyNext.fPosZ, pos.z);

		// 角度の更新
		fRotX = SetRot(fRateKey, KeyNext.fRotX, rot.x);
		fRotY = SetRot(fRateKey, KeyNext.fRotY, rot.y);
		fRotZ = SetRot(fRateKey, KeyNext.fRotZ, rot.z);

		// モデルの位置を更新
		D3DXVECTOR3 posOffset = m_ppModel[nCntModel]->GetPosOffset();
		m_ppModel[nCntModel]->SetPosition(D3DXVECTOR3(fPosX + posOffset.x, fPosY + posOffset.y, fPosZ + posOffset.z));

		// モデルの角度を更新
		D3DXVECTOR3 rotOffset = m_ppModel[nCntModel]->GetRotOffset();
		m_ppModel[nCntModel]->SetRotation(D3DXVECTOR3(fRotX + rotOffset.x, fRotY + rotOffset.y, fRotZ + rotOffset.z));
#endif
	}

#if 1
	m_nCounter++;	// ブレンドモーションカウンターを加算
	if (m_nCounter >= m_nFrameMove)
	{
		m_nCounter = 0;
		m_nKey++;
		if (m_bFrameChanger == true)
		{
			ChangeFrame();
		}
		m_bMoveChanger = false;
		m_nType = m_nTypeMove;
	}
	m_nKey = m_nKey % m_nNumKey;
#elif 0
	m_nCounter++;	// ブレンドモーションカウンターを加算
	if (m_nCounter >= m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame)
	{
		m_nCounter = 0;
		m_nKey++;
		if (m_bFrameChanger == true)
		{
			ChangeFrame();
		}
		m_bMoveChanger = false;
		m_nType = m_nTypeMove;
	}
	m_nKey = m_nKey % m_nNumKey;

	m_nCounterMove++;
	if (m_nCounterMove >= m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame)
	{
		m_nCounter = 0;
		if (m_bFrameChanger == true)
		{
			ChangeFrame();
		}
		m_bMoveChanger = false;
		m_nType = m_nTypeMove;
	}

#endif
}

//=============================
// ブレンドモーション位置更新
//=============================
float CMotion::SetBlendPos(float fRate, float fNext, float fNow, float fRateBlend, float fNextBlend, float fNowBlend)
{
	float fPos = 0.0f;
	float fDiffKey = fNext - fNow;
	float fPosCurrent = fNow + (fDiffKey * fRate);
	// ブレンドモーション情報
	float fDiffKeyBlend = fNextBlend - fNowBlend;
	float fPosBlend = fNowBlend + (fDiffKeyBlend * fRateBlend);
	// 差分を求め、位置を求める
	float fDiffBrend = fPosBlend - fPosCurrent;
	fPos = fPosCurrent + (fDiffBrend * fRateBlend);
	return fPos;
}

//=============================
// ブレンドモーション角度更新
//=============================
float CMotion::SetBlendRot(float fRate, float fNext, float fNow, float fRateBlend, float fNextBlend, float fNowBlend)
{
	float fRot = 0.0f;
	float fDiffKey = fNext - fNow;
	float fRotCurrent = fNow + (fDiffKey * fRate);
	// 角度補正
	fRotCurrent = AngleNormalize(fRotCurrent);
	// ブレンドモーション情報
	float fDiffKeyBlend = fNextBlend - fNowBlend;
	// 角度補正
	fDiffKeyBlend = AngleNormalize(fDiffKeyBlend);
	float fRotBlend = fNowBlend + (fDiffKeyBlend * fRateBlend);
	// 差分を求め、位置を求める
	float fDiffBrend = fRotBlend - fRotCurrent;
	fRot = fRotCurrent + (fDiffBrend * fRateBlend);

	return fRot;
}

//=============================
// モーション位置更新
//=============================
float CMotion::SetPos(float fRate, float fNext, float fNow)
{
	float fPos = 0.0f;
	float fDiffKey = fNext - fNow;
	fPos = fNow + fDiffKey * fRate;
	return fPos;
}

//=============================
// モーション角度更新
//=============================
float CMotion::SetRot(float fRate, float fNext, float fNow)
{
	float fRot = 0.0f;
	float fDiffKey = fNext - fNow;
	// 角度補正
	fDiffKey = AngleNormalize(fDiffKey);
	fRot = fNow + fDiffKey * fRate;
	return AngleNormalize(fRot);
}

//=============================
// 移動モーション位置更新
//=============================
float CMotion::SetMovePos(float fRate, float fDest, float fNow)
{
	float fPos = 0.0f;
	float fDiffKey = fDest - fNow;
	fPos = fNow + fDiffKey * fRate;
	return fPos;
}