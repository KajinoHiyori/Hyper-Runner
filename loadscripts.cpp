//==========================================
// 
// テキスト読み込み処理[loadscripts.h]
// Author : Kajino Hiyori _Create2026/10/06~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "loadscripts.h"

//=============================
// コンストラクタ
//=============================
CLoadScripts::CLoadScripts()
{

}

//=============================
// デストラクタ
//=============================
CLoadScripts::~CLoadScripts()
{

}

//=============================
// 有効文字連結処理
//=============================
void CLoadScripts::Judgment(char* aStr, char* cCopy)
{
	char aLocalCopy[MAX_STRING];	// ローカルで情報を格納
	memset(&aLocalCopy[0], 0, sizeof aLocalCopy);
	int nLength = 0;

	while (1)
	{
		if (*aStr == '#' || strcmp(aStr, "\0") == 0)
		{ // # or NULL文字が読み込まれる
			break;
		}
		else if (*aStr == 0x3000 || strcmp(aStr, "\n") == 0 || strcmp(aStr, "\t") == 0)
		{ // 全角スペース or 改行 or Tabスペースが読み込まれる
			aStr++;	// 文字列を進める
			continue;
		}
		else
		{
			aLocalCopy[nLength] = *aStr;
			nLength++;
			aStr++;
		}
	};

	// コピー先に文字列を設定
	strcpy(cCopy, &aLocalCopy[0]);
}

//=============================
// 有効文字の連結[tab空白削除]
//=============================
void CLoadScripts::DeleteTab(char* aStr, char* cCopy)
{
	char aLocalCopy[MAX_STRING];	// ローカルで情報を格納
	memset(&aLocalCopy[0], 0, sizeof aLocalCopy);
	memset(cCopy, 0, sizeof cCopy);
	int nLength = 0;

	while (1)
	{
		if (*aStr == '#' || strcmp(aStr, "\0") == 0)
		{ // # or NULL文字
			break;
		}
		else if (*aStr == 0x3000 || strcmp(aStr, "\n") == 0)
		{ // 全角スペース or 改行
			break;
		}
		else if (*aStr == '\t')
		{
			aStr++;
		}
		else
		{ //  Tabスペース
			aLocalCopy[nLength] = *aStr;
			nLength++;
			aStr++;
		}
	};

	// コピー先に文字列を設定
	strcpy(cCopy, &aLocalCopy[0]);
}

//=============================
// ファイル名読み込み処理
//=============================
void CLoadScripts::LoadFileName(char* aStrCopy)
{
	char aStringTab[MAX_STRING];	// ローカルで文字列を格納[コピー]
	memset(&aStringTab[0], 0, sizeof aStringTab);

	// ファイル名の獲得
	DeleteTab(aStrCopy, aStringTab);	// Tabを削除

	char* pLoad = NULL;
	if ((pLoad = strchr(aStringTab, '=')) == NULL)
	{
		// =の位置を獲得
	}

	// 正式に情報を読み取れた場合
	char aFileName[FILENAME_MAX];
	if (sscanf(pLoad + 1, "%s", &aFileName[0]) != EOF)
	{ // 受け取ったファイル名を文字列に変換
		const char* pFileName = &aFileName[0];
		int nLength = strlen(pFileName);
		for (int nChar = 0; nChar < nLength; nChar++, pFileName++)
		{ // ファイル名の格納
			aFileName[nChar] = *pFileName;
		}
	}

	memcpy(aStrCopy, &aFileName[0], sizeof aFileName);
}

//=============================
// D3DXVECTOR3型の読み込み
//=============================
D3DXVECTOR3 CLoadScripts::LoadVector3(char* aStrCopy)
{
	// 数値の獲得
	char* pLoad = NULL;
	if ((pLoad = strchr(aStrCopy, '=')) == NULL)
	{ // =の位置を獲得
	}

	// 正式に情報を読み取れた場合
	float floatX = 0.0f;
	float floatY = 0.0f;
	float floatZ = 0.0f;
	if (sscanf(pLoad + 1, "%f %f %f", &floatX, &floatY, &floatZ) != EOF)
	{ // =以降に保存されている数値を設定
		return D3DXVECTOR3(floatX, floatY, floatZ);
	}

	return INIT_D3DVEC3;
}

//=============================
// float型の読み込み
//=============================
float CLoadScripts::LoadFloat(char* aStrCopy)
{
	// 数値の獲得
	char* pLoad = NULL;
	if ((pLoad = strchr(aStrCopy, '=')) == NULL)
	{ // =の位置を獲得
	}

	// 正式に情報を読み取れた場合
	float fValue = 0.0f;
	if (sscanf(pLoad + 1, "%f", &fValue) != EOF)
	{ // =以降に保存されている数値を設定
		return fValue;
	}

	return fValue;
}

//=============================
// int型の読み込み
//=============================
int CLoadScripts::LoadInt(char* aStrCopy)
{
	// 数値の獲得
	char* pLoad = NULL;
	if ((pLoad = strchr(aStrCopy, '=')) == NULL)
	{ // =の位置を獲得
	}

	// 正式に情報を読み取れた場合
	int nValue = 0;
	if (sscanf(pLoad + 1, "%d", &nValue) != EOF)
	{ // =以降に保存されている数値を設定
		return nValue;
	}

	return nValue;
}