#pragma once
#include <d3d11.h>
#include<SimpleMath.h>		//Vector3���g������

//��
class Sphere 
{
public:
	DirectX::SimpleMath::Vector3 Center;//���S���W
	float radius;//���a
	//�R���X�g���N�^
	Sphere()
	{
		radius = 1.0f;
	}

};

//����
class Segment
{
public:
	DirectX::SimpleMath::Vector3 start;		//�n�_���W
	DirectX::SimpleMath::Vector3 end;		//�I�_���W

};

//�J�v�Z���i�����X�E�B�[�u�����`��j
class Capsule
{
public:
	Segment segment;	//�c��
	float radius;	//���a
	Capsule()
	{
		segment.start = DirectX::SimpleMath::Vector3(0, 0, 0);
		segment.end = DirectX::SimpleMath::Vector3(0, 1, 0);
		radius = 1.0f;
	}

};

// �@���t���O�p�`�i�����v��肪�\�ʁj
class Triangle
{
public:
	DirectX::SimpleMath::Vector3	P0;
	DirectX::SimpleMath::Vector3	P1;
	DirectX::SimpleMath::Vector3	P2;
	DirectX::SimpleMath::Vector3	Normal;	// �@���x�N�g��
};

void ClosestPtPoint2Segment(const DirectX::SimpleMath::Vector3& _point, const Segment& _segment, DirectX::SimpleMath::Vector3* _closest);

void ClosestPtSegment2Segment(const Segment& _segment0, const Segment& _segment1, DirectX::SimpleMath::Vector3* _closest0, DirectX::SimpleMath::Vector3* _closest1);

bool CheckSphere2Sphere(const Sphere& _sphereA, const Sphere& _sphereB,DirectX::SimpleMath::Vector3* _inter = nullptr);

bool CheckCapsule2Capsule(const Capsule& _capsule0, const Capsule& _capsule1,DirectX::SimpleMath::Vector3* _inter =nullptr);

bool CheckSphere2Capsule(const Sphere& _sphere, const Capsule& _capsule,DirectX::SimpleMath::Vector3* _inter = nullptr);

void ComputeTriangle(const DirectX::SimpleMath::Vector3& _p0, const DirectX::SimpleMath::Vector3& _p1, const DirectX::SimpleMath::Vector3& _p2, Triangle* _triangle);

void ClosestPtPoint2Triangle(const DirectX::SimpleMath::Vector3& _point, const Triangle& _triangle, DirectX::SimpleMath::Vector3* _closest);

bool CheckPoint2Triangle(const DirectX::SimpleMath::Vector3& _point, const Triangle& _triangle);

bool CheckSphere2Triangle(const Sphere& _sphere, const Triangle& _triangle, DirectX::SimpleMath::Vector3 *_inter);

float VectorLength(const DirectX::SimpleMath::Vector3& v);

float VectorLengthSQ(const DirectX::SimpleMath::Vector3& v);

bool CheckSegment2Triangle(const Segment& _segment, const Triangle& _triangle, DirectX::SimpleMath::Vector3 *_inter);


float Distance3D(const DirectX::SimpleMath::Vector3& p1, const DirectX::SimpleMath::Vector3& p2);

float DistanceSQ3D(const DirectX::SimpleMath::Vector3& p1, const DirectX::SimpleMath::Vector3& p2);

float GetSqDistancePoint2Segment(const DirectX::SimpleMath::Vector3& _point, const Segment& _segment);
float GetSqDistanceSegment2Segment(const Segment& _segment0, const Segment& _segment1);