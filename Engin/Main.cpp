//--------------------------------------------------------------------------------------
// File: Main.cpp
//
// メイン
//
// 備考：このプログラムはDirectX11を使用するための最低必要と思われる処理を組み込んだサンプルです
//       余分な処理は記述していませんのでオリジナルフレームワークのベースに使用してください
//
// Date: 2015.8.27
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#include <Windows.h>
#include <DirectXColors.h>
#include <d3d11.h>
#include "Direct3D.h"
#include "DirectXTK.h"
#include "ImaseLib\FPSTimer.h"

#include <SimpleMath.h>

#include "Grid.h"
#include "DebugCamera.h"

#include<Model.h>
#include"Camera.h"
#include"Obj3D.h"
#include"Player.h"
#include"Enmey.h"
#include"Collision.h"
#include"CollisionNode.h"
#include"LandShapeData.h"
#include"LandShape.h"
#include<vector>

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DirectX::SimpleMath;

//////////////////
// 定数宣言		//
//////////////////
// ウインドウスタイル
static const DWORD WINDOW_STYLE = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

// 画面解像度
static const int WINDOW_W = 640;
static const int WINDOW_H = 480;

//////////////////////////////
// 関数のプロトタイプ宣言	//
//////////////////////////////
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////
// グローバル変数	//
//////////////////////
HINSTANCE g_hInst = nullptr;	// インスタンスハンドル
HWND g_hWnd = nullptr;	// ウインドウハンドル

//プリミティブバッチ
std::unique_ptr<PrimitiveBatch<VertexPositionNormal>>g_pPrimitiveBatch;

//ポリゴン表示　エフェクト
std::unique_ptr<DirectX::BasicEffect>g_pPolygonEffect;

//地形データのポインタ配列
std::vector<LandShape*>	landShapeArray;

//ポリゴン表示　入力レイアウト
ComPtr<ID3D11InputLayout> g_pPolygonInputLayout;


//範囲指定乱数
float RandomRange(float min, float max)
{
	return static_cast<float>(rand()) / RAND_MAX*(max - min) + min;
}


//頂点インデックス
uint16_t indices[] =
{
	//0,1,2,1,3,2
	0,1,2,/*0,*/
	1,3,2
};

//頂点座標
VertexPositionNormal vertices[] =
{//
	{ Vector3(-1.0f,+1.0f,0.0f),Vector3(0.0f,0.0f,+1.0f) }, //0
	{ Vector3(-1.0f,-1.0f,0.0f),Vector3(0.0f,0.0f,+1.0f) }, //1
	{ Vector3(+1.0f,+1.0f,0.0f),Vector3(0.0f,0.0f,+1.0f) }, //2
	{ Vector3(+1.0f,-1.0f,0.0f),Vector3(0.0f,0.0f,+1.0f) }, //3

};

//ポリゴン初期化
void PolygonInit()
{

	//プリミティブバッチを作成
	g_pPrimitiveBatch.reset(new PrimitiveBatch<VertexPositionNormal>(g_pImmediateContext.Get()));
	//ポリゴン描画用のエフェクトを作成
	g_pPolygonEffect.reset(new BasicEffect(g_pd3dDevice.Get()));
	//ライド計算を有効化
	g_pPolygonEffect->SetLightingEnabled(true);
	//環境光の色を設定
	g_pPolygonEffect->SetAmbientLightColor(Vector3(0.2f, 0.2f, 0.2f));
	//拡散反射光の素材色を設定
	g_pPolygonEffect->SetDiffuseColor(Vector3(1.0f, 1.0f, 1.0f));
	//ライト0番を有効化
	g_pPolygonEffect->SetLightEnabled(0, true);
	//ライト0番の色を設定
	g_pPolygonEffect->SetLightDiffuseColor(0, Vector3(0.2f, 1.0f, 0.2f));
	//ライト0番の向きを設定
	g_pPolygonEffect->SetLightDirection(0, Vector3(1.0f, -0.5f, 2.0f));
	//ライト1番を有効化
	g_pPolygonEffect->SetLightEnabled(1, true);
	//ライト1番の色を設定
	g_pPolygonEffect->SetLightDiffuseColor(1, Vector3(0.5f, 0.2f, 0.3f));
	//ライト番の向きを設定
	g_pPolygonEffect->SetLightDirection(1, Vector3(-1.0f, -0.5f, -2.0f));

	void const* shaderByteCode;
	size_t byteCodeLength;
	//シェーダーの取得
	g_pPolygonEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	//入力レイアウトの作成
	g_pd3dDevice.Get()->CreateInputLayout(VertexPositionNormal::InputElements,
		VertexPositionNormal::InputElementCount, shaderByteCode, byteCodeLength,
		g_pPolygonInputLayout.GetAddressOf());

}

//ポリゴン開放
void PolygonTerm()
{
	//入力レイアウトの開放
	g_pPolygonInputLayout.Reset();
	//エフェクトの開放
	g_pPolygonEffect.reset();
	//プリミティブバッチの解放
	g_pPrimitiveBatch.reset();
}

//ポリゴン描画
void PolygonDraw(Matrix view, Matrix proj)
{
	static float angle = 0;
	angle += 6.0f;
	//ワールド行列
	Matrix world;

	//スケーリング行列
	Matrix scalemat = Matrix::CreateScale(0.5f, 1.0f, 100.0f);

	//回転、Z軸回りlラジアン→X軸回りｍラジアン→Y軸回りｎラジアン
	Matrix rotmatZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(angle));
	Matrix rotmatX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(angle));
	Matrix rotmatY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(angle));
	Matrix rotmatY2 = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(-angle));
	//平行移動(l,m,n)X方向にｌメートルY方向にmメートルZ方向にnメートル移動
	Vector3 pos(1.0f,0,0);

	Matrix transmat = SimpleMath::Matrix::CreateTranslation(sinf(angle),0.0f, cosf(angle));

	//ワールド行列に反映
	world = world*2.5f*rotmatY*transmat*rotmatY2;
	
	//ワールド行列を設定
	g_pPolygonEffect->SetWorld(world);
	//ビュー行列を設定
	g_pPolygonEffect->SetView(view);
	//プロジェクション行列を設定
	g_pPolygonEffect->SetProjection(proj);
	//エフェクトの設定（各行列やテクスチャなどを反映）
	g_pPolygonEffect->Apply(g_pImmediateContext.Get());

	//深度ステンシルステートを設定
	g_pImmediateContext->OMSetDepthStencilState(g_state->DepthDefault(), 0);
	//ブレンドステートを設定
	g_pImmediateContext->OMSetBlendState(g_state->NonPremultiplied(), nullptr, 0xFFFFFFF);

	//g_pImmediateContext->RSSetState(g_state->CullClockwise());
	//ラスタライザステートを設定（時計回りを非表示）
	//g_pImmediateContext->RSSetState(g_state->Wireframe());
	g_pImmediateContext->RSSetState(g_state->CullClockwise());
	//入力レイアウトを設定
	g_pImmediateContext->IASetInputLayout(g_pPolygonInputLayout.Get());

	//描画開始
	g_pPrimitiveBatch->Begin();
	//頂点座標を渡して描画
	g_pPrimitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices, 6, vertices, 4);
	//g_pPrimitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, indices, 7, vertices, 4);
	//描画終了
	g_pPrimitiveBatch->End();
}

void ModelDraw(Matrix view, Matrix proj)
{
	EffectFactory*factory = new EffectFactory(g_pd3dDevice.Get());
	//テクスチャの読み込みパスを指定
	factory->SetDirectory(L"Resources/cModels");
	std::unique_ptr<Model>model = Model::CreateFromCMO(g_pd3dDevice.Get(), L"Resources/cModels/textbox.cmo", *factory);

}


//--------------------------------------------------------------------------------------
// メイン
//--------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	//カメラ
	Camera camera;

	// 1秒間に60フレームで固定する
	ImaseLib::FPSTimer fpsTimer(60);

	// ウインドウの作成
	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;

	// DirectXデバイスの初期化
	if (FAILED(Direct3D_InitDevice(g_hWnd)))
	{
		// 初期化に失敗
		Direct3D_CleanupDevice();
		
		return 0;
	}

	// DirectXTK関係の初期化
	DirectXTK_Initialize();

	// グリッド床オブジェクト
	GridFloor gridFloor(20.0f, 20);

	PolygonInit();
	EffectFactory* factory = new EffectFactory(g_pd3dDevice.Get());

	//テクスチャの読み込みパスを指定
	factory->SetDirectory(L"Resources/cModels");


	//Obj3Dクラスの全体の初期化
	{
		//デバイスの設定
		Obj3D::SetDevice(g_pd3dDevice.Get());
		//デバイスコンテキストの設定
		Obj3D::SetDeviceContext(g_pImmediateContext.Get());
		//描画ステートの設定
		Obj3D::SetStates(g_state.get());
		////エフェクトファクトリの設定
		Obj3D::SetEffectFactory(factory);
		////カメラの設定
		Obj3D::SetCamerra(&camera);
	}

	//地形配置情報設定用　構造体
	struct LandShapeTable
	{
		Vector3 trans;
		Vector3 rot;
		float scale;
		wchar_t* mdl_name;
		wchar_t* cmo_name;
	};

	//地形配置データテーブル（配列の要素数は指定しない）
	static LandShapeTable s_landShapeTable[]=
	{
		//座標				　回転				縮尺	地形ファイル名　表示用モデルファイル名
		//{ Vector3(-4,0.002, 4),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(-4,0.002, 0),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo"},
		//{ Vector3(-4,0.002, -4), Vector3(0, 0, 0), 4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(-4,0.002, -8), Vector3(0, 0, 0), 4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(-4,0.002, -12), Vector3(0, 0, 0), 4.0f,L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(-4,0.002, -16), Vector3(0, 0, 0), 4.0f,L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(-4,0.002, -20), Vector3(0, 0, 0), 4.0f,L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(-4,0.002, -24), Vector3(0, 0, 0), 4.0f,L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(0,0.002, -24), Vector3(0, 0, 0), 4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(0,0.002, 8),Vector3(0, 0, 0),   4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//
		//{ Vector3(4,0.002, 8),Vector3(0, 0, 0),   4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(8,0.002, 8),Vector3(0, 0, 0),   4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(12,0.002, 8),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(16,0.002, 8),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(20,0.002, 8),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(24,0.002, 8),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(28,0.002, 8),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(32,0.002, 8),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },


		//{ Vector3(4,0.002, 4),Vector3(0, 0, 0),  4.0f,   L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(4,0.002, 0), Vector3(0, 0, 0),  4.0f,   L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(4,0.002, -4), Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(4,0.002, -8), Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(4,0.002, -12), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(4,0.002, -20), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(4,0.002, -24), Vector3(0, 0, 0), 4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//
		//{ Vector3(8,0.002, 4), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(8,0.002, -28), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(12,0.002, -28), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(16,0.002, -28), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(16,0.002, -24), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{Vector3(8,0.002, -12), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(8,0.002, -20), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(12,0.002, -12), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },	

		//{ Vector3(16,0.002, -12), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(16,0.002, -20), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(20,0.002, -20), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(24,0.002, 0), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(24,0.002, -4), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(24,0.002, -8), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(24,0.002, -12), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(24,0.002, -20), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(28,0.002, 0), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(28,0.002, -20), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(32,0.002, 0), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(32,0.002, -4), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(32,0.002, -8), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(32,0.002, -12), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(32,0.002, -16), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(32,0.002, -20), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(16,0.002, -4), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(12,0.002, -4), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(16,0.002, 0), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(20,0.002, 0), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(24,0.002, 0), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(28,0.002, 0), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//
		////{ Vector3(32,0.002, 4), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/goal.MDL", L"Resources/cModels/goal.cmo" },

		//床
		{ Vector3(0,0.002,-0.1),Vector3(0, 0, 0),  100.0f, L"Resources/LandShape/yuka.MDL", L"Resources/cModels/yuka.cmo" },
	};

	//配列の要素数を計算
	int landshape_table_num = sizeof(s_landShapeTable) / sizeof(LandShapeTable);
	////vectorの要素数を変更
	//landShapeArray.resize(landshape_table_num);
	//テーブルの1要素ずつ全部処理する
	for (int i = 0; i < landshape_table_num; i++)
	{
		LandShapeTable* ptable = &s_landShapeTable[i];
		//地形データを生成
		LandShape* landShape = new LandShape;
		//ファイル名が空白の場合はモデルの読み込みを飛ばす設定
		wchar_t* mdl_name = nullptr;
		wchar_t* cmo_name = nullptr;
		if (lstrcmpW(ptable->mdl_name, L"") != 0)mdl_name = ptable->mdl_name;
		if (lstrcmpW(ptable->cmo_name, L"") != 0)cmo_name = ptable->cmo_name;

		Vector3 rot = ptable->rot;
		//度数法からラジアンに変換
		rot.x = XMConvertToRadians(ptable->rot.x);
		rot.y = XMConvertToRadians(ptable->rot.y);
		rot.z = XMConvertToRadians(ptable->rot.z);

		landShape->Initialize(mdl_name, cmo_name);
		landShape->SetTrans(ptable->trans);
		landShape->SetRot(rot);
		landShape->SetScale(ptable->scale);
		//配列に要素として追加
		landShapeArray.push_back(landShape);
	}

	

	Vector3 pos2[20];
	for (int i = 0; i < 20; i++)
	{
		pos2[i] = Vector3(rand() % 100 - 50, 0, rand() % 100 - 50);
	}

	//Vector3 tank_pos;
	//Obj3Dの生成
	Obj3D* ground = new Obj3D;
	Obj3D* effect = new Obj3D;
	
	//モデルの読み込み
	ground->LoadModelFile(L"Resources/cModels/ground2002.cmo");
	effect->LoadModelFile(L"Resources/cModels/sander.cmo");
	
	std::unique_ptr<LandShapeData> landshapeData = LandShapeData::CreateFromMDL(L"Resources/LandShape/tail.MDL");

	LandShapeCommonDef def;
	def.pCamera = &camera;
	def.pDevice = g_pd3dDevice.Get();
	def.pDeviceContext = g_pImmediateContext.Get();
	LandShape::InitializeCommon(def);

	//地形データを作成（背景物の数だけ繰り返す）
	LandShape* landshape = new LandShape;
	//MLファイル、CMOファイルを読み込んで初期化
	landshape->Initialize(L"Resources/LandShape/cubebox2.MDL", L"Resources/cModels/cubebox2.cmo");
	//ワールド座標を指定
	landshape->SetTrans(Vector3(0, 0, 0));
	//ワールド回転を指定（※ラジアン）
	landshape->SetRot(Vector3(0, 0, 0));
	//ワールドスケーリングを指定(X,Y,Z共通)
	landshape->SetScale(10.0f);

	//effect->SetTrans(Vector3(0, 50, 0));

	Obj3D* dorm = new Obj3D;
	//モデルの読み込み
	dorm->LoadModelFile(L"Resources/cModels/SkyDorm.cmo");
	
	Player* player = new Player;
	player->Initialize();
	
	const int ENEMY_NUM = 1;
	Enemy* enemy[ENEMY_NUM];
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		enemy[i] = new Enemy;
		enemy[i]->Initialize();
		enemy[i]->SetTrans(Vector3(8, 0, -24));
	}



	//////カプセルと球のの当たり判定

	//球A
	Sphere sphereA;
	//Aの中心座標
	sphereA.Center = Vector3(10.0f, 0, 0);
	//Aの半径
	sphereA.radius = 5.0f;

	//カプセルA
	Capsule capsuleA;
	capsuleA.segment.start = Vector3(-10.0f, 0, 0);
	capsuleA.segment.end = Vector3(0.0f, 0, 0);
	capsuleA.radius = 1.0f;

	bool hit = CheckSphere2Capsule(sphereA, capsuleA);
	if (hit)
	{
		OutputDebugString(L"当たった!\n");
	}
	else
	{
		OutputDebugString(L"当たってない!\n");
	}


	CollisionNode::SetDebugVisible(false);
	int TT = 0;
	// メインループ
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		int fps = fpsTimer.GetNowFPS();
		wchar_t buf[16];
		swprintf_s(buf, 16, L"fps = %d", fps);


		// メッセージが来ているか調べる
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			// メッセージが来ていればウインドウプロシージャへ送る
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// キー入力やマウス情報の更新
			DirectXTK_UpdateInputState();


			if (g_keyTracker->pressed.D1)
			{
				TT += 1;
				if (TT == 1)
				{
					CollisionNode::SetDebugVisible(true);
				}
				if (TT == 2)
				{
					CollisionNode::SetDebugVisible(false);
					TT *= 0;
				}
			}

			// デバッグカメラの更新
			camera.Update();

			// バックバッファのクリア
			g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView.Get(), DirectX::Colors::MidnightBlue);

			// 深度バッファのクリア
			g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			//static float angle = 0;
			//angle += 0.05f;
			//---------- ここで描画コマンドを発行する　---------//
			g_spriteBatch->Begin();

			//ワールド行列
			Matrix world = Matrix::Identity;

			// ビュー行列を作成する
			Matrix view = camera.GetViewmat();

			const float ROT_SPEEDS = 0.03f;
			const float CAMERA_DISTANCE = 5.0f;
			Vector3 nextEyepos, nextRefpos;

			// 視点
			Vector3 cameraV(0.0f, 2.0f, CAMERA_DISTANCE);
			Matrix playerworld = player->GetLocalWorld();
			cameraV = Vector3::TransformNormal(cameraV, playerworld);
			nextEyepos = player->GetTrans() + cameraV;
			camera.SetEyepos(camera.GetEyepos() + (nextEyepos - camera.GetEyepos())*0.05);
			camera.Update();

			// 注視点
			Vector3 refV(0.0f, 0.5f, 0.0);
			refV = Vector3::TransformNormal(refV, playerworld);
			nextRefpos = player->GetTrans() + refV;
			//現在camera.GetRefpos()
			camera.SetRefpos(camera.GetRefpos() + (nextRefpos - camera.GetRefpos())*0.95);

			//カメラ回転
			Vector3 upVec(0.0f,1.0f,0.0f);
			upVec = Vector3::TransformNormal(upVec, playerworld);
			//現在camera.GetRefpos()
			camera.SetUpvec(upVec);



			//	// 射影行列を作成する
			Matrix proj = camera.GetProjmat();

			//	// グリッド床の表示
			gridFloor.Render(view, proj);


			world = Matrix::Identity;

			ground->Calc();
			dorm->Calc();
	

			player->Update();
			player->Draw();

			for (int i = 0; i < ENEMY_NUM; i++)
			{
				enemy[i]->Update();
				enemy[i]->Draw();
			}

			//弾丸と敵の当たり判定（PlayerとEnemyのUpdate後に行うこと）
			{
				//弾丸の当たり判定カプセル
				const Capsule& bulletCapsule = player->GetCollisionNodeBullet();
				//敵の数だけ処理する
				for (int i = 0; i < ENEMY_NUM; i++)
				{
					//既に死んでいる敵は、スキップ
					if (enemy[i]->GetDeath()) continue;

					//敵の当たり判定球
					const Sphere& enemySphere = enemy[i]->GetCollisionNodeBody();
					const Sphere& enemySphere2 = enemy[i]->GetCollisionNodeARN();
					const Sphere& enemySphere3 = enemy[i]->GetCollisionNodeBRE();

					//衝突点の座標を入れる変数
					Vector3 inter;

					//球とカプセルの衝突判定
					if (CheckSphere2Capsule(enemySphere, bulletCapsule, &inter))
					{
						//弾丸を元の位置に戻す
						player->ResetBullet();
						//敵が死ぬ
						enemy[i]->SetDeath(true);
			
						//effect->SetTrans(inter);

					}

					if (CheckSphere2Capsule(enemySphere2, bulletCapsule, &inter))
					{
						//弾丸を元の位置に戻す
						player->ResetBullet();
						//敵が死ぬ
						enemy[i]->SetDeath2(true);
					}
					if (CheckSphere2Capsule(enemySphere3, bulletCapsule, &inter))
					{
						//弾丸を元の位置に戻す
						player->ResetBullet();
						//敵が死ぬ
						enemy[i]->SetDeath3(true);
					}
				}
			}

			//全地形を更新
			for (std::vector<LandShape*>::iterator it = landShapeArray.begin(); it != landShapeArray.end(); it++)
			{
				LandShape* landShape = *it;
				if (landShape == nullptr)continue;

				landShape->Calc();
			}
			for (std::vector<LandShape*>::iterator it = landShapeArray.begin(); it != landShapeArray.end(); it++)
			{
				LandShape* landShape = *it;
				if (landShape == nullptr)continue;

				landShape->Draw();
			}
			//壁の当たり判定 ※プレイヤーの更新後に処理を行うこと　※本当は関数化するべき
			{
				//当たり判定球の取得
				Sphere sphere = player->GetCollisionNodeBody();
				//プレイヤー原点のワールド座標を取得
				Vector3 trans = player->GetTrans();
				//球とプレイヤー原点のずれを計算
				Vector3 sphere2player = trans - sphere.Center;

				//全ての地形データについて処理
				for (std::vector<LandShape*>::iterator it = landShapeArray.begin();
				it != landShapeArray.end();it++)
				{
					LandShape* pLandShape = *it;

					Vector3 reject;//排斥ベクトルを入れるための変数
					//球と地形の交差判定
					if (pLandShape->IntersectSphere(sphere, &reject))
					{//交差している場合
						//めり込み分だけ、球を押し出すように移動
						sphere.Center = sphere.Center + reject;
					}
				}
				//食い込み補正後の位置から、プレイヤーのワールド座標を計算
				player->SetTrans(sphere.Center + sphere2player);
				//行列を再計算
				player->Calc();
			}
				// 地面の当たり判定
			{
				// 自機の速度取得
				const Vector3& vel = player->GetVelocity();
				// 落下中か
				if (vel.y <= 0.0f) {
					bool hit = false;
					Segment player_segment;
					Vector3 player_pos = player->GetTrans();
					// 自機の頭から足元まで貫く線分
					player_segment.start = player_pos + Vector3(0, 1.0f, 0);
					player_segment.end = player_pos + Vector3(0, -0.5f, 0);

					// 距離保存用　大きい数字で初期化
					float distance = 1.0e5;
					Vector3 inter;
					// 全ての地形データについて処理
					for (std::vector<LandShape*>::iterator it = landShapeArray.begin();
					it != landShapeArray.end();
						it++) {
						LandShape* pLandShape = *it;
						float temp_distance;
						Vector3 temp_inter;

						// 線分と地形の交差判定
						if (pLandShape->IntersectSegment(player_segment, &temp_inter)) {
							hit = true;
							temp_distance = Vector3::Distance(player_segment.start, temp_inter);				// 今までで一番距離が近いか
							if (temp_distance < distance)
							{
								inter = temp_inter;
								distance = temp_distance;
							}
						}
					}

					if (hit) {
						Vector3 new_position = player_pos;
						new_position.y = inter.y;
						player->StopJump();
						player->SetTrans(new_position);
					}

					if (!hit) {// 落下開始
						player->StartFall();
					}
					// 行列の再計算
					player->Calc();
				}
			}

		  //   	effect->Calc();
				//effect->Draw();
				ground->Draw();
				dorm->Draw();
				//landshape->Draw();

				if(g_key.K)
				{
					g_spriteFont->DrawString(g_spriteBatch.get(), L"CLEAR", Vector2(0, 0));
				}
				g_spriteBatch->End();

				// 画面更新を待つ
				fpsTimer.WaitFrame();

				// バックバッファとフロントバッファを入れ替える
				g_pSwapChain->Present(0, 0);
			}
		}
		

		PolygonTerm();
		delete factory;
		// DirectXデバイス周りの終了処理
		Direct3D_CleanupDevice();

		return 0;
}

//--------------------------------------------------------------------------------------
// ウインドウの作成
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// ウインドウクラスを登録する
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)NULL);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"WindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)NULL);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// ウインドウを作成する
	g_hInst = hInstance;
	RECT rc = { 0, 0, WINDOW_W, WINDOW_H };	// ←ウインドウサイズ
	AdjustWindowRect(&rc, WINDOW_STYLE, FALSE);
	g_hWnd = CreateWindow(L"WindowClass", L"3Dプログラミング基礎", WINDOW_STYLE,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!g_hWnd)
		return E_FAIL;

	// ウインドウの表示
	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

//--------------------------------------------------------------------------------------
// ウインドウプロシージャー
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:		// ウインドウが破棄されたメッセージ
		PostQuitMessage(0);	// WM_QUITメッセージを送出する（メインループを抜ける）
		break;

	case WM_ACTIVATEAPP:	// ウインドウがアクティブ化、非アクティブ化する時に来るメッセージ
		Keyboard::ProcessMessage(message, wParam, lParam);
		Mouse::ProcessMessage(message, wParam, lParam);
		break;

	case WM_KEYDOWN:		// キー入力関係のメッセージ
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	case WM_INPUT:			// マウス関係のメッセージ
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(message, wParam, lParam);
		break;

	default:
		// 自分で処理しないメッセージはDefWindowProc関数に任せる
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

