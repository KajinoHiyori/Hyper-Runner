//===============================
//
// サウンド処理 [sound.cpp]
// Author : Kajino Hiyori
//
//===============================
#include "sound.h"

//===============================
// サウンド情報
//===============================
CSound::SOUNDINFO CSound::m_aSoundInfo[SOUND_LABEL_MAX] =
{
	{"data\\SOUNDS\\BGM\\title.wav", -1},		// TITLE
	{"data\\SOUNDS\\BGM\\tutorial.wav", -1},	// TITLE
	{"data\\SOUNDS\\BGM\\game.wav",-1},			// GAME
	{"data\\SOUNDS\\BGM\\result.wav", -1},		// retult
	{"data\\SOUNDS\\BGM\\ranking.wav", -1},		// RANKING
	{"data\\SOUNDS\\SE\\machine.wav", 0},		// ufo_move
	{"data\\SOUNDS\\SE\\jump.wav", 0},			// jump
	{"data\\SOUNDS\\SE\\grow_up.wav", -1},		// 成長
	{"data\\SOUNDS\\SE\\digg.wav", -1},			// 掘る
	{"data\\SOUNDS\\SE\\countdown000.wav", 0},	// カウントダウン
	{"data\\SOUNDS\\SE\\countdown001.wav", 0},	// カウントダウン
	{"data\\SOUNDS\\SE\\select.wav", 0},		// select
	{"data\\SOUNDS\\SE\\enter.wav", 0},			// enter
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
	{"data\\SOUNDS\\SE\\eat.wav", 0},			// コーン取得
};

//===============================
// 静的メンバ変数宣言
//===============================

//===============================
// マクロ定義
//===============================
#define PLAYSOUND	// 音再生
//#undef	PLAYSOUND	// PLAYSOUNDを無効化

//===============================
// コンストラクタ
//===============================
CSound::CSound()
{
	m_pXAudio2 = NULL;						// XAudio2オブジェクトへのインターフェイス
	m_pMasteringVoice = NULL;				// マスターボイス
	m_apSourceVoice[0] = {};	// ソースボイス
	m_apDataAudio[0] = {};	// オーディオデータ
	m_aSizeAudio[0] = {};		// オーディオデータサイズ
}

//===============================
// デストラクタ
//===============================
CSound::~CSound()
{

}

//===============================
// 初期化処理
//===============================
HRESULT CSound::Init(HWND hWnd)
{
#ifdef PLAYSOUND
	HRESULT hr;

	// COMライブラリの初期化
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "COMライブラリの初期化に失敗！", "警告！", MB_ICONWARNING);

		return E_FAIL;
	}

	// XAudio2オブジェクトの作成
	hr = XAudio2Create(&m_pXAudio2, 0);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2オブジェクトの作成に失敗！", "警告！", MB_ICONWARNING);

		// COMライブラリの終了処理
		CoUninitialize();

		return E_FAIL;
	}

	// マスターボイスの生成
	hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "マスターボイスの生成に失敗！", "警告！", MB_ICONWARNING);

		// XAudio2オブジェクトの開放
		SAFE_RELEASE(m_pXAudio2)		

		// COMライブラリの終了処理
		CoUninitialize();

		return E_FAIL;
	}

	// サウンドデータの初期化
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// バッファのクリア
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// サウンドデータファイルの生成
		hFile = CreateFile(m_aSoundInfo[nCntSound].pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(1)", "警告！", MB_ICONWARNING);

			return HRESULT_FROM_WIN32(GetLastError());
		}

		if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{// ファイルポインタを先頭に移動
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(2)", "警告！", MB_ICONWARNING);

			return HRESULT_FROM_WIN32(GetLastError());
		}

		// WAVEファイルのチェック
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(1)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// チャンクデータの読み込み
		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(2)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		if (dwFiletype != 'EVAW')
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(3)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// フォーマットチェック
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(1)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// チャンクデータの読み込み
		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(2)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// データチェック
		hr = CheckChunk(hFile, 'atad', &m_aSizeAudio[nCntSound], &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(1)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// チャンクデータの読み込み
		m_apDataAudio[nCntSound] = (BYTE*)malloc(m_aSizeAudio[nCntSound]);
		hr = ReadChunkData(hFile, m_apDataAudio[nCntSound], m_aSizeAudio[nCntSound], dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(2)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// ソースボイスの生成
		hr = m_pXAudio2->CreateSourceVoice(&m_apSourceVoice[nCntSound], &(wfx.Format));
		if (FAILED(hr))
		{
			MessageBox(hWnd, "ソースボイスの生成に失敗！", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// バッファの値設定
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		buffer.AudioBytes = m_aSizeAudio[nCntSound];
		buffer.pAudioData = m_apDataAudio[nCntSound];
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount = m_aSoundInfo[nCntSound].nCntLoop;

		// オーディオバッファの登録
		m_apSourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);

		// ファイルをクローズ
		CloseHandle(hFile);
	}
#endif // PLAYSOUND
	return S_OK;
}

//===============================
// 終了処理
//===============================
void CSound::Uninit(void)
{
#ifdef PLAYSOUND
	// 一時停止
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (m_apSourceVoice[nCntSound] != NULL)
		{
			// 一時停止
			m_apSourceVoice[nCntSound]->Stop(0);

			// ソースボイスの破棄
			m_apSourceVoice[nCntSound]->DestroyVoice();
			m_apSourceVoice[nCntSound] = NULL;

			// オーディオデータの開放
			free(m_apDataAudio[nCntSound]);
			m_apDataAudio[nCntSound] = NULL;
		}
	}

	// マスターボイスの破棄
	m_pMasteringVoice->DestroyVoice();
	m_pMasteringVoice = NULL;

	// XAudio2オブジェクトの開放
	SAFE_RELEASE(m_pXAudio2);

	// COMライブラリの終了処理
	CoUninitialize();
#endif
}

//===============================
// セグメント再生(再生中なら停止)
//===============================
HRESULT CSound::Play(SOUND_LABEL label)
{
#ifdef PLAYSOUND
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_aSizeAudio[label];
	buffer.pAudioData = m_apDataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = m_aSoundInfo[label].nCntLoop;

	// 状態取得
	m_apSourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		m_apSourceVoice[label]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	m_apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	m_apSourceVoice[label]->Start(0);
#endif
	return S_OK;
}

//===============================
// セグメント停止(ラベル指定)
//===============================
void CSound::Stop(SOUND_LABEL label)
{
#ifdef PLAYSOUND
	XAUDIO2_VOICE_STATE xa2state;

	// 状態取得
	m_apSourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		m_apSourceVoice[label]->FlushSourceBuffers();
	}
#endif // !PLAYSOUND
}

//===============================
// セグメント停止(全て)
//===============================
void CSound::Stop(void)
{
#ifdef PLAYSOUND
	// 一時停止
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (m_apSourceVoice[nCntSound] != NULL)
		{
			// 一時停止
			m_apSourceVoice[nCntSound]->Stop(0);
		}
	}
#endif
}

//===============================
// チャンクのチェック
//===============================
HRESULT CSound::CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition)
{
#ifdef PLAYSOUND
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;

	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを先頭に移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクデータの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// ファイルタイプの読み込み
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// ファイルポインタをチャンクデータ分移動
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == format)
		{
			*pChunkSize = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if (dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}
#endif
	return S_OK;
}

//===============================
// チャンクデータの読み込み
//===============================
HRESULT CSound::ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
#ifdef PLAYSOUND
	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを指定位置まで移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// データの読み込み
		return HRESULT_FROM_WIN32(GetLastError());
	}
#endif
	return S_OK;
}
