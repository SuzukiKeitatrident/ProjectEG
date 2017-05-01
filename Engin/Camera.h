#pragma once
#include <d3d11.h>
#include<SimpleMath.h>
class Camera
{
private:
	//�r���[�s��
	DirectX::SimpleMath::Matrix m_Viewmat;
	//�J�������W
	DirectX::SimpleMath::Vector3 m_Eyepos;
	//�^�[�Q�b�g���W
	DirectX::SimpleMath::Vector3 m_Refpos;
	//������x�N�g��
	DirectX::SimpleMath::Vector3 m_Upvec;
	
	//�v���W�F�N�V�����s��
	DirectX::SimpleMath::Matrix m_Projmat;
	//�c��������p
	float m_FoY;
	//�A�X�y�N�g��
	float m_Aspect;
	//�j�A�N���b�v
	float m_NearClip;
	//�t�@�[�N���b�v
	float m_FarClip;
public:
	//�R���X�g���N�^
	Camera();
	//�f�X�g���N�^
	virtual ~Camera();
	//�X�V
	void Update();
	//�A�N�Z�b�T
	void SetEyepos(const DirectX::SimpleMath::Vector3 pos) { m_Eyepos = pos; }
	void SetRefpos(const DirectX::SimpleMath::Vector3 pos) { m_Refpos = pos; }
	void SetUpvec(const DirectX::SimpleMath::Vector3 pos) { m_Upvec = pos; }
	const DirectX::SimpleMath::Vector3& GetEyepos()const { return m_Eyepos; }
	const DirectX::SimpleMath::Vector3& GetRefpos()const { return m_Refpos; }
	const DirectX::SimpleMath::Vector3& GetUpvec()const { return m_Upvec; }
	const DirectX::SimpleMath::Matrix& GetViewmat()const { return m_Viewmat; }
	const DirectX::SimpleMath::Matrix& GetProjmat()const { return m_Projmat; }


};