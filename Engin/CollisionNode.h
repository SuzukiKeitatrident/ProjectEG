//----------------------------------------------------------------------
//ファイル名:CollisionNode
//作成者:
//作成日
//説明：衝突判定用ノード
//		任意のワールド行列にぶらさげ、親子関係を結ぶことができます。
//		デバッグ用に、当たり判定を表示する機能があります。
//----------------------------------------------------------------------
#pragma once

#include"Obj3D.h"
#include"Collision.h"


class CollisionNode
{
protected:
	//デバッグ表示のON/OFFを設定
	static bool s_DebugVisible;
public:
	//デバッグ表示のON/OFFを設定
	static void SetDebugVisible(bool flag) { s_DebugVisible = flag; }
	//デバッグ表示のON/OFFを取得
	static bool GetDebugVisible(void) { return s_DebugVisible; }
};

//当たり判定球ノード
class SphereNode : public Sphere
{
public:
	//コンストラクタ
	SphereNode();
	//初期化
	void Initialize();
	//描画
	void Draw();
	//更新
	void Update();
	//親行列を設定
	void SetParentMatrix(const DirectX::SimpleMath::Matrix* pParentMatrix);
	//ローカル半径を設定
	void SetLocalRadius(float radius) { m_LocalRadius = radius; }
	//親行列からのオフセットを設定
	void SetTrans(const DirectX::SimpleMath::Vector3& trans) { m_Trans = trans; }
protected:
	//表示用オブジェクト
	Obj3D m_Obj;
	//親行列の影響を受ける前のローカル半径
	float m_LocalRadius;
	//親行列からのオフセット座標
	DirectX::SimpleMath::Vector3 m_Trans;
};

//デバッグ表示付きカプセルノード
class CapsuleNode :public Capsule
{
public:
	//コンストラクタ
	CapsuleNode();
	//初期化
	void Initialize();
	//描画
	void Draw();
	//更新
	void Update();
	//親行列を設定
	void SetParentMatrix(const DirectX::SimpleMath::Matrix* pParentMatrix);
	//親行列からの回転を設定
	void SetRot(const DirectX::SimpleMath::Vector3& rot);
	//ローカル半径を設定
	void SetLocalRadius(float radius) { m_LocalRadius = radius; }
	//ローカル幅長さを設定
	void SetLenngth(float length) { m_LocalLength = length; }
	//親行列からのオフセットを設定
	void SetTrans(const DirectX::SimpleMath::Vector3& trans) { m_Trans = trans; }

	void GetCollisionNodeBullet();

protected:
	//表示用のオブジェクト
	Obj3D m_Obj;
	//親行列の影響を受ける前のローカル軸長さ
	float m_LocalLength;
	//親行列の影響を受ける前のローカル半径
	float m_LocalRadius;
	//親行列からのオフセット座標
	DirectX::SimpleMath::Vector3 m_Trans;
};