#pragma once

#include"LandShapeData.h"

#include<PrimitiveBatch.h>
#include<VertexTypes.h>
#include<CommonStates.h>

#include"Camera.h"
#include"Obj3D.h"
#include"Collision.h"

struct LandShapeCommonDef
{
	//デバイス
	ID3D11Device* pDevice;
	//デバイスコンテキスト
	ID3D11DeviceContext* pDeviceContext;
	//カメラ
	Camera* pCamera;
};

//共有データ
class LandShapeCommon
{
	friend class LandShape;
public:
	//コンストラクタ
	LandShapeCommon(LandShapeCommonDef def);
	//デストラクタ
	~LandShapeCommon();
protected:
	//共通カメラ
	Camera* m_pCamera;
	//コモンステートへのポインタ
	std::unique_ptr<DirectX::CommonStates> m_pStates;
	//エフェクトファクトリ
	std::unique_ptr<DirectX::EffectFactory> m_pEffectFactory;
	//ベーシックエフェクトへのポインタ
	std::unique_ptr<DirectX::BasicEffect> m_pEffect;
	//プリミティブバッチへのポインタ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionNormal>> m_pPrimitiveBatch;
	//入力レイアウトへのポインタ
	ID3D11InputLayout* m_pInputLayout;
	//デバイスコンテキストへのポインタ
	ID3D11DeviceContext* m_pDeviceContext;

};

class LandShape
{
public:
	//エフェクトファクトリ生成
	static void InitializeCommon(LandShapeCommonDef def);

	//コンストラクタ
	LandShape();
	//初期化
	void Initialize(const wchar_t* filename_mdl, const wchar_t*filename_cmo = nullptr);
	//ワールド行列の計算
	void Calc();
	//デバッグ描画
	void Draw();


	//アクセッサ
	void SetTrans(const DirectX::SimpleMath::Vector3& trans) { m_Obj.SetTrans(trans); }
	void SetRot(const DirectX::SimpleMath::Vector3& rot) { m_Obj.SetRot(rot); }
	void SetScale(float scale) { m_Obj.SetScale(DirectX::SimpleMath::Vector3(scale)); }
	void SetLocalWorld(const DirectX::SimpleMath::Matrix& mat) { m_Obj.SetLocalWorld(mat); }

	const DirectX::SimpleMath::Vector3& GetTrans() { return m_Obj.GetTrans(); }
	const DirectX::SimpleMath::Vector3& GetRot() { return m_Obj.GetRot(); }
	float GetScale() { return m_Obj.GetScale().x; }
	const DirectX::SimpleMath::Matrix& GetLocalWorld() { return m_Obj.GetLocalWorld(); }

	bool IntersectSphere(const Sphere&sphere, DirectX::SimpleMath::Vector3* reject);
	bool LandShape::IntersectSegment(const Segment& segment, DirectX::SimpleMath::Vector3* inter);


protected:
	//共通データ
	static std::unique_ptr<LandShapeCommon> s_pCommon;
	//地形当たりデータマップ
	static std::map<std::wstring, std::unique_ptr<LandShapeData>> s_dataarray;

	//表示オブジェクト
	Obj3D m_Obj;
	//地形当たりデータ
	const LandShapeData* m_pData;
	//ワールド→モデル行列
	DirectX::SimpleMath::Matrix m_WorldLocal;

};

