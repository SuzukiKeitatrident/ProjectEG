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
	//�f�o�C�X
	ID3D11Device* pDevice;
	//�f�o�C�X�R���e�L�X�g
	ID3D11DeviceContext* pDeviceContext;
	//�J����
	Camera* pCamera;
};

//���L�f�[�^
class LandShapeCommon
{
	friend class LandShape;
public:
	//�R���X�g���N�^
	LandShapeCommon(LandShapeCommonDef def);
	//�f�X�g���N�^
	~LandShapeCommon();
protected:
	//���ʃJ����
	Camera* m_pCamera;
	//�R�����X�e�[�g�ւ̃|�C���^
	std::unique_ptr<DirectX::CommonStates> m_pStates;
	//�G�t�F�N�g�t�@�N�g��
	std::unique_ptr<DirectX::EffectFactory> m_pEffectFactory;
	//�x�[�V�b�N�G�t�F�N�g�ւ̃|�C���^
	std::unique_ptr<DirectX::BasicEffect> m_pEffect;
	//�v���~�e�B�u�o�b�`�ւ̃|�C���^
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionNormal>> m_pPrimitiveBatch;
	//���̓��C�A�E�g�ւ̃|�C���^
	ID3D11InputLayout* m_pInputLayout;
	//�f�o�C�X�R���e�L�X�g�ւ̃|�C���^
	ID3D11DeviceContext* m_pDeviceContext;

};

class LandShape
{
public:
	//�G�t�F�N�g�t�@�N�g������
	static void InitializeCommon(LandShapeCommonDef def);

	//�R���X�g���N�^
	LandShape();
	//������
	void Initialize(const wchar_t* filename_mdl, const wchar_t*filename_cmo = nullptr);
	//���[���h�s��̌v�Z
	void Calc();
	//�f�o�b�O�`��
	void Draw();


	//�A�N�Z�b�T
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
	//���ʃf�[�^
	static std::unique_ptr<LandShapeCommon> s_pCommon;
	//�n�`������f�[�^�}�b�v
	static std::map<std::wstring, std::unique_ptr<LandShapeData>> s_dataarray;

	//�\���I�u�W�F�N�g
	Obj3D m_Obj;
	//�n�`������f�[�^
	const LandShapeData* m_pData;
	//���[���h�����f���s��
	DirectX::SimpleMath::Matrix m_WorldLocal;

};

