#include<fstream>
#include<algorithm>
#include "LandShape.h"
#include"CollisionNode.h"
#include"Collision.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

//�ÓI�����o�ϐ��̎���
std::unique_ptr<LandShapeCommon>LandShape::s_pCommon;
std::map<std::wstring, std::unique_ptr<LandShapeData>> LandShape::s_dataarray;

LandShapeCommon::LandShapeCommon(LandShapeCommonDef def)
{
	//�J����
	m_pCamera = def.pCamera;
	//�`��X�e�[�g
	m_pStates.reset(new CommonStates(def.pDevice));
	//�G�t�F�N�g�t�@�N�g��
	m_pEffectFactory.reset(new EffectFactory(def.pDevice));
	//�v���~�e�B�u�o�b�`
	m_pPrimitiveBatch.reset(new PrimitiveBatch<VertexPositionNormal>(def.pDeviceContext));
	//�G�t�F�N�g
	m_pEffect.reset(new BasicEffect(def.pDevice));
	m_pEffect->SetLightingEnabled(true);
	m_pEffect->SetAmbientLightColor(Vector3(0,0.0f,0));
	m_pEffect->SetDiffuseColor(Vector3(1.0f, 1.0f, 1.0f));
	m_pEffect->SetLightEnabled(0, true);
	m_pEffect->SetLightDiffuseColor(0, Vector3(0.2f, 1.0f, 0.2f));
	m_pEffect->SetLightDirection(0, Vector3(1, -0.5f, 2));
	m_pEffect->SetLightEnabled(1, true);
	m_pEffect->SetLightDiffuseColor(1, Vector3(0.5f, 0.2f, 0.3f));
	m_pEffect->SetLightDirection(1, Vector3(-1, -0.5f, -2));

	void const* shaderByteConde;
	size_t byteCodeLength;

	//�V�F�[�_�[�̎擾
	m_pEffect->GetVertexShaderBytecode(&shaderByteConde, &byteCodeLength);

	//���̓��C�A�E�g�̍쐬
	def.pDevice->CreateInputLayout(VertexPositionNormal::InputElements, VertexPositionNormal::InputElementCount, shaderByteConde, byteCodeLength,&m_pInputLayout);
	//�f�o�C�X�R���e�L�X�g
	m_pDeviceContext = def.pDeviceContext;
}

LandShapeCommon::~LandShapeCommon()
{
	//���̓��C�A�E�g�̉��
	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = nullptr;
	}
}

void LandShape::InitializeCommon(LandShapeCommonDef def)
{
	//���ɏ������ς�
	if (s_pCommon) return;

	//���ʃf�[�^����
	s_pCommon.reset(new LandShapeCommon(def));
}

//�R���X�g���N�^
LandShape::LandShape() :m_pData(nullptr)
{

}
//������
void LandShape::Initialize(const wchar_t* filename_mdl, const wchar_t* filename_cmo)
{
	if (filename_mdl)
	{
		std::map<std::wstring, std::unique_ptr<LandShapeData>>::iterator it;
		it = s_dataarray.find(filename_mdl);
		if (s_dataarray.count(filename_mdl) == 0)
		{
			//���f���̓ǂݍ���
			s_dataarray[filename_mdl] = LandShapeData::CreateFromMDL(filename_mdl);
		}
		//�n�`�f�[�^���Z�b�g
		m_pData = s_dataarray[filename_mdl].get();
	}
	if (filename_cmo)
	{
		m_Obj.LoadModelFile(filename_cmo);
	}
}

//���[���h�s��̌v�Z
void LandShape::Calc()
{
	m_Obj.Calc();
	//�t�s����v�Z
	const Matrix& localworld = m_Obj.GetLocalWorld();
	m_WorldLocal = localworld.Invert();
}

//�n�`�f�[�^�̕`��

void LandShape::Draw()
{
	if (CollisionNode::GetDebugVisible() == false)
	{
		//���f���`��
		m_Obj.Draw();
	}
	else if(m_pData)
	{
		 //�f�o�b�O�`��
		const Matrix& view = s_pCommon->m_pCamera->GetViewmat();
		const Matrix& projection = s_pCommon->m_pCamera->GetProjmat();

		//�쐬�����s����G�t�F�N�g�ɃZ�b�g
		s_pCommon->m_pEffect->SetWorld(m_Obj.GetLocalWorld());
		s_pCommon->m_pEffect->SetView(view);
		s_pCommon->m_pEffect->SetProjection(projection);

		//�G�t�F�N�g�̐ݒ�i�e�s���e�N�X�`���Ȃǂ�ݒ肵�Ă���j
		s_pCommon->m_pEffect->Apply(s_pCommon->m_pDeviceContext);
		
		//�[�x�X�e���V���@�X�e�[�g��ݒ肷��
		s_pCommon->m_pDeviceContext->OMSetDepthStencilState(s_pCommon->m_pStates->DepthDefault(), 0);

		//�u�����f�B���O�@�X�e�[�g��ݒ肷��
		s_pCommon->m_pDeviceContext->OMSetBlendState(s_pCommon->m_pStates->NonPremultiplied(), nullptr, 0xFFFFFFFF);

		//���X�^���C�U�@�X�e�[�g��ݒ肷��@���v�����\��
		s_pCommon->m_pDeviceContext->RSSetState(s_pCommon->m_pStates->CullClockwise());

		//�T���v���[�X�e�[�g��ݒ肷��
		auto samplerState = s_pCommon->m_pStates->PointWrap();
		s_pCommon->m_pDeviceContext->PSSetSamplers(0, 1, &samplerState);

		//���̓��C�A�E�g��ݒ肷��
		s_pCommon->m_pDeviceContext->IASetInputLayout(s_pCommon->m_pInputLayout);

		//�`��J�n
		s_pCommon->m_pPrimitiveBatch->Begin();

		const uint16_t* pIndex = &m_pData->m_Indices[0];
		int numIndex = m_pData->m_Indices.size();

		const VertexPositionNormal* pVerTex = (VertexPositionNormal*)&m_pData->m_Vertices[0];
		int numVertex = m_pData->m_Vertices.size();
		
		//�O�p�`�v���~�e�B�u�̕`��
		s_pCommon->m_pPrimitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pIndex, numIndex, pVerTex, numVertex);

		//�`��I��
		s_pCommon->m_pPrimitiveBatch->End();
	}
}

bool LandShape::IntersectSphere(const Sphere& sphere, Vector3* reject)
{
	if (m_pData == nullptr)return false;

	//���_�f�b�N�X�̐���3�Ŋ����āA�O�p�`�̐����v�Z
	int nTri = m_pData->m_Indices.size() / 3;
	//�q�b�g�t���O��������
	bool hit = false;
	//�傫�������ŏ�����
	float distance = 1.0e5;
	Vector3 l_inter;
	Vector3 l_normal;
	Vector3 l_down;
	//�X�P�[�����擾
	float scale = GetScale();

	//�����R�s�[
	Sphere localsphere = sphere;

	//�X�P�[��0�̏ꍇ�A���肵�Ȃ�
	if (scale <= 1.0e-10)return false;

	//���̒��S�_�����[���h���W���烂�f�����W�n�Ɉ�������
	localsphere.Center = Vector3::Transform(sphere.Center, m_WorldLocal);
	//���a�����[���h�����[���h���W�n���烂�f�����W�n�ɕϊ�
	localsphere.radius = sphere.radius / scale;

	for (int i = 0; i < nTri; i++)
	{
		//�O�p�`�̊e���_�̃C���f�b�N�X���擾
		int index0 = m_pData->m_Indices[i * 3];
		int index1 = m_pData->m_Indices[i * 3 + 1];
		int index2 = m_pData->m_Indices[i * 3 + 2];
		//�e���_�̃��[�J�����W���擾
		Vector3 pos0 = m_pData->m_Vertices[index0].Pos;
		Vector3 pos1 = m_pData->m_Vertices[index1].Pos;
		Vector3 pos2 = m_pData->m_Vertices[index2].Pos;

		Triangle tri;

		//3�_����O�p�`���\�z�iTODO:��Ɍv�Z���Ă����j
		ComputeTriangle(pos0, pos1, pos2, &tri);

		float temp_distace;
		Vector3 temp_inter;

		//�O�p�`�Ƌ��̓����蔻��
		if (CheckSphere2Triangle(localsphere,tri,&temp_inter))
		{
			hit = true;
			temp_distace = Vector3::Distance(localsphere.Center, temp_inter);
			if (temp_distace < distance)
			{
				l_inter = temp_inter;
				l_normal = tri.Normal;
				distance = temp_distace;
			}
		}

	}
	if (hit)
	{
		//���������f�����W�n���烏�[���h���W�n�ɕϊ�
		distance *= scale;

		//���[���h�s����擾
		const Matrix& localworld = m_Obj.GetLocalWorld();

		//�r�˃x�N�g���̌v�Z
		if (reject != nullptr)
		{
			//�n�`�̖@�����������f�����W�n���烏�[���h���W�n�ɕϊ�
			*reject = Vector3::TransformNormal(l_normal, localworld);
			reject->Normalize();
			//�߂荞�ݕ��̒����ɐL�΂�
			*reject = (*reject)*(sphere.radius - distance);
		}
	}
	return hit;
}
bool LandShape::IntersectSegment(const Segment& segment, Vector3* inter)
{
	if (m_pData == nullptr) return false;

	// �O�p�`�̐�
	int nTri = m_pData->m_Indices.size() / 3;
	// �q�b�g�t���O��������
	bool hit = false;
	// �傫�������ŏ�����
	float distance = 1.0e5;
	// �p�x����p�ɒn�ʂƂ݂Ȃ��p�x�̌��E�l<�x>
	const float limit_angle = XMConvertToRadians(30.0f);
	Vector3 l_inter;

	// �R�s�[
	Segment localSegment = segment;
	// ���������[���h���W���烂�f�����W�n�Ɉ�������
	localSegment.start = Vector3::Transform(localSegment.start, m_WorldLocal);
	localSegment.end = Vector3::Transform(localSegment.end, m_WorldLocal);
	// �����̕����x�N�g�����擾
	Vector3 segmentNormal = localSegment.end - localSegment.start;
	segmentNormal.Normalize();

	for (int i = 0; i < nTri; i++)
	{
		// �O�p�`�̊e���_�̃C���f�b�N�X���擾
		int index0 = m_pData->m_Indices[i * 3];
		int index1 = m_pData->m_Indices[i * 3 + 1];
		int index2 = m_pData->m_Indices[i * 3 + 2];
		// �e���_�̃��[�J�����W���擾
		Vector3 pos0 = m_pData->m_Vertices[index0].Pos;
		Vector3 pos1 = m_pData->m_Vertices[index1].Pos;
		Vector3 pos2 = m_pData->m_Vertices[index2].Pos;

		Triangle tri;

		// 3�_����O�p�`���\�z�iTODO:��Ɍv�Z���Ă����j
		ComputeTriangle(pos0, pos1, pos2, &tri);

		float temp_distance;
		Vector3 temp_inter;

		// ������x�N�g���Ɩ@���̓���
		float dot = -segmentNormal.Dot(tri.Normal);
		// ������Ƃ̊p�x�����v�Z
		float angle = acosf(dot);
		// ������Ƃ̊p�x�����E�p���傫����΁A�n�ʂƂ݂Ȃ����A�X�L�b�v
		if (angle > limit_angle) continue;

		// �����ƎO�p�`�i�|���S���j�̌�������
		if (CheckSegment2Triangle(localSegment, tri, &temp_inter))
		{
			hit = true;
			// �Փ˓_�܂ł̋������Z�o
			temp_distance = Vector3::Distance(localSegment.start, temp_inter);
			// ��������ԋ߂����̂��L�^
			if (temp_distance < distance)
			{
				l_inter = temp_inter;
				distance = temp_distance;
			}
		}
	}

	if (hit && inter != nullptr)
	{
		// ��_�����f�����W�n���烏�[���h���W�n�ɕϊ�
		const Matrix& localworld = m_Obj.GetLocalWorld();
		*inter = Vector3::Transform(l_inter, localworld);
	}

	return hit;
}
