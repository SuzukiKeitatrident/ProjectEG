#include<fstream>
#include<algorithm>
#include "LandShapeData.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

//���f���f�[�^�ǂݍ���
std::unique_ptr<LandShapeData>LandShapeData::CreateFromMDL(const char*meshData)
{
	//�ŐV�̓ǂݍ��݈ʒu�������|�C���^
	const void* head = meshData;

	std::unique_ptr<LandShapeData> landshape(new LandShapeData());

	//�m�[�h����ǂݎ��
	const UINT* p_nNode = static_cast<const UINT*>(head);
	//�ǂݍ��݈ʒu��i�߂�h
	head = p_nNode + 1;
	//�m�[�h���擾
	UINT nNode = *p_nNode;

	//�m�[�h����ǂݎ��
	const NodeInfo* nodeInfo_array = static_cast<const NodeInfo*>(head);
	head = nodeInfo_array + nNode;
	
	int mesh = 0;
	//���b�V���̒��_�f�[�^�ƃC���f�b�N�X�f�[�^��ǂݍ���
	landshape->m_Indices.clear();
	//�m�[�h�������e�m�[�h�̒��_�f�[�^����2�����z��
	vector<vector<VERTX_LANDSHAPE>> vertex_array;
	vector<vector<USHORT>> index_array;
	for (UINT i = 0; i < nNode; i++)
	{
		//�m�[�h���1�����擾
		const NodeInfo* nodeInfo = &nodeInfo_array[i];

		vector<VERTX_LANDSHAPE> va;
		vector<USHORT> ia;

		vertex_array.push_back(va);
		index_array.push_back(ia);

		//���b�V���Ȃ�
		if (nodeInfo->kind == 1)
		{
			//���_�f�[�^����ǂݎ��
			const UINT* p_nVertex = static_cast<const UINT*>(head);;
			head = p_nVertex + 1;
			UINT nVertex = *p_nVertex;
			vertex_array[i].reserve(nVertex);
			size_t vbBytes = sizeof(VERTEX_FILE)* nVertex;
			const VERTEX_FILE* node_vertex_array = static_cast<const VERTEX_FILE*>(head);
			head = node_vertex_array + nVertex;

			for (UINT j = 0; j < nVertex; j++)
			{
				//���_�f�[�^1���̃A�h���X���v�Z
				const VERTEX_FILE* vertex = &node_vertex_array[j];
				//�g���f�[�^�������o��(Normal,UV�͕s�v�j
				VERTX_LANDSHAPE vertex_landshape;
				//�R�s�[
				vertex_landshape.Pos = vertex->Pos;
				vertex_landshape.Normal = vertex->Normal;
				//���_�f�[�^��z��Ɋi�[
				vertex_array[i].push_back(vertex_landshape);
			}

			//�C���f�b�N�X�f�[�^����ǂݎ��
			const UINT* p_nIndices = static_cast<const UINT*>(head);
			head = p_nIndices + 1;
			//�C���f�b�N�X�f�[�^��
			UINT nIndices = *p_nIndices;
			index_array[i].reserve(nIndices);

			size_t ibBytes = sizeof(USHORT)* nIndices;
			//�C���f�b�N�X�f�[�^��ǂݎ��
			const USHORT* node_inde_array = static_cast<const USHORT*>(head);
			head = node_inde_array + nIndices;
			
			for (UINT j = 0; j < nIndices; j++)
			{
				//�C���f�b�N�X1���̃A�h���X���擾
				const USHORT* index = &node_inde_array[j];
				//�C���f�b�N�X�����Ɋi�[
				index_array[i].push_back(*index);
			}
		}
	}

	//���ׂĂ̒��_�f�[�^�����f�����W�n�ɕϊ�
	UpdateNodeMatrices(0, nodeInfo_array, nullptr, vertex_array);

	//�m�[�h�̒��_�f�[�^��z��Ɋi�[
	landshape->m_Vertices.clear();
	landshape->m_Indices.clear();
	int vertex_count = 0;
	for (UINT i = 0; i < nNode; i++)
	{
		vector<USHORT>::iterator it;

		//�C���f�b�N�X��S�Ẵm�[�h�̒ʂ��ԍ��ɕϊ�����
		for (it = index_array[i].begin(); it != index_array[i].end(); it++)
		{
			*it += vertex_count;
		}
		
		vertex_count += vertex_array[i].size();

		//�Ō���ɘA������
		landshape->m_Vertices.insert(landshape->m_Vertices.end(), vertex_array[i].begin(), vertex_array[i].end());
		landshape->m_Indices.insert(landshape->m_Indices.end(), index_array[i].begin(), index_array[i].end());
	}
	return landshape;
}

//���f���f�[�^�ǂݍ���
std::unique_ptr<LandShapeData>LandShapeData::CreateFromMDL(const wchar_t*szFileName)
{
	ifstream ifs;
	std::unique_ptr<char[]>data;

	//�t�@�C���I�[�v��
	ifs.open(szFileName, ios::in | ios::binary);

	//�ǂݍ��ݎ��s
	assert(ifs && "CreateFormMDL Error: Failed to open MDL.");

	//�t�@�C���T�C�Y���擾
	ifs.seekg(0, fstream::end);
	streamoff eofPos = ifs.tellg();
	ifs.clear();
	ifs.seekg(0, fstream::beg);
	streamoff begPos = ifs.tellg();
	streamoff size = eofPos - begPos;

	//�ǂݍ��ނ��߂̃��������m�ہi�G���[�`�F�b�N�͂��Ă��Ȃ��j
	data.reset(new char[(UINT)size]);

	//�t�@�C���擪����o�b�t�@�փR�s�[
	ifs.read(data.get(), size);

	//�t�@�C���N���[�Y
	ifs.close();

	OutputDebugString(szFileName);
	OutputDebugString(L"\n");

	//�ǂݍ��񂾃f�[�^���烂�f���f�[�^�𐶐�
	auto model = CreateFromMDL(data.get());

	//���O�擾
	model->name = szFileName;
	
	return model;
}

void LandShapeData::UpdateNodeMatrices(int index, const NodeInfo* nodeInfo_array, Matrix* pParentMatrix, vector<vector<VERTX_LANDSHAPE>>&vertexArray)
{
	//�m�[�h���1�����擾
	const NodeInfo* nodeInfo = &nodeInfo_array[index];

	//���f���s����쐬
	Matrix modelm;
	//��]�s��
	Matrix rotm = Matrix::CreateFromQuaternion(nodeInfo->rotation);
	//���s�ړ��s��
	Matrix transm = Matrix::CreateTranslation(nodeInfo->translation);

	//�s�������
	modelm *= rotm;
	modelm *= transm;

	//�e�s�񂪂���΁A������
	if (pParentMatrix != nullptr)
	{
		modelm = modelm*(*pParentMatrix);
	}

	//���b�V���Ȃ�
	if (nodeInfo->kind == 1)
	{
		vector<VERTX_LANDSHAPE>::iterator it;

		for (it = vertexArray[index].begin(); it != vertexArray[index].end(); it++)
		{
			//���W��ϊ��i�m�[�h���W�n�����f�����W�n�j
			it->Pos = Vector3::Transform(it->Pos, modelm);
			it->Normal = Vector3::TransformNormal(it->Normal, modelm);

			char str[256];

			//�ϊ���̒��_���W�����O�o��
			sprintf_s(str, "(%.3f,%.3f,%.3f)\n", it->Pos.x, it->Pos.y, it->Pos.z);
			//�ϊ���̖@�������O�o��
			//sprintf_s(str, "(%.3f,%.3f,%.3f)\n", it->Normal.x, it->Normal.y, it->Normal.z);

			OutputDebugStringA(str);
		}
	}
	
	//�Z��m�[�h�́A�����Ƌ��ʂ̐e�s��ŕϊ�
	if (nodeInfo->sibling > 0)
	{
		//�ċA�Ăяo��
		UpdateNodeMatrices(nodeInfo->sibling, nodeInfo_array, pParentMatrix, vertexArray);
	}

	//�q�m�[�h�͎�����e�s��Ƃ��ĕϊ�
	if (nodeInfo->child > 0)
	{
		//�ċA�Ăяo��
		UpdateNodeMatrices(nodeInfo->child, nodeInfo_array, &modelm, vertexArray);

	}
}