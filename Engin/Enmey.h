#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
//#include"FrameworkInputManager.h"
#include"Obj3D.h"
#include"CollisionNode.h"

class Enemy
{
public:
	//���{�b�g�p�[�c
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
	Enemy();
	~Enemy();


	float GetShortAngleRad(float nowangle, float dstAngle);

	void Initialize();
	void Update();
	void Calc();
	void Draw();
	const DirectX::SimpleMath::Vector3& GetTrans();
	void SetTrans(const DirectX::SimpleMath::Vector3& trans);
	const DirectX::SimpleMath::Matrix& GetLocalWorld();
	const DirectX::SimpleMath::Vector3 GetRot();
	//void SetInputManager(Franework::InputManager* pInputManager) { m_pInputManager = pInputManager; }
	const SphereNode& GetCollisionNodeBody() { return m_CollisionNodeBody; }
	const SphereNode& GetCollisionNodeARN() { return m_CollisionNodeARN; }
	const SphereNode& GetCollisionNodeBRE() { return m_CollisionNodeBRE; }

	bool GetDeath();
	void SetDeath(bool flag);


	bool GetDeath2();
	void SetDeath2(bool flag2);
	
	bool GetDeath3();
	void SetDeath3(bool flag3);



protected:
	Obj3D m_Obj[PARTS_NUM];
	//Framework::InputManager* m_pInputManager;
	//�i�H�ύX�^�C�}�[
	int m_Timer;
	//�ڕW�p�x���x��
	float m_DistAngle;
	DirectX::SimpleMath::Vector3 m_BulletVel;
	//�G�{�̂̓����蔻�苅
	SphereNode m_CollisionNodeBody;
	SphereNode m_CollisionNodeARN;
	SphereNode m_CollisionNodeBRE;

	bool death;
	bool death2;
	bool death3;

	int m_Timer2;
};

