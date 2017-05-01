#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
//#include"FrameworkInputManager.h"
#include"Obj3D.h"
#include"CollisionNode.h"

class Player
{
public:
	//ロボットパーツ
	enum

	{
		PARTS_TANK,
		PARTS_WAIST,
		PARTS_BREAST,
		PARTS_HEAD,
		PARTS_ARN_R,
		PARTS_GUN_R,

		PARTS_NUM
	};
	Player();
	~Player();
	void Initialize();
	void Update();
	void Calc();
	void Draw();
	void FireBullet();
	void ResetBullet();
	void StartJump();
	void StartFall();
	void StopJump();

	const DirectX::SimpleMath::Vector3& GetTrans();
	const DirectX::SimpleMath::Vector3& GetVelocity();

	void SetTrans(const DirectX::SimpleMath::Vector3& trans);
	const DirectX::SimpleMath::Matrix& GetLocalWorld();
	const DirectX::SimpleMath::Vector3 GetRot();
	//void SetInputManager(Franework::InputManager* pInputManager) { m_pInputManager = pInputManager; }
	bool m_FireFlag;
	//弾丸の当たり判定カプセルを取得
	const CapsuleNode& GetCollisionNodeBullet(){return  m_CollsionNodeBullet; }
	const SphereNode& GetCollisionNodeBody() { return m_CollisionNodeBody; }
	const SphereNode& GetCollisionNodeARN() { return m_CollisionNodeARN; }
	const SphereNode& GetCollisionNodeBRE() { return m_CollisionNodeBRE; }
	// ジャンプ初速
	static const float JUMP_SPEED_FIRST;
	static const float JUMP_SPEED_MAX;
	// 重力加速
	static const float GRAVITY_ACC;


protected:
	Obj3D m_Obj[PARTS_NUM];

	int TT;
	DirectX::SimpleMath::Vector3 m_BulletVel;
	//Framework::InputManager* m_pInputManager;
	//弾丸の当たり判定カプセル
	CapsuleNode m_CollsionNodeBullet;

	SphereNode m_CollisionNodeBody;
	SphereNode m_CollisionNodeARN;
	SphereNode m_CollisionNodeBRE;

	// 速度
	DirectX::SimpleMath::Vector3 m_Velocity;
	// ジャンプ（落下）中か？
	bool m_isJump;
	
	bool m_isFall;
};

