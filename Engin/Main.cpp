//--------------------------------------------------------------------------------------
// File: Main.cpp
//
// ���C��
//
// ���l�F���̃v���O������DirectX11���g�p���邽�߂̍Œ�K�v�Ǝv���鏈����g�ݍ��񂾃T���v���ł�
//       �]���ȏ����͋L�q���Ă��܂���̂ŃI���W�i���t���[�����[�N�̃x�[�X�Ɏg�p���Ă�������
//
// Date: 2015.8.27
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#include <Windows.h>
#include <DirectXColors.h>
#include <d3d11.h>
#include "Direct3D.h"
#include "DirectXTK.h"
#include "ImaseLib\FPSTimer.h"

#include <SimpleMath.h>

#include "Grid.h"
#include "DebugCamera.h"

#include<Model.h>
#include"Camera.h"
#include"Obj3D.h"
#include"Player.h"
#include"Enmey.h"
#include"Collision.h"
#include"CollisionNode.h"
#include"LandShapeData.h"
#include"LandShape.h"
#include<vector>

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DirectX::SimpleMath;

//////////////////
// �萔�錾		//
//////////////////
// �E�C���h�E�X�^�C��
static const DWORD WINDOW_STYLE = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

// ��ʉ𑜓x
static const int WINDOW_W = 640;
static const int WINDOW_H = 480;

//////////////////////////////
// �֐��̃v���g�^�C�v�錾	//
//////////////////////////////
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////
// �O���[�o���ϐ�	//
//////////////////////
HINSTANCE g_hInst = nullptr;	// �C���X�^���X�n���h��
HWND g_hWnd = nullptr;	// �E�C���h�E�n���h��

//�v���~�e�B�u�o�b�`
std::unique_ptr<PrimitiveBatch<VertexPositionNormal>>g_pPrimitiveBatch;

//�|���S���\���@�G�t�F�N�g
std::unique_ptr<DirectX::BasicEffect>g_pPolygonEffect;

//�n�`�f�[�^�̃|�C���^�z��
std::vector<LandShape*>	landShapeArray;

//�|���S���\���@���̓��C�A�E�g
ComPtr<ID3D11InputLayout> g_pPolygonInputLayout;


//�͈͎w�藐��
float RandomRange(float min, float max)
{
	return static_cast<float>(rand()) / RAND_MAX*(max - min) + min;
}


//���_�C���f�b�N�X
uint16_t indices[] =
{
	//0,1,2,1,3,2
	0,1,2,/*0,*/
	1,3,2
};

//���_���W
VertexPositionNormal vertices[] =
{//
	{ Vector3(-1.0f,+1.0f,0.0f),Vector3(0.0f,0.0f,+1.0f) }, //0
	{ Vector3(-1.0f,-1.0f,0.0f),Vector3(0.0f,0.0f,+1.0f) }, //1
	{ Vector3(+1.0f,+1.0f,0.0f),Vector3(0.0f,0.0f,+1.0f) }, //2
	{ Vector3(+1.0f,-1.0f,0.0f),Vector3(0.0f,0.0f,+1.0f) }, //3

};

//�|���S��������
void PolygonInit()
{

	//�v���~�e�B�u�o�b�`���쐬
	g_pPrimitiveBatch.reset(new PrimitiveBatch<VertexPositionNormal>(g_pImmediateContext.Get()));
	//�|���S���`��p�̃G�t�F�N�g���쐬
	g_pPolygonEffect.reset(new BasicEffect(g_pd3dDevice.Get()));
	//���C�h�v�Z��L����
	g_pPolygonEffect->SetLightingEnabled(true);
	//�����̐F��ݒ�
	g_pPolygonEffect->SetAmbientLightColor(Vector3(0.2f, 0.2f, 0.2f));
	//�g�U���ˌ��̑f�ސF��ݒ�
	g_pPolygonEffect->SetDiffuseColor(Vector3(1.0f, 1.0f, 1.0f));
	//���C�g0�Ԃ�L����
	g_pPolygonEffect->SetLightEnabled(0, true);
	//���C�g0�Ԃ̐F��ݒ�
	g_pPolygonEffect->SetLightDiffuseColor(0, Vector3(0.2f, 1.0f, 0.2f));
	//���C�g0�Ԃ̌�����ݒ�
	g_pPolygonEffect->SetLightDirection(0, Vector3(1.0f, -0.5f, 2.0f));
	//���C�g1�Ԃ�L����
	g_pPolygonEffect->SetLightEnabled(1, true);
	//���C�g1�Ԃ̐F��ݒ�
	g_pPolygonEffect->SetLightDiffuseColor(1, Vector3(0.5f, 0.2f, 0.3f));
	//���C�g�Ԃ̌�����ݒ�
	g_pPolygonEffect->SetLightDirection(1, Vector3(-1.0f, -0.5f, -2.0f));

	void const* shaderByteCode;
	size_t byteCodeLength;
	//�V�F�[�_�[�̎擾
	g_pPolygonEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	//���̓��C�A�E�g�̍쐬
	g_pd3dDevice.Get()->CreateInputLayout(VertexPositionNormal::InputElements,
		VertexPositionNormal::InputElementCount, shaderByteCode, byteCodeLength,
		g_pPolygonInputLayout.GetAddressOf());

}

//�|���S���J��
void PolygonTerm()
{
	//���̓��C�A�E�g�̊J��
	g_pPolygonInputLayout.Reset();
	//�G�t�F�N�g�̊J��
	g_pPolygonEffect.reset();
	//�v���~�e�B�u�o�b�`�̉��
	g_pPrimitiveBatch.reset();
}

//�|���S���`��
void PolygonDraw(Matrix view, Matrix proj)
{
	static float angle = 0;
	angle += 6.0f;
	//���[���h�s��
	Matrix world;

	//�X�P�[�����O�s��
	Matrix scalemat = Matrix::CreateScale(0.5f, 1.0f, 100.0f);

	//��]�AZ�����l���W�A����X����肍���W�A����Y����肎���W�A��
	Matrix rotmatZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(angle));
	Matrix rotmatX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(angle));
	Matrix rotmatY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(angle));
	Matrix rotmatY2 = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(-angle));
	//���s�ړ�(l,m,n)X�����ɂ����[�g��Y������m���[�g��Z������n���[�g���ړ�
	Vector3 pos(1.0f,0,0);

	Matrix transmat = SimpleMath::Matrix::CreateTranslation(sinf(angle),0.0f, cosf(angle));

	//���[���h�s��ɔ��f
	world = world*2.5f*rotmatY*transmat*rotmatY2;
	
	//���[���h�s���ݒ�
	g_pPolygonEffect->SetWorld(world);
	//�r���[�s���ݒ�
	g_pPolygonEffect->SetView(view);
	//�v���W�F�N�V�����s���ݒ�
	g_pPolygonEffect->SetProjection(proj);
	//�G�t�F�N�g�̐ݒ�i�e�s���e�N�X�`���Ȃǂ𔽉f�j
	g_pPolygonEffect->Apply(g_pImmediateContext.Get());

	//�[�x�X�e���V���X�e�[�g��ݒ�
	g_pImmediateContext->OMSetDepthStencilState(g_state->DepthDefault(), 0);
	//�u�����h�X�e�[�g��ݒ�
	g_pImmediateContext->OMSetBlendState(g_state->NonPremultiplied(), nullptr, 0xFFFFFFF);

	//g_pImmediateContext->RSSetState(g_state->CullClockwise());
	//���X�^���C�U�X�e�[�g��ݒ�i���v�����\���j
	//g_pImmediateContext->RSSetState(g_state->Wireframe());
	g_pImmediateContext->RSSetState(g_state->CullClockwise());
	//���̓��C�A�E�g��ݒ�
	g_pImmediateContext->IASetInputLayout(g_pPolygonInputLayout.Get());

	//�`��J�n
	g_pPrimitiveBatch->Begin();
	//���_���W��n���ĕ`��
	g_pPrimitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices, 6, vertices, 4);
	//g_pPrimitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, indices, 7, vertices, 4);
	//�`��I��
	g_pPrimitiveBatch->End();
}

void ModelDraw(Matrix view, Matrix proj)
{
	EffectFactory*factory = new EffectFactory(g_pd3dDevice.Get());
	//�e�N�X�`���̓ǂݍ��݃p�X���w��
	factory->SetDirectory(L"Resources/cModels");
	std::unique_ptr<Model>model = Model::CreateFromCMO(g_pd3dDevice.Get(), L"Resources/cModels/textbox.cmo", *factory);

}


//--------------------------------------------------------------------------------------
// ���C��
//--------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	//�J����
	Camera camera;

	// 1�b�Ԃ�60�t���[���ŌŒ肷��
	ImaseLib::FPSTimer fpsTimer(60);

	// �E�C���h�E�̍쐬
	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;

	// DirectX�f�o�C�X�̏�����
	if (FAILED(Direct3D_InitDevice(g_hWnd)))
	{
		// �������Ɏ��s
		Direct3D_CleanupDevice();
		
		return 0;
	}

	// DirectXTK�֌W�̏�����
	DirectXTK_Initialize();

	// �O���b�h���I�u�W�F�N�g
	GridFloor gridFloor(20.0f, 20);

	PolygonInit();
	EffectFactory* factory = new EffectFactory(g_pd3dDevice.Get());

	//�e�N�X�`���̓ǂݍ��݃p�X���w��
	factory->SetDirectory(L"Resources/cModels");


	//Obj3D�N���X�̑S�̂̏�����
	{
		//�f�o�C�X�̐ݒ�
		Obj3D::SetDevice(g_pd3dDevice.Get());
		//�f�o�C�X�R���e�L�X�g�̐ݒ�
		Obj3D::SetDeviceContext(g_pImmediateContext.Get());
		//�`��X�e�[�g�̐ݒ�
		Obj3D::SetStates(g_state.get());
		////�G�t�F�N�g�t�@�N�g���̐ݒ�
		Obj3D::SetEffectFactory(factory);
		////�J�����̐ݒ�
		Obj3D::SetCamerra(&camera);
	}

	//�n�`�z�u���ݒ�p�@�\����
	struct LandShapeTable
	{
		Vector3 trans;
		Vector3 rot;
		float scale;
		wchar_t* mdl_name;
		wchar_t* cmo_name;
	};

	//�n�`�z�u�f�[�^�e�[�u���i�z��̗v�f���͎w�肵�Ȃ��j
	static LandShapeTable s_landShapeTable[]=
	{
		//���W				�@��]				�k��	�n�`�t�@�C�����@�\���p���f���t�@�C����
		//{ Vector3(-4,0.002, 4),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(-4,0.002, 0),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo"},
		//{ Vector3(-4,0.002, -4), Vector3(0, 0, 0), 4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(-4,0.002, -8), Vector3(0, 0, 0), 4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(-4,0.002, -12), Vector3(0, 0, 0), 4.0f,L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(-4,0.002, -16), Vector3(0, 0, 0), 4.0f,L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(-4,0.002, -20), Vector3(0, 0, 0), 4.0f,L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(-4,0.002, -24), Vector3(0, 0, 0), 4.0f,L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(0,0.002, -24), Vector3(0, 0, 0), 4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(0,0.002, 8),Vector3(0, 0, 0),   4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//
		//{ Vector3(4,0.002, 8),Vector3(0, 0, 0),   4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(8,0.002, 8),Vector3(0, 0, 0),   4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(12,0.002, 8),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(16,0.002, 8),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(20,0.002, 8),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(24,0.002, 8),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(28,0.002, 8),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(32,0.002, 8),Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },


		//{ Vector3(4,0.002, 4),Vector3(0, 0, 0),  4.0f,   L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(4,0.002, 0), Vector3(0, 0, 0),  4.0f,   L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(4,0.002, -4), Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(4,0.002, -8), Vector3(0, 0, 0),  4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(4,0.002, -12), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(4,0.002, -20), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(4,0.002, -24), Vector3(0, 0, 0), 4.0f,  L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//
		//{ Vector3(8,0.002, 4), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(8,0.002, -28), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(12,0.002, -28), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(16,0.002, -28), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(16,0.002, -24), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{Vector3(8,0.002, -12), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(8,0.002, -20), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(12,0.002, -12), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },	

		//{ Vector3(16,0.002, -12), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{Vector3(16,0.002, -20), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(20,0.002, -20), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(24,0.002, 0), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(24,0.002, -4), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(24,0.002, -8), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(24,0.002, -12), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(24,0.002, -20), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(28,0.002, 0), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(28,0.002, -20), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(32,0.002, 0), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(32,0.002, -4), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(32,0.002, -8), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(32,0.002, -12), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(32,0.002, -16), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(32,0.002, -20), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(16,0.002, -4), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },

		//{ Vector3(12,0.002, -4), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(16,0.002, 0), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(20,0.002, 0), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(24,0.002, 0), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//{ Vector3(28,0.002, 0), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/ipo.MDL", L"Resources/cModels/ipo.cmo" },
		//
		////{ Vector3(32,0.002, 4), Vector3(0, 0, 0),  4.0f, L"Resources/LandShape/goal.MDL", L"Resources/cModels/goal.cmo" },

		//��
		{ Vector3(0,0.002,-0.1),Vector3(0, 0, 0),  100.0f, L"Resources/LandShape/yuka.MDL", L"Resources/cModels/yuka.cmo" },
	};

	//�z��̗v�f�����v�Z
	int landshape_table_num = sizeof(s_landShapeTable) / sizeof(LandShapeTable);
	////vector�̗v�f����ύX
	//landShapeArray.resize(landshape_table_num);
	//�e�[�u����1�v�f���S����������
	for (int i = 0; i < landshape_table_num; i++)
	{
		LandShapeTable* ptable = &s_landShapeTable[i];
		//�n�`�f�[�^�𐶐�
		LandShape* landShape = new LandShape;
		//�t�@�C�������󔒂̏ꍇ�̓��f���̓ǂݍ��݂��΂��ݒ�
		wchar_t* mdl_name = nullptr;
		wchar_t* cmo_name = nullptr;
		if (lstrcmpW(ptable->mdl_name, L"") != 0)mdl_name = ptable->mdl_name;
		if (lstrcmpW(ptable->cmo_name, L"") != 0)cmo_name = ptable->cmo_name;

		Vector3 rot = ptable->rot;
		//�x���@���烉�W�A���ɕϊ�
		rot.x = XMConvertToRadians(ptable->rot.x);
		rot.y = XMConvertToRadians(ptable->rot.y);
		rot.z = XMConvertToRadians(ptable->rot.z);

		landShape->Initialize(mdl_name, cmo_name);
		landShape->SetTrans(ptable->trans);
		landShape->SetRot(rot);
		landShape->SetScale(ptable->scale);
		//�z��ɗv�f�Ƃ��Ēǉ�
		landShapeArray.push_back(landShape);
	}

	

	Vector3 pos2[20];
	for (int i = 0; i < 20; i++)
	{
		pos2[i] = Vector3(rand() % 100 - 50, 0, rand() % 100 - 50);
	}

	//Vector3 tank_pos;
	//Obj3D�̐���
	Obj3D* ground = new Obj3D;
	Obj3D* effect = new Obj3D;
	
	//���f���̓ǂݍ���
	ground->LoadModelFile(L"Resources/cModels/ground2002.cmo");
	effect->LoadModelFile(L"Resources/cModels/sander.cmo");
	
	std::unique_ptr<LandShapeData> landshapeData = LandShapeData::CreateFromMDL(L"Resources/LandShape/tail.MDL");

	LandShapeCommonDef def;
	def.pCamera = &camera;
	def.pDevice = g_pd3dDevice.Get();
	def.pDeviceContext = g_pImmediateContext.Get();
	LandShape::InitializeCommon(def);

	//�n�`�f�[�^���쐬�i�w�i���̐������J��Ԃ��j
	LandShape* landshape = new LandShape;
	//ML�t�@�C���ACMO�t�@�C����ǂݍ���ŏ�����
	landshape->Initialize(L"Resources/LandShape/cubebox2.MDL", L"Resources/cModels/cubebox2.cmo");
	//���[���h���W���w��
	landshape->SetTrans(Vector3(0, 0, 0));
	//���[���h��]���w��i�����W�A���j
	landshape->SetRot(Vector3(0, 0, 0));
	//���[���h�X�P�[�����O���w��(X,Y,Z����)
	landshape->SetScale(10.0f);

	//effect->SetTrans(Vector3(0, 50, 0));

	Obj3D* dorm = new Obj3D;
	//���f���̓ǂݍ���
	dorm->LoadModelFile(L"Resources/cModels/SkyDorm.cmo");
	
	Player* player = new Player;
	player->Initialize();
	
	const int ENEMY_NUM = 1;
	Enemy* enemy[ENEMY_NUM];
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		enemy[i] = new Enemy;
		enemy[i]->Initialize();
		enemy[i]->SetTrans(Vector3(8, 0, -24));
	}



	//////�J�v�Z���Ƌ��̂̓����蔻��

	//��A
	Sphere sphereA;
	//A�̒��S���W
	sphereA.Center = Vector3(10.0f, 0, 0);
	//A�̔��a
	sphereA.radius = 5.0f;

	//�J�v�Z��A
	Capsule capsuleA;
	capsuleA.segment.start = Vector3(-10.0f, 0, 0);
	capsuleA.segment.end = Vector3(0.0f, 0, 0);
	capsuleA.radius = 1.0f;

	bool hit = CheckSphere2Capsule(sphereA, capsuleA);
	if (hit)
	{
		OutputDebugString(L"��������!\n");
	}
	else
	{
		OutputDebugString(L"�������ĂȂ�!\n");
	}


	CollisionNode::SetDebugVisible(false);
	int TT = 0;
	// ���C�����[�v
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		int fps = fpsTimer.GetNowFPS();
		wchar_t buf[16];
		swprintf_s(buf, 16, L"fps = %d", fps);


		// ���b�Z�[�W�����Ă��邩���ׂ�
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			// ���b�Z�[�W�����Ă���΃E�C���h�E�v���V�[�W���֑���
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// �L�[���͂�}�E�X���̍X�V
			DirectXTK_UpdateInputState();


			if (g_keyTracker->pressed.D1)
			{
				TT += 1;
				if (TT == 1)
				{
					CollisionNode::SetDebugVisible(true);
				}
				if (TT == 2)
				{
					CollisionNode::SetDebugVisible(false);
					TT *= 0;
				}
			}

			// �f�o�b�O�J�����̍X�V
			camera.Update();

			// �o�b�N�o�b�t�@�̃N���A
			g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView.Get(), DirectX::Colors::MidnightBlue);

			// �[�x�o�b�t�@�̃N���A
			g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			//static float angle = 0;
			//angle += 0.05f;
			//---------- �����ŕ`��R�}���h�𔭍s����@---------//
			g_spriteBatch->Begin();

			//���[���h�s��
			Matrix world = Matrix::Identity;

			// �r���[�s����쐬����
			Matrix view = camera.GetViewmat();

			const float ROT_SPEEDS = 0.03f;
			const float CAMERA_DISTANCE = 5.0f;
			Vector3 nextEyepos, nextRefpos;

			// ���_
			Vector3 cameraV(0.0f, 2.0f, CAMERA_DISTANCE);
			Matrix playerworld = player->GetLocalWorld();
			cameraV = Vector3::TransformNormal(cameraV, playerworld);
			nextEyepos = player->GetTrans() + cameraV;
			camera.SetEyepos(camera.GetEyepos() + (nextEyepos - camera.GetEyepos())*0.05);
			camera.Update();

			// �����_
			Vector3 refV(0.0f, 0.5f, 0.0);
			refV = Vector3::TransformNormal(refV, playerworld);
			nextRefpos = player->GetTrans() + refV;
			//����camera.GetRefpos()
			camera.SetRefpos(camera.GetRefpos() + (nextRefpos - camera.GetRefpos())*0.95);

			//�J������]
			Vector3 upVec(0.0f,1.0f,0.0f);
			upVec = Vector3::TransformNormal(upVec, playerworld);
			//����camera.GetRefpos()
			camera.SetUpvec(upVec);



			//	// �ˉe�s����쐬����
			Matrix proj = camera.GetProjmat();

			//	// �O���b�h���̕\��
			gridFloor.Render(view, proj);


			world = Matrix::Identity;

			ground->Calc();
			dorm->Calc();
	

			player->Update();
			player->Draw();

			for (int i = 0; i < ENEMY_NUM; i++)
			{
				enemy[i]->Update();
				enemy[i]->Draw();
			}

			//�e�ۂƓG�̓����蔻��iPlayer��Enemy��Update��ɍs�����Ɓj
			{
				//�e�ۂ̓����蔻��J�v�Z��
				const Capsule& bulletCapsule = player->GetCollisionNodeBullet();
				//�G�̐�������������
				for (int i = 0; i < ENEMY_NUM; i++)
				{
					//���Ɏ���ł���G�́A�X�L�b�v
					if (enemy[i]->GetDeath()) continue;

					//�G�̓����蔻�苅
					const Sphere& enemySphere = enemy[i]->GetCollisionNodeBody();
					const Sphere& enemySphere2 = enemy[i]->GetCollisionNodeARN();
					const Sphere& enemySphere3 = enemy[i]->GetCollisionNodeBRE();

					//�Փ˓_�̍��W������ϐ�
					Vector3 inter;

					//���ƃJ�v�Z���̏Փ˔���
					if (CheckSphere2Capsule(enemySphere, bulletCapsule, &inter))
					{
						//�e�ۂ����̈ʒu�ɖ߂�
						player->ResetBullet();
						//�G������
						enemy[i]->SetDeath(true);
			
						//effect->SetTrans(inter);

					}

					if (CheckSphere2Capsule(enemySphere2, bulletCapsule, &inter))
					{
						//�e�ۂ����̈ʒu�ɖ߂�
						player->ResetBullet();
						//�G������
						enemy[i]->SetDeath2(true);
					}
					if (CheckSphere2Capsule(enemySphere3, bulletCapsule, &inter))
					{
						//�e�ۂ����̈ʒu�ɖ߂�
						player->ResetBullet();
						//�G������
						enemy[i]->SetDeath3(true);
					}
				}
			}

			//�S�n�`���X�V
			for (std::vector<LandShape*>::iterator it = landShapeArray.begin(); it != landShapeArray.end(); it++)
			{
				LandShape* landShape = *it;
				if (landShape == nullptr)continue;

				landShape->Calc();
			}
			for (std::vector<LandShape*>::iterator it = landShapeArray.begin(); it != landShapeArray.end(); it++)
			{
				LandShape* landShape = *it;
				if (landShape == nullptr)continue;

				landShape->Draw();
			}
			//�ǂ̓����蔻�� ���v���C���[�̍X�V��ɏ������s�����Ɓ@���{���͊֐�������ׂ�
			{
				//�����蔻�苅�̎擾
				Sphere sphere = player->GetCollisionNodeBody();
				//�v���C���[���_�̃��[���h���W���擾
				Vector3 trans = player->GetTrans();
				//���ƃv���C���[���_�̂�����v�Z
				Vector3 sphere2player = trans - sphere.Center;

				//�S�Ă̒n�`�f�[�^�ɂ��ď���
				for (std::vector<LandShape*>::iterator it = landShapeArray.begin();
				it != landShapeArray.end();it++)
				{
					LandShape* pLandShape = *it;

					Vector3 reject;//�r�˃x�N�g�������邽�߂̕ϐ�
					//���ƒn�`�̌�������
					if (pLandShape->IntersectSphere(sphere, &reject))
					{//�������Ă���ꍇ
						//�߂荞�ݕ������A���������o���悤�Ɉړ�
						sphere.Center = sphere.Center + reject;
					}
				}
				//�H�����ݕ␳��̈ʒu����A�v���C���[�̃��[���h���W���v�Z
				player->SetTrans(sphere.Center + sphere2player);
				//�s����Čv�Z
				player->Calc();
			}
				// �n�ʂ̓����蔻��
			{
				// ���@�̑��x�擾
				const Vector3& vel = player->GetVelocity();
				// ��������
				if (vel.y <= 0.0f) {
					bool hit = false;
					Segment player_segment;
					Vector3 player_pos = player->GetTrans();
					// ���@�̓����瑫���܂Ŋт�����
					player_segment.start = player_pos + Vector3(0, 1.0f, 0);
					player_segment.end = player_pos + Vector3(0, -0.5f, 0);

					// �����ۑ��p�@�傫�������ŏ�����
					float distance = 1.0e5;
					Vector3 inter;
					// �S�Ă̒n�`�f�[�^�ɂ��ď���
					for (std::vector<LandShape*>::iterator it = landShapeArray.begin();
					it != landShapeArray.end();
						it++) {
						LandShape* pLandShape = *it;
						float temp_distance;
						Vector3 temp_inter;

						// �����ƒn�`�̌�������
						if (pLandShape->IntersectSegment(player_segment, &temp_inter)) {
							hit = true;
							temp_distance = Vector3::Distance(player_segment.start, temp_inter);				// ���܂łň�ԋ������߂���
							if (temp_distance < distance)
							{
								inter = temp_inter;
								distance = temp_distance;
							}
						}
					}

					if (hit) {
						Vector3 new_position = player_pos;
						new_position.y = inter.y;
						player->StopJump();
						player->SetTrans(new_position);
					}

					if (!hit) {// �����J�n
						player->StartFall();
					}
					// �s��̍Čv�Z
					player->Calc();
				}
			}

		  //   	effect->Calc();
				//effect->Draw();
				ground->Draw();
				dorm->Draw();
				//landshape->Draw();

				if(g_key.K)
				{
					g_spriteFont->DrawString(g_spriteBatch.get(), L"CLEAR", Vector2(0, 0));
				}
				g_spriteBatch->End();

				// ��ʍX�V��҂�
				fpsTimer.WaitFrame();

				// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�����ւ���
				g_pSwapChain->Present(0, 0);
			}
		}
		

		PolygonTerm();
		delete factory;
		// DirectX�f�o�C�X����̏I������
		Direct3D_CleanupDevice();

		return 0;
}

//--------------------------------------------------------------------------------------
// �E�C���h�E�̍쐬
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// �E�C���h�E�N���X��o�^����
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)NULL);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"WindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)NULL);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// �E�C���h�E���쐬����
	g_hInst = hInstance;
	RECT rc = { 0, 0, WINDOW_W, WINDOW_H };	// ���E�C���h�E�T�C�Y
	AdjustWindowRect(&rc, WINDOW_STYLE, FALSE);
	g_hWnd = CreateWindow(L"WindowClass", L"3D�v���O���~���O��b", WINDOW_STYLE,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!g_hWnd)
		return E_FAIL;

	// �E�C���h�E�̕\��
	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

//--------------------------------------------------------------------------------------
// �E�C���h�E�v���V�[�W���[
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:		// �E�C���h�E���j�����ꂽ���b�Z�[�W
		PostQuitMessage(0);	// WM_QUIT���b�Z�[�W�𑗏o����i���C�����[�v�𔲂���j
		break;

	case WM_ACTIVATEAPP:	// �E�C���h�E���A�N�e�B�u���A��A�N�e�B�u�����鎞�ɗ��郁�b�Z�[�W
		Keyboard::ProcessMessage(message, wParam, lParam);
		Mouse::ProcessMessage(message, wParam, lParam);
		break;

	case WM_KEYDOWN:		// �L�[���͊֌W�̃��b�Z�[�W
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	case WM_INPUT:			// �}�E�X�֌W�̃��b�Z�[�W
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(message, wParam, lParam);
		break;

	default:
		// �����ŏ������Ȃ����b�Z�[�W��DefWindowProc�֐��ɔC����
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

