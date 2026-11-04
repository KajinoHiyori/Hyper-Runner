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

	// 全モデル(パーツ)ごとに位置、向きを更新
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		float fPosX = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosX;	// 位置X
		float fPosY = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosY;	// 位置Y
		float fPosZ = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosZ;	// 位置Z
		float fRotX = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotX;	// 向きX
		float fRotY = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotY;	// 向きY
		float fRotZ = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotZ;	// 向きZ

		// モデルの位置を更新
		D3DXVECTOR3 posOffset = m_ppModel[nCntModel]->GetPosOffset();
		m_ppModel[nCntModel]->SetPosition(D3DXVECTOR3(fPosX + posOffset.x, fPosY + posOffset.y, fPosZ + posOffset.z));

		// モデルの角度を更新
		D3DXVECTOR3 rotOffset = m_ppModel[nCntModel]->GetRotOffset();
		m_ppModel[nCntModel]->SetRotation(D3DXVECTOR3(fRotX + rotOffset.x, fRotY + rotOffset.y, fRotZ + rotOffset.z));
	}
}

//=============================
// モーションの更新処理
//=============================
void CMotion::Update(void)
{
	// 全モデル(パーツ)ごとに位置、向きを更新
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		KEY Key = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel];											// 現在のキー
		KEY KeyNext = m_aInfo[m_nType].aKeyInfo[(m_nKey + 1) % m_nNumKey].aKey[nCntModel];						// 次のフレームのキー
		KEY KeyBlend = m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].aKey[nCntModel];								// ブレンドモーションの現在のキー
		KEY KeyNextBlend = m_aInfo[m_nTypeBlend].aKeyInfo[(m_nKeyBlend + 1) % m_nNumKeyBlend].aKey[nCntModel];	// ブレンドモーションの次のキー
		float fRateKey = (float)m_nCounter / (float)m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame;					// 現在のモーションカウンター / 現在の再生フレーム
		float fRateKeyBlend = (float)m_nCounterMotionBlend / (float)m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].nFrame;	// ブレンドモーションカウンター / ブレンドモーションフレーム
		float fRateBlend = (float)m_nCounterBlend / (float)m_nFrameBlend;	// ブレンドフレームを処理している比率
		float fDiffKey = 0.0f;	// 現在のモーションの次のキーと現在のキーの差分
		float fDiffKeyBlend = 0.0f;	// ブレンドモーションの次のキーと現在のキーの差分
		float fDiffBrend = 0.0f;	// 現在のモーションとブレンドモーションとの差分
		float fPosXCurrent = 0.0f;	// 現在のX座標の位置を格納
		float fPosYCurrent = 0.0f;	// 現在のY座標の位置を格納
		float fPosZCurrent = 0.0f;	// 現在のZ座標の位置を格納
		float fRotXCurrent = 0.0f;	// 現在のX座標の角度を格納
		float fRotYCurrent = 0.0f;	// 現在のY座標の角度を格納
		float fRotZCurrent = 0.0f;	// 現在のZ座標の角度を格納
		float fPosXBlend = 0.0f;	// ブレンドモーションのX座標の位置を格納
		float fPosYBlend = 0.0f;	// ブレンドモーションのY座標の位置を格納
		float fPosZBlend = 0.0f;	// ブレンドモーションのZ座標の位置を格納
		float fRotXBlend = 0.0f;	// ブレンドモーションのX座標の角度を格納
		float fRotYBlend = 0.0f;	// ブレンドモーションのY座標の角度を格納
		float fRotZBlend = 0.0f;	// ブレンドモーションのZ座標の角度を格納
		float fPosX = 0.0f;	// 求める値[X座標の位置]
		float fPosY = 0.0f; // 求める値[Y座標の位置]
		float fPosZ = 0.0f; // 求める値[Z座標の位置]
		float fRotX = 0.0f; // 求める値[X座標の角度]
		float fRotY = 0.0f; // 求める値[Y座標の角度]
		float fRotZ = 0.0f; // 求める値[Z座標の角度]

		if (m_bBlend == true)	// ブレンド有の場合
		{
			// 位置の更新 [Integer division by zero]に対処すること
			// X
			// 現在のモーション情報
			fDiffKey = KeyNext.fPosX - Key.fPosX;
			fPosXCurrent = Key.fPosX + (fDiffKey * fRateKey);
			// ブレンドモーション情報
			fDiffKeyBlend = KeyNextBlend.fPosX - KeyBlend.fPosX;
			fPosXBlend = KeyBlend.fPosX + (fDiffKeyBlend * fRateKeyBlend);
			// 差分を求め、位置を求める
			fDiffBrend = fPosXBlend - fPosXCurrent;
			fPosX = fPosXCurrent + (fDiffBrend * fRateBlend);

			// Y
			// 現在のモーション情報
			fDiffKey = KeyNext.fPosY - Key.fPosY;
			fPosYCurrent = Key.fPosY + (fDiffKey * fRateKey);
			// ブレンドモーション情報
			fDiffKeyBlend = KeyNextBlend.fPosY - KeyBlend.fPosY;
			fPosYBlend = KeyBlend.fPosY + (fDiffKeyBlend * fRateKeyBlend);
			// 差分を求め、位置を求める
			fDiffBrend = fPosYBlend - fPosYCurrent;
			fPosY = fPosYCurrent + (fDiffBrend * fRateBlend);

			// Z
			// 現在のモーション情報
			fDiffKey = KeyNext.fPosZ - Key.fPosZ;
			fPosZCurrent = Key.fPosZ + (fDiffKey * fRateKey);
			// ブレンドモーション情報
			fDiffKeyBlend = KeyNextBlend.fPosZ - KeyBlend.fPosZ;
			fPosZBlend = KeyBlend.fPosZ + (fDiffKeyBlend * fRateKeyBlend);

			// 差分を求め、位置を求める
			fDiffBrend = fPosZBlend - fPosZCurrent;
			fPosZ = fPosZCurrent + (fDiffBrend * fRateBlend);

			// 角度の更新
			// X
			// 現在のモーション情報
			fDiffKey = KeyNext.fRotX - Key.fRotX;
			fRotXCurrent = Key.fRotX + (fDiffKey * fRateKey);
			// 角度補正
			fRotXCurrent = AngleNormalize(fRotXCurrent);
			// ブレンドモーション情報
			fDiffKeyBlend = KeyNextBlend.fRotX - KeyBlend.fRotX;
			// 角度補正
			fDiffKeyBlend = AngleNormalize(fDiffKeyBlend);
			fRotXBlend = KeyBlend.fRotX + (fDiffKeyBlend * fRateKeyBlend);
			// 差分を求め、位置を求める
			fDiffBrend = fRotXBlend - fRotXCurrent;
			fRotX = fRotXCurrent + (fDiffBrend * fRateBlend);

			// Y
			// 現在のモーション情報
			fDiffKey = KeyNext.fRotY - Key.fRotY;
			fRotYCurrent = Key.fRotY + (fDiffKey * fRateKey);
			// 角度補正
			fRotYCurrent = AngleNormalize(fRotYCurrent);
			// ブレンドモーション情報
			fDiffKeyBlend = KeyNextBlend.fRotY - KeyBlend.fRotY;
			// 角度補正
			fDiffKeyBlend = AngleNormalize(fDiffKeyBlend);
			fRotYBlend = KeyBlend.fRotY + (fDiffKeyBlend * fRateKeyBlend);
			// 差分を求め、位置を求める
			fDiffBrend = fRotYBlend - fRotYCurrent;
			fRotY = fRotYCurrent + (fDiffBrend * fRateBlend);

			// Z
			// 現在のモーション情報
			fDiffKey = KeyNext.fRotZ - Key.fRotZ;
			fRotZCurrent = Key.fRotZ + (fDiffKey * fRateKey);
			// 角度補正
			fRotZCurrent = AngleNormalize(fRotZCurrent);
			// ブレンドモーション情報
			fDiffKeyBlend = KeyNextBlend.fRotZ - KeyBlend.fRotZ;
			// 角度補正
			fDiffKeyBlend = AngleNormalize(fDiffKeyBlend);
			fRotZBlend = KeyBlend.fRotZ + (fDiffKeyBlend * fRateKeyBlend);
			// 差分を求め、位置を求める
			fDiffBrend = fRotZBlend - fRotZCurrent;
			fRotZ = fRotZCurrent + (fDiffBrend * fRateBlend);

		}
		else // ブレンド無の場合
		{
			// 位置の更新 [Integer division by zero]に対処すること
			// X
			fDiffKey = KeyNext.fPosX - Key.fPosX;
			fPosX = Key.fPosX + fDiffKey * fRateKey;

			// Y
			fDiffKey = KeyNext.fPosY - Key.fPosY;
			fPosY = Key.fPosY + fDiffKey * fRateKey;

			// Z
			fDiffKey = KeyNext.fPosZ - Key.fPosZ;
			fPosZ = Key.fPosZ + fDiffKey * fRateKey;

			// 角度の更新
			// X
			fDiffKey = KeyNext.fRotX - Key.fRotX;
			// 角度補正
			fDiffKey = AngleNormalize(fDiffKey);
			fRotX = Key.fRotX + fDiffKey * fRateKey;

			// Y
			fDiffKey = KeyNext.fRotY - Key.fRotY;
			// 角度補正
			fDiffKey = AngleNormalize(fDiffKey);
			fRotY = Key.fRotY + fDiffKey * fRateKey;

			// Z
			fDiffKey = KeyNext.fRotZ - Key.fRotZ;
			// 角度補正
			fDiffKey = AngleNormalize(fDiffKey);
			fRotZ = Key.fRotZ + fDiffKey * fRateKey;
		}

		// 角度補正
		fRotX = AngleNormalize(fRotX);
		fRotY = AngleNormalize(fRotY);
		fRotZ = AngleNormalize(fRotZ);

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
			if (m_bLoop == false && m_nKey + 1 >= m_nNumKey)
			{
				m_bFinish = true;
			}
			m_nKey = m_nKey % m_nNumKey;
		}
	}
}
