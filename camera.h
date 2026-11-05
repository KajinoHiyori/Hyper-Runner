//========================================================
// 
// カメラ処理[camera.h]
// Author : Kajino Hiyori _Create2026/06/01
//
//========================================================
#ifndef _CAMERA_H_
#define _CAMERA_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "scene.h"

//=============================
// 前方宣言
//=============================

//=============================
// カメラクラス
//=============================
class CCamera 
{
public:
	CCamera();
	~CCamera();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void SetCamera(void);
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	void SetRotation(D3DXVECTOR3 rot) { m_rot = rot; }
	D3DXVECTOR3 GetPosV(void) { return m_posV; }
	D3DXVECTOR3 GetPosR(void) { return m_posR; }
	D3DXVECTOR3 GetVecU(void) { return m_vecU; }
	void SetSceneCamera(CScene::MODE mode, CScene* pScene);

private:
	void UpdateGame(void);
	void UpdateResult(void);
	void UpdateOperate(void);
	D3DXVECTOR3 m_posV;			// 視点
	D3DXVECTOR3 m_posR;			// 注視点
	D3DXVECTOR3 m_vecU;			// 上方向ベクトル
	D3DXVECTOR3 m_rot;			// 向き
	D3DXMATRIX m_mtxProjection;	// プロジェクションマトリックス
	D3DXMATRIX m_mtxView;		// ビューマトリックス
	D3DXVECTOR3 m_posVDest;		// 目的の視点
	D3DXVECTOR3 m_posRDest;		// 目的の注視点
	D3DXVECTOR3 m_rotDest;		// 目的の向き
	int m_nCntCameraRoll;		// カメラの回り込み間隔を設定
	bool m_bCameraType;			// カメラの操作方法を管理[trueで追従カメラ、falseで操作カメラ]
};

#endif