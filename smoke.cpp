//==========================================
// 
// スモーク処理[smoke.cpp]
// Author : Kajino Hiyori _Create2026/08/24~
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "smoke.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//=============================
// マクロ定義
//=============================
#define EFFECT_SIZE		(20.0f)		// スモークのサイズ
#define FRAME			(3)			// フレーム
#define MAXANIM			(8)			// アニメーション総数
#define TEX_WIDTH		(1.0f / (float)MAXANIM)	// テクスチャ幅
#define TEX_HEIGHT		(1.0f)		// テクスチャ高さ
#define SIZE_RANDOM		((float)(rand() % 20) + 5.0f)	// サイズのランダム値

//=============================
// 静的メンバ変数宣言
//=============================
const char* CSmoke::m_pFilename =
{
	"data\\TEXTURE\\smoke000.png",	// テクスチャ000
};
int CSmoke::m_nNumAll = 0;

//=============================
// コンストラクタ
//=============================
CSmoke::CSmoke(CObject::PRIORITY priority) : CBillboardAnim(priority)
{
	// メンバ変数の値をクリア
	m_move = INIT_D3DVEC3;	// 移動量
	m_col = COLOR_WHITE;	// 色
	m_nNumAll++;			// スモークの総数を加算
}

//=============================
// デストラクタ
//=============================
CSmoke::~CSmoke()
{

}

//=============================
// スモーク生成処理
//=============================
CSmoke* CSmoke::Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fSize)
{
	CSmoke* pSmoke = NULL;

	if (pSmoke == NULL)
	{
		pSmoke = new CSmoke;	// インスタンス生成

		if (pSmoke != NULL)
		{ // インスタンス生成に成功
			// スモークの初期化処理
			pSmoke->Init(pos, move, col, fSize);

			// 移動量設定
			pSmoke->m_move = move;

			return pSmoke;
		}
	}

	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CSmoke::Init(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fSize)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// テクスチャの読み込み
	int nIdx = pTexture->Register(m_pFilename);

	// 2Dオブジェクトの初期化
	CBillboardAnim::Init(pos, fSize, fSize);
	CBillboardAnim::SetTextureIdx(nIdx);	// テクスチャ指定
	CBillboardAnim::SetColor(col);			// 色指定
	CBillboardAnim::SetAnimInfo(fSize, fSize, FRAME, MAXANIM, TEX_WIDTH, TEX_HEIGHT, false);
	// オブジェクトにタイプを指定
	CBillboardAnim::SetType(TYPE_EFFECT);

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CSmoke::Uninit(void)
{
	// 総数の減算
	m_nNumAll--;

	// billboardの終了処理
	CBillboardAnim::Uninit();
}

//=============================
// 更新処理
//=============================
void CSmoke::Update(void)
{
	// 位置更新
	D3DXVECTOR3 pos = CBillboardAnim::GetPosition();
	pos += m_move;
	CBillboardAnim::SetPosition(pos);

	// アニメーション更新
	CBillboardAnim::Update();
}

//=============================
// 描画処理
//=============================
void CSmoke::Draw(void)
{
	// billboardの描画処理
	CBillboardAnim::Draw();
}