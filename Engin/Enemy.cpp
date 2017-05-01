//�G

#include "Enmey.h"
#include"DirectXTK.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Enemy::Enemy()
{
	m_Timer = 0;
	m_DistAngle = 0.0f;
}


Enemy::~Enemy()
{
}

void Enemy::Initialize()
{
	m_Obj[PARTS_TANK].LoadModelFile(L"Resources/cModels/zentai.cmo");

	m_Obj[PARTS_BREAST].SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());

	m_Obj[PARTS_TANK].SetTrans(Vector3(0, 0.01f, 0));

	m_Obj[PARTS_BREAST].LoadModelFile(L"Resources/cModels/tankhou1.cmo");

	m_Obj[PARTS_HEAD].SetParentMatrix(&m_Obj[PARTS_BREAST].GetLocalWorld());

	m_Obj[PARTS_BREAST].SetTrans(Vector3(0, 0.01f, 0));
	//
	m_Obj[PARTS_HEAD].LoadModelFile(L"Resources/cModels/tankhou2.cmo");

	m_Obj[PARTS_ARN_R].SetParentMatrix(&m_Obj[PARTS_BREAST].GetLocalWorld());

	m_Obj[PARTS_HEAD].SetTrans(Vector3(0, 0.01f, 0));

	m_Obj[PARTS_ARN_R].LoadModelFile(L"Resources/cModels/houdaiiii.cmo");

	m_Obj[PARTS_ARN_R].SetTrans(Vector3(0, 0.01f, 0));

	//������
	m_CollisionNodeBody.Initialize();
	//�e�s���ݒ�
	m_CollisionNodeBody.SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());
	//���̔��a
	m_CollisionNodeBody.SetLocalRadius(0.2f);
	//�e�s�񂩂�̈ʒu
	m_CollisionNodeBody.SetTrans(Vector3(0,0.7f,0));

	//������
	m_CollisionNodeARN.Initialize();
	//�e�s���ݒ�
	m_CollisionNodeARN.SetParentMatrix(&m_Obj[PARTS_ARN_R].GetLocalWorld());
	//���̔��a
	m_CollisionNodeARN.SetLocalRadius(0.4f);
	//�e�s�񂩂�̈ʒu
	m_CollisionNodeARN.SetTrans(Vector3(0, 0.7f, 0));

	//������
	m_CollisionNodeBRE.Initialize();
	//�e�s���ݒ�
	m_CollisionNodeBRE.SetParentMatrix(&m_Obj[PARTS_BREAST].GetLocalWorld());
	//���̔��a
	m_CollisionNodeBRE.SetLocalRadius(0.6f);
	//�e�s�񂩂�̈ʒu
	m_CollisionNodeBRE.SetTrans(Vector3(0, 0.7f, 0));


	death = false;
	death2 = false;
	death3 = false;
}

void Enemy::Update()
{
	if (death)
	{
		return;
	}
	//����I�ɐi�s������ς���
	//m_Timer--;	//�����o�ϐ��Ń^�C�}�[������Ă��A�J�E���g�_�E��
	//if (m_Timer < 0)
	//{
	//	//�J�E���g���O�ɒB������
	//	m_Timer = 60;
	//	//�ڕW�p�x��ύX
	//	float rnd = (float)rand() / RAND_MAX - 0.5f;
	//	rnd *= 180.0f;
	//	m_DistAngle += rnd;//�����o�ϐ��ŖڕW�p�x��ێ�
	//}
	////�ڕW�p�x�Ɍ������āA�@�̂̊p�x�����킶����
	//{
	//	Vector3 rotv = m_Obj[PARTS_TANK].GetRot();
	//	//�ڕW�p�x�ւ́A�ŒZ�p�x���擾
	//	float angle = GetShortAngleRad(rotv.y, XMConvertToRadians(m_DistAngle));
	//	rotv.y += angle*0.01f;
	//	m_Obj[PARTS_TANK].SetRot(rotv);
	//}
	////�@�̂̌����Ă�������ɐi��
	//{
	//	//���̍��W���擾
	//	Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
	//	
	//	Vector3 move(0, 0, -0.02f);
	//	Vector3 rotv = m_Obj[PARTS_TANK].GetRot();
	//	Matrix rotm = Matrix::CreateRotationY(rotv.y);
	//	
	//	//���W���ړ�
	//	trans += move;

	//	//�ړ���̍��W���Z�b�g
	//	m_Obj[PARTS_TANK].SetTrans(trans);

		//�ړ��𔽉f���čs��X�V
		Calc();

		//�����蔻��̍X�V�i�e�s��X�V��ɍs�����Ɓj
		m_CollisionNodeBody.Update();
		m_CollisionNodeARN.Update();
		m_CollisionNodeBRE.Update();

		if (death2)
		{
			m_Obj[PARTS_ARN_R].SetTrans(Vector3(m_Obj[PARTS_ARN_R].GetTrans().x, m_Obj[PARTS_ARN_R].GetTrans().y + 0.1f, m_Obj[PARTS_ARN_R].GetTrans().z));
			m_Timer2++;
		}
}



float Enemy::GetShortAngleRad(float nowangle, float dstAngle)
{
	

	if (dstAngle - nowangle < XM_PI)
	{
		return dstAngle - nowangle;
	}
	else
	{
		return (XM_2PI - (nowangle-dstAngle));
	}
}

void Enemy::Calc()
{
	for (int i = 0; i < PARTS_NUM; i++)
	{
		if (i==PARTS_ARN_R)
		{
			if (death2&&60 < m_Timer2)
			{
				continue;
			}
		}
		if (i == PARTS_BREAST)
		{
			if (death3)
			{
				continue;
			}
		}
		m_Obj[i].Calc();
	}

}

void Enemy::Draw()
{
	if (death)
	{
		return;
	}
	for (int i = 0; i < PARTS_NUM; i++)
	{
		if (i==PARTS_ARN_R)
		{
			if (death2 && 60 < m_Timer2)
			{
				continue;
			}
		}
		if (i == PARTS_BREAST)
		{
			if (death3)
			{
				continue;
			}
		}

		m_Obj[i].Draw();
	}
	m_CollisionNodeBody.Draw();
	m_CollisionNodeARN.Draw();
	m_CollisionNodeBRE.Draw();
}

const DirectX::SimpleMath::Vector3& Enemy::GetTrans()
{
	return m_Obj[PARTS_TANK].GetTrans();
}

void Enemy::SetTrans(const DirectX::SimpleMath::Vector3& trans)
{
	m_Obj[PARTS_TANK].SetTrans(trans);
}

const DirectX::SimpleMath::Matrix& Enemy::GetLocalWorld()
{
	return m_Obj[PARTS_TANK].GetLocalWorld();
}

const DirectX::SimpleMath::Vector3 Enemy::GetRot()
{
	return m_Obj[PARTS_TANK].GetRot();
}

bool Enemy::GetDeath()
{
	return 	death;
}

void Enemy::SetDeath(bool flag)
{
	death = flag;
}

bool Enemy::GetDeath2()
{
	return 	death2;
}

void Enemy::SetDeath2(bool flag2)
{
	death2 = flag2;
}
bool Enemy::GetDeath3()
{
	return 	death3;
}

void Enemy::SetDeath3(bool flag3)
{
	death3 = flag3;
}

