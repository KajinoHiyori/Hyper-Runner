//==========================================
// 
// メイン処理[main.h]
// Author : Kajino Hiyori _Create2026/05/08~
// 
//==========================================
#ifndef _MAIN_H_
#define _MAIN_H_

//=============================
// インクルードファイル
//=============================
#include <winsock2.h>	// Winsock通信を行う際に必要
#include <mutex>		// 排他制御に必要
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <time.h>
#include "color.h"		// D3DXCOLOR型の色指定
#include "d3dx9.h"		// 描画処理に必要
#include "dinput.h"		// 入力処理に必要
#include "xaudio2.h"	// サウンド処理に必要
#include "xinput.h"		// ジョイパッド処理に必要

//=============================
// ライブラリのリンク
//=============================
#pragma comment(lib, "d3d9.lib")	// 描画処理に必要
#pragma comment(lib, "d3dx9.lib")	// [d3d9.lib]の拡張ライブラリ
#pragma comment(lib, "dxguid.lib")	// DirectXコンポーネント(部品)使用に必要
#pragma comment(lib, "winmm.lib")	// システム時刻の取得に必要
#pragma comment(lib, "dinput8.lib")	// 入力処理に必要
#pragma comment(lib, "xinput.lib")	// ジョイパッド処理に必要
#pragma comment (lib, "ws2_32.lib")	// winsock通信に必要

//=============================
// エラー対策
//=============================
#define DIRECTINPUT_VERSION	(0x0800)	// ビルド時の警告対処用マクロ

//=============================
// マクロ定義
//=============================
#define SCREEN_WIDTH					(1280)			// ウィンドウの幅
#define SCREEN_HEIGHT					(720)			// ウィンドウの高さ
										//  頂点座標2D	   頂点カラー    テクスチャ座標
#define FVF_VERTEX_2D					(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)	// 頂点フォーマット
#define FVF_VERTEX_3D					(D3DFVF_XYZ		|	D3DFVF_NORMAL		| D3DFVF_DIFFUSE	|	D3DFVF_TEX1)	// 頂点フォーマット[3D]
#define FVF_VERTEX_3D_MULTI				(D3DFVF_XYZ		|	D3DFVF_NORMAL		| D3DFVF_DIFFUSE	|	D3DFVF_TEX2)	// 頂点フォーマット[3D]
#define WINDOW_NAME						"Hyper Runner"	// ウィンドウの名前(キャプションに表示)
#define HALF_D3DXPI						(D3DX_PI * 0.5f)	// 円周率の約半分
#define DOUBLE_D3DXPI					(D3DX_PI * 2.0f)	// 円周率の約2倍
#define INIT_D3DVEC3					(D3DXVECTOR3(0.0f, 0.0f, 0.0f))	// D3DXVECTOR3型の初期値
#define ROTATE_D3DXVEC3					(D3DXVECTOR3(0.0f, HALF_D3DXPI, 0.0f))	// D3DXVECTOR3型の90°回転
#define SCREEN_CENTER					(D3DXVECTOR3(640.0f,360.0f, 0.0f))	// スクリーン中心位置
#define RAND_POS						(D3DXVECTOR3(static_cast<float>(rand() % SCREEN_WIDTH), static_cast<float>(rand() % SCREEN_HEIGHT), 0.0f))	// ランダムな座標位置[2D]
#define RAND_ROT						((rand() % (int)(D3DX_PI * 2.0f * 100.0f) - (int)(D3DX_PI * 100.0f)) * 0.01f)	// 角度ランダム値
#define CAMERATYPE_ORTHO				// 平衡投影
#undef	CAMERATYPE_ORTHO				// 平衡投影定義の無効化
#define FOV_DEFAULT						(45.0f)	// 基本の視野角
#define CAMERA_CLOSEST					(10.0f)		// カメラとの距離がこれ以上近い場合、描画を行わない
#define CAMERA_FARTHEST					(10000.0f)	// カメラとの距離がこれ以上遠い場合、描画を行わない
#define CORRECTION_MOVE					(0.1f)	// 移動の減衰係数
#define GRAVITY							(1.0f)			// 重力
#define SAFE_UNINIT(pointer)			if (pointer != NULL){pointer->Uninit(); delete pointer; pointer = NULL;}		// Uninit
#define SAFE_UNLOAD(pointer)			if (pointer != NULL){pointer->Unload(); delete pointer; pointer = NULL;}		// Unload
#define SAFE_RELEASE(pointer)			if (pointer != NULL){pointer->Release(); pointer = NULL;}						// Release
#define	SAFE_DELETE_SINGULAR(pointer)	if (pointer != NULL){delete pointer; pointer = NULL;}							// delete
#define	SAFE_DELETE_PLURAL(pointer)		if (pointer != NULL){delete[] pointer; pointer = NULL;}							// delete[]

//=============================
// 頂点情報[2D]の構造体を定義
//=============================
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点座標(x, y, z)
	float rhw;			// 座標変換用係数(1.0fで固定)
	D3DCOLOR col;		// 頂点カラー(r, g, b, a)
	D3DXVECTOR2 tex;	// テクスチャ座標(x, y)
}VERTEX_2D;

//=============================
// 頂点情報[3D]の構造体を定義
//=============================
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点座標(x, y, z)
	D3DXVECTOR3 nor;	// 法線ベクトル
	D3DCOLOR col;		// 頂点カラー(r, g, b, a)
	D3DXVECTOR2 tex;	// テクスチャ座標(x, y)
}VERTEX_3D;

//=============================
// 頂点情報[3D]の構造体を定義
//=============================
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点座標(x, y, z)
	D3DXVECTOR3 nor;	// 法線ベクトル
	D3DCOLOR col;		// 頂点カラー(r, g, b, a)
	D3DXVECTOR2 tex;	// テクスチャ座標(x, y)
	D3DXVECTOR2 texM;	// マルチテクスチャ座標(x, y)
}VERTEX_3D_MULTI;

//=============================
// プロトタイプ宣言
//=============================
float AngleNormalize(float fAngle);
bool CrossCollisionLeft(D3DXVECTOR3 pos, D3DXVECTOR3 posPoint, D3DXVECTOR3 vecLine);
bool CrossCollisionRight(D3DXVECTOR3 pos, D3DXVECTOR3 posPoint, D3DXVECTOR3 vecLine);
void DestroyApp(void);
#endif