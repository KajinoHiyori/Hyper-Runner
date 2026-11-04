//===========================================================================================
//
// サウンド処理 [sound.h]
// Author : Kajino Hiyori
//
//===========================================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"

//=============================
// サウンドクラス
//=============================
class CSound
{
public:
	// サウンドの種類
	typedef enum
	{
		BGM_TITLE = 0,		// TITLE
		BGM_TUTORIAL,	// TUTORIAL
		BGM_GAME,		// GAME
		BGM_RESULT,		// GAMECLEAR
		BGM_RANKING,	// RANKING
		SE_MACHINEMOVE,	// 機械移動
		SE_JUMP,		// ジャンプ
		SE_GROW,		// 成長
		SE_DIGG,		// 掘る
		SE_COUNTDOWN,	// カウントダウン
		SE_GO,			// GO
		SE_SELECT,			// 選択
		SE_ENTER,			// 決定
		SE_EAT00,			// コーン取得
		SE_EAT01,			// コーン取得
		SE_EAT02,			// コーン取得
		SE_EAT03,			// コーン取得
		SE_EAT04,			// コーン取得
		SE_EAT05,			// コーン取得
		SE_EAT06,			// コーン取得
		SE_EAT07,			// コーン取得
		SE_EAT08,			// コーン取得
		SE_EAT09,			// コーン取得
		SE_EAT10,			// コーン取得
		SE_EAT11,			// コーン取得
		SE_EAT12,			// コーン取得
		SE_EAT13,			// コーン取得
		SE_EAT14,			// コーン取得
		SE_EAT15,			// コーン取得
		SE_EAT16,			// コーン取得
		SE_EAT17,			// コーン取得
		SE_EAT18,			// コーン取得
		SE_EAT19,			// コーン取得
		SE_EAT20,			// コーン取得
		SOUND_LABEL_MAX,
	} SOUND_LABEL;

	// サウンド構造体
	typedef struct
	{
		const char* pFilename;	// ファイル名
		int nCntLoop;			// ループカウント
	} SOUNDINFO;

	CSound();
	~CSound();
	HRESULT Init(HWND hWnd);
	void Uninit(void);
	HRESULT Play(SOUND_LABEL label);
	void Stop(SOUND_LABEL label);
	void Stop(void);

private:
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);
	IXAudio2* m_pXAudio2 = NULL;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice* m_pMasteringVoice = NULL;			// マスターボイス
	IXAudio2SourceVoice* m_apSourceVoice[SOUND_LABEL_MAX] = {};	// ソースボイス
	BYTE* m_apDataAudio[SOUND_LABEL_MAX] = {};					// オーディオデータ
	DWORD m_aSizeAudio[SOUND_LABEL_MAX] = {};					// オーディオデータサイズ
	static SOUNDINFO m_aSoundInfo[SOUND_LABEL_MAX];					// サウンド構造体
};

//=============================
// マクロ定義
//=============================
#define NUM_CORNSOUND	((int)(CSound::SOUND_LABEL_MAX - CSound::SE_ENTER - 1))	// コーンの音数

#endif
