//==========================================
// 
// テキスト読み込み処理[loadscripts.h]
// Author : Kajino Hiyori _Create2026/10/06~
// 
//==========================================
#ifndef _LOAD_SCRIPTS_H_
#define _LOAD_SCRIPTS_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"

//=============================
// マクロ定義
//=============================
#define MAX_STRING				(2048)					// ローカル文字列の最大数
#define TITLESCRIPTS_PATH		("data\\SCRIPTS\\titlescripts.txt")		// 外部ファイル名
#define TUTORIALSCRIPTS_PATH	("data\\SCRIPTS\\tutorialscripts.txt")	// 外部ファイル名
#define GAMESCRIPTS_PATH		("data\\SCRIPTS\\gamescripts.txt")		// 外部ファイル名
#define RESULTSCRIPTS_PATH		("data\\SCRIPTS\\resultscripts.txt")	// 外部ファイル名
#define RANKINGSCRIPTS_PATH		("data\\SCRIPTS\\rankingscripts.txt")	// 外部ファイル名
#define LOADMODELSCRIPTS_PATH	("data\\SCRIPTS\\loadmodel.txt")		// 外部ファイル名
#define NUM_MODEL			("NUM_MODEL")			// NUM_MODEL
#define MODEL_FILENAME		("MODEL_FILENAME")		// MODEL_FILENAME
#define CHARACTERSET		("CHARACTERSET")		// CHARACTERSET
#define END_CHARACTERSET	("END_CHARACTERSET")	// END_CHARACTERSET
#define NUM_PARTS			("NUM_PARTS")			// NUM_PARTS
#define PARTSSET			("PARTSSET")			// PARTSSET
#define END_PARTSSET		("END_PARTSSET")		// END_PARTSSET
#define INDEX				("INDEX")				// INDEX
#define PARENT				("PARENT")				// PARENT
#define POS					("POS")					// POS
#define ROT					("ROT")					// ROT
#define MOTIONSET			("MOTIONSET")			// MOTIONSET
#define END_MOTIONSET		("END_MOTIONSET")		// END_MOTIONSET
#define LOOP				("LOOP")				// LOOP
#define NUM_KEY				("NUM_KEY")				// NUM_KEY
#define KEYSET				("KEYSET")				// KEYSET
#define END_KEYSET			("END_KEYSET")			// END_KEYSET
#define FRAME				("FRAME")				// FRAME
#define KEY					("KEY")					// KEY
#define END_KEY				("END_KEY")				// END_KEY
//=============================
// テキスト読み込みクラス
//=============================
class CLoadScripts
{
public:
	CLoadScripts();
	~CLoadScripts();
	void Judgment(char* aStr, char* cCopy);
	void LoadFileName(char* aStrCopy);
	D3DXVECTOR3 LoadVector3(char* aStrCopy);
	float LoadFloat(char* aStrCopy);
	int LoadInt(char* aStrCopy);

private:
	void DeleteTab(char* aStr, char* cCopy);
};

#endif // !_LOAD_SCRIPTS_H_

