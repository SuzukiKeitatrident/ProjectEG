#include "Player.h"
#include"DirectXTK.h"


using namespace DirectX;
using namespace DirectX::SimpleMath;
// static�ϐ��̎���
const float Player::GRAVITY_ACC = 0.03f;
const float Player::JUMP_SPEED_FIRST = 0.5f;
const float Player::JUMP_SPEED_MAX = 0.3f;

Player::Player()
{
}


Player::~Player()
{
}

void Player::Initialize()
{
	m_Obj[PARTS_TANK].LoadModelFile(L"Resources/cModels/zentai.cmo");
	
	m_Obj[PARTS_BREAST].SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());

	m_Obj[PARTS_TANK].SetTrans(Vector3(0, 0.1f, 0));

	m_Obj[PARTS_BREAST].LoadModelFile(L"Resources/cModels/tankhou1.cmo");

	m_Obj[PARTS_HEAD].SetParentMatrix(&m_Obj[PARTS_BREAST].GetLocalWorld());

	m_Obj[PARTS_BREAST].SetTrans(Vector3(0, 0.01f, 0));
	//
	m_Obj[PARTS_HEAD].LoadModelFile(L"Resources/cModels/tankhou2.cmo");

	m_Obj[PARTS_ARN_R].SetParentMatrix(&m_Obj[PARTS_BREAST].GetLocalWorld());

	m_Obj[PARTS_HEAD].SetTrans(Vector3(0, 0.01f, 0));

	m_Obj[PARTS_ARN_R].LoadModelFile(L"Resources/cModels/houdaiiii.cmo");

	m_Obj[PARTS_ARN_R].SetTrans(Vector3(0, 0.01f, 0));

	m_FireFlag = false;

	m_isJump = false;

	m_isFall = false;

	//������
	m_CollisionNodeBody.Initialize();
	//�e�s���ݒ�
	m_CollisionNodeBody.SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());
	//���̔��a
	m_CollisionNodeBody.SetLocalRadius(0.6f);
	//�e�s�񂩂�̈ʒu
	m_CollisionNodeBody.SetTrans(Vector3(0, 0.7f, 0));

	TT = 0;
	{
		//������
		m_CollsionNodeBullet.Initialize();
		//�e�s���ݒ�
		m_CollsionNodeBullet.SetParentMatrix(&m_Obj[PARTS_ARN_R].GetLocalWorld());
		//�J�v�Z���̔��a
		m_CollsionNodeBullet.SetLocalRadius(0.75f);
		//�J�v�Z���̎�����
		m_CollsionNodeBullet.SetLenngth(2.0f);
		//�e�s�񂩂�̈ʒu
		m_CollsionNodeBullet.SetTrans(Vector3(0, 0, 0));
		//�e�s�񂩂�̉�]
		m_CollsionNodeBullet.SetRot(Vector3(-XM_PIDIV2, 0, 0));
	}
}

void Player::Update()
{
	
	const float ROT_SPEED = 0.03f;
	if (g_key.Up)
	{
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.x -= ROT_SPEED;
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	if (g_key.Down)
	{
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.x += ROT_SPEED;
		m_Obj[PARTS_TANK].SetRot(rot);
	}

	if(g_key.A)
	{
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y += ROT_SPEED;
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	//if (m_pInputManager->GetKeyboard()->IsKeyDown('D'))
	if (g_key.D)
	{
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y -= ROT_SPEED;
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	//�O�i���
	if (g_key.W)
	{
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
		float rot_y = m_Obj[PARTS_TANK].GetRot().y;
		float rot_x = m_Obj[PARTS_TANK].GetRot().x;
		float rot_z = m_Obj[PARTS_TANK].GetRot().z;
		
		SimpleMath::Vector3 moveV(0, 0,+0.1);
		Matrix rotm = Matrix::CreateRotationY(rot_y);
		rotm *= Matrix::CreateRotationX(rot_x);
		rotm *= Matrix::CreateRotationZ(rot_z);

		moveV = Vector3::TransformNormal(moveV,rotm);
		trans -= moveV;
		m_Obj[PARTS_TANK].SetTrans(trans);
	}
	if (g_key.S)
	{
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
		float rot_y=m_Obj[PARTS_TANK].GetRot().y;
		float rot_x = m_Obj[PARTS_TANK].GetRot().x;
		float rot_z = m_Obj[PARTS_TANK].GetRot().z;

		SimpleMath::Vector3 moveV(0, 0, +0.1f);
		Matrix rotm = Matrix::CreateRotationY(rot_y);
		rotm *= Matrix::CreateRotationX(rot_x);
		rotm *= Matrix::CreateRotationZ(rot_z);
		moveV = Vector3::TransformNormal(moveV,rotm);
		trans += moveV;
		m_Obj[PARTS_TANK].SetTrans(trans);
	}
	
	
	if (g_key.I)
	{
		Vector3 rot = m_Obj[PARTS_BREAST].GetRot();
		rot.x -= ROT_SPEED;
		m_Obj[PARTS_BREAST].SetRot(rot);
	}
	if (g_key.R)
	{
		Vector3 rot = m_Obj[PARTS_WAIST].GetRot();
		rot.y += 0.1f;
		rot.x -= ROT_SPEED;
		m_Obj[PARTS_WAIST].SetRot(rot);
	}
	if (g_key.Y)
	{
		Vector3 rot = m_Obj[PARTS_BREAST].GetRot();
		rot.y += 0.1f;
		m_Obj[PARTS_BREAST].SetRot(rot);
	}
	if (g_key.T)
	{
		Vector3 rot = m_Obj[PARTS_HEAD].GetRot();
		rot.y -= ROT_SPEED;
		m_Obj[PARTS_HEAD].SetRot(rot);
	}

	if (g_key.F)
	{
		Vector3 rot = m_Obj[PARTS_ARN_R].GetRot();
		rot.y += ROT_SPEED;
		m_Obj[PARTS_ARN_R].SetRot(rot);
	}
	if (g_key.G)
	{
		Vector3 rot = m_Obj[PARTS_ARN_R].GetRot();
		rot.y -= ROT_SPEED;
		m_Obj[PARTS_ARN_R].SetRot(rot);
	}

	if (m_FireFlag)
	{
		m_BulletVel.y -= 0.01f;
		m_Obj[PARTS_ARN_R].SetTrans(m_Obj[PARTS_ARN_R].GetTrans() + m_BulletVel);
	}
	
	if (g_keyTracker->pressed.P)
	{
		TT += 1;
		if (TT == 1)
		{
			FireBullet();
		}
		if (TT == 2)
		{
			ResetBullet();
			TT*= 0;
		}
	}
	if (m_isJump)
	{
		m_Velocity.y -= 0.01f;
	}

	if (g_keyTracker->pressed.Space)
	{
		StartJump();
	}

	m_Obj[PARTS_TANK].SetTrans(m_Obj[PARTS_TANK].GetTrans() + m_Velocity);
	


	Calc();

	m_CollsionNodeBullet.Update();
	m_CollisionNodeBody.Update();
}

void Player::Calc()
{

	for (int i = 0; i < PARTS_NUM; i++)
	{
		m_Obj[i].Calc();
	}
}

void Player::Draw()
{
	for (int i = 0; i < PARTS_NUM; i++)
	{
		m_Obj[i].Draw();
	}
	m_CollsionNodeBullet.Draw();
	m_CollisionNodeBody.Draw();
}

//�e�ۗp�̃p�[�c���ˏo����
void Player::FireBullet()
{
	//���ɔ��˒�
	if (m_FireFlag)return;

	//�e�q�֌W�������������[���h���W���擾
	Matrix worldm = m_Obj[PARTS_ARN_R].GetLocalWorld();

	Vector3 scale;
	Quaternion rotq;
	Vector3 pos;
	//�s��̂P�s����Vector3�Ƃ��Ĉ���
	Vector3* m0 = (Vector3*)&worldm.m[0];
	Vector3* m1 = (Vector3*)&worldm.m[1];
	Vector3* m2 = (Vector3*)&worldm.m[2];
	Vector3* m3 = (Vector3*)&worldm.m[3];
	//���[���h���W�����o��
	pos = *m3;
	//���[���h�X�P�[�����O�����o��
	scale = Vector3(m0->Length(), m1->Length(), m2->Length());
	//�X�P�[�����O��ł�����
	//�@������ɂ��A�����3x3�s�񂪁A��]�݂̂̍s��ɂȂ�
	m0->Normalize();
	m1->Normalize();
	m2->Normalize();
	//���[���h��]�����o��
	rotq = Quaternion::CreateFromRotationMatrix(worldm);
	
	//�e�q�֌W����������
	m_Obj[PARTS_ARN_R].SetParentMatrix(nullptr);
	m_Obj[PARTS_ARN_R].SetScale(scale);
	m_Obj[PARTS_ARN_R].SetRotQ(rotq);
	m_Obj[PARTS_ARN_R].SetTrans(pos);

	//���˂���e�ۂ̑��x�x�N�g��
	m_BulletVel = Vector3(0, 0.1f, 0);

	//�x�N�g�����N�H�[�^�j�I���ŉ�]
	m_BulletVel = Vector3::Transform(m_BulletVel, rotq);

	m_FireFlag = true;
}

//�e�ۗp�̃p�[�c�����{�b�g�Ɏ��t���Ȃ���
void Player::ResetBullet()
{
	//���˒��ł͂Ȃ�
	if (!m_FireFlag)		return;

	m_Obj[PARTS_ARN_R].SetParentMatrix(&m_Obj[PARTS_HEAD].GetLocalWorld());

	m_Obj[PARTS_ARN_R].SetScale(Vector3(1, 1, 1));
	m_Obj[PARTS_ARN_R].SetRot(Vector3(0, 0, 0));
	m_Obj[PARTS_ARN_R].SetTrans(Vector3(0.0f, 0.0f, 0.0f));

	m_FireFlag = false;
}

float GetShortAngleRad();

const DirectX::SimpleMath::Vector3& Player::GetTrans()
{
	return m_Obj[PARTS_TANK].GetTrans();
}

void Player::SetTrans(const DirectX::SimpleMath::Vector3& trans)
{
	m_Obj[PARTS_TANK].SetTrans(trans);
}

const DirectX::SimpleMath::Matrix& Player::GetLocalWorld()
{
	return m_Obj[PARTS_TANK].GetLocalWorld();
}

const DirectX::SimpleMath::Vector3 Player::GetRot()
{
	return m_Obj[PARTS_TANK].GetRot();
}
const DirectX::SimpleMath::Vector3& Player::GetVelocity()
{
	return m_Velocity;
}

// �W�����v���J�n����
void Player::StartJump() 
{
	// �W�����v���łȂ���
	if (!m_isJump)
	{
		// ������̏�����ݒ�
		m_Velocity.y = JUMP_SPEED_FIRST;
		// �W�����v�t���O�𗧂Ă�
		m_isJump = true;
	}
}

// �������J�n����
void Player::StartFall() 
{
	// �W�����v���łȂ���
	if (!m_isJump)
	{
		// ������̏�����0
		m_Velocity.y = 0.0f;
		// �W�����v�t���O�𗧂Ă�
		m_isJump = true;
	}
}

// �W�����v���I������
void Player::StopJump() {
	// �W�����v�t���O�����낷
	m_isJump = false;
	// �f�t�H���g�l�ŏ�����
	m_Velocity = Vector3();
}


