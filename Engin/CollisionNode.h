//----------------------------------------------------------------------
//�t�@�C����:CollisionNode
//�쐬��:
//�쐬��
//�����F�Փ˔���p�m�[�h
//		�C�ӂ̃��[���h�s��ɂԂ炳���A�e�q�֌W�����Ԃ��Ƃ��ł��܂��B
//		�f�o�b�O�p�ɁA�����蔻���\������@�\������܂��B
//----------------------------------------------------------------------
#pragma once

#include"Obj3D.h"
#include"Collision.h"


class CollisionNode
{
protected:
	//�f�o�b�O�\����ON/OFF��ݒ�
	static bool s_DebugVisible;
public:
	//�f�o�b�O�\����ON/OFF��ݒ�
	static void SetDebugVisible(bool flag) { s_DebugVisible = flag; }
	//�f�o�b�O�\����ON/OFF���擾
	static bool GetDebugVisible(void) { return s_DebugVisible; }
};

//�����蔻�苅�m�[�h
class SphereNode : public Sphere
{
public:
	//�R���X�g���N�^
	SphereNode();
	//������
	void Initialize();
	//�`��
	void Draw();
	//�X�V
	void Update();
	//�e�s���ݒ�
	void SetParentMatrix(const DirectX::SimpleMath::Matrix* pParentMatrix);
	//���[�J�����a��ݒ�
	void SetLocalRadius(float radius) { m_LocalRadius = radius; }
	//�e�s�񂩂�̃I�t�Z�b�g��ݒ�
	void SetTrans(const DirectX::SimpleMath::Vector3& trans) { m_Trans = trans; }
protected:
	//�\���p�I�u�W�F�N�g
	Obj3D m_Obj;
	//�e�s��̉e�����󂯂�O�̃��[�J�����a
	float m_LocalRadius;
	//�e�s�񂩂�̃I�t�Z�b�g���W
	DirectX::SimpleMath::Vector3 m_Trans;
};

//�f�o�b�O�\���t���J�v�Z���m�[�h
class CapsuleNode :public Capsule
{
public:
	//�R���X�g���N�^
	CapsuleNode();
	//������
	void Initialize();
	//�`��
	void Draw();
	//�X�V
	void Update();
	//�e�s���ݒ�
	void SetParentMatrix(const DirectX::SimpleMath::Matrix* pParentMatrix);
	//�e�s�񂩂�̉�]��ݒ�
	void SetRot(const DirectX::SimpleMath::Vector3& rot);
	//���[�J�����a��ݒ�
	void SetLocalRadius(float radius) { m_LocalRadius = radius; }
	//���[�J����������ݒ�
	void SetLenngth(float length) { m_LocalLength = length; }
	//�e�s�񂩂�̃I�t�Z�b�g��ݒ�
	void SetTrans(const DirectX::SimpleMath::Vector3& trans) { m_Trans = trans; }

	void GetCollisionNodeBullet();

protected:
	//�\���p�̃I�u�W�F�N�g
	Obj3D m_Obj;
	//�e�s��̉e�����󂯂�O�̃��[�J��������
	float m_LocalLength;
	//�e�s��̉e�����󂯂�O�̃��[�J�����a
	float m_LocalRadius;
	//�e�s�񂩂�̃I�t�Z�b�g���W
	DirectX::SimpleMath::Vector3 m_Trans;
};