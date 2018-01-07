/*=================================================
//								FILE : XFileLoad.cpp
//		�t�@�C������ :
//		X�t�@�C����ǂݍ��݁A�f�[�^�Ƃ��ēǂݎ���Ă���
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "XFileLoad.h"
#include "TextureManager.h"

using namespace DirectX;
using namespace std;

//X�t�@�C����ǂݍ��ލۂɕK�v�ȏ����ꎞ�I�ɒ��߂Ă����\���̂��B���ړI�ŗ��p����
namespace {
	//���_���𒙂߂Ă���
	struct XFileVertex {
		DirectX::XMFLOAT4 vPos;
		DirectX::XMFLOAT4 vNormal;
		DirectX::XMFLOAT2 vUV;
		DirectX::XMUINT4 vBoneNo;
		DirectX::XMFLOAT4 vWeight;

		int vBoneCnt;		//�{�[���̃J�E���g

		XFileVertex() {
			vPos = { 0.0f,0.0f,0.0f,1.0f };
			vNormal = { 0.0f,0.0f,0.0f,0.0f };
			vUV = { 0.0f,0.0f };
			vBoneNo = { 128,128,128,128 };
			vWeight = { 0.0f,0.0f,0.0f,0.0f };
			vBoneCnt = 0;
		}
	};

	struct XFileFace {
		XFileVertex* vVertex;			//���_���(���_�����m�ۂ���)
		DirectX::XMINT3* vIndex;		//���_�C���f�b�N�X(�ʐ����m�ۂ���)
		int vVertexNum;		//���_��
		int vFaceNum;			//�ʐ�

		XFileFace() {
			vVertex = nullptr;
			vIndex = nullptr;
			vVertexNum = 0;
			vFaceNum = 0;
		}

		~XFileFace() {
			delete[] vVertex;
			delete[] vIndex;
		}
	};

	struct XFileMaterial {
		DirectX::XMFLOAT4 vColor;
		DirectX::XMFLOAT4 vSpecular;
		std::string vTexFileName;		//�e�N�X�`���̃t�@�C�����B�Ȃ����"NONE"������
		float vPow;		//���ʔ��˂̋����B�܂��g���ĂȂ����������l���擾���Ă���

		XFileMaterial() {
			vColor = { 0.0f,0.0f,0.0f,1.0f };
			vSpecular = { 0.0f,0.0f,0.0f,1.0f };
			vTexFileName = "NONE";
			vPow = 0;
		}
	};

	//�}�e���A�����𒙂߂Ă���
	struct XFileMaterialList {
		int vMaterilNum;		//�}�e���A������ۑ����Ă���
		int vTotalFaceNum;				//�}�e���A���ʐ�(���ʐ��ƈꏏ�̂��Ƃ���������1�Ƃ��őS���̃|���S���ɂ��̃}�e���A�����w��Ƃ���������ۂ�)
		int* vMaterialNo;		//�ʐ����̗̈���m�ۂ��Ă��̒��ɉ��Ԃ̃}�e���A�����g�����i�[���Ă���
		int* vFaceNum;			//�}�e���A�������̗̈���m�ۂ��Ă��̒��ɖʐ����J�E���g���Ă���
		XFileMaterial* vMaterialData;		//�}�e���A���̃J���[����ێ�

		XFileMaterialList() {
			vMaterilNum = 0;
			vTotalFaceNum = 0;
			vMaterialNo = nullptr;
			vFaceNum = nullptr;
			vMaterialData = nullptr;
		}

		~XFileMaterialList() {
			delete[] vMaterialData;
			delete[] vFaceNum;
		}
	};

	//�A�j���[�V�������𒙂߂Ă���
	struct XFileAnime {

	};


}

struct XFileLoadMeshBuf {
	XFileFace vFace;
	XFileMaterialList vMat;
};

/// <summary>
/// �������Ăяo����X�t�@�C���̃��[�h���s��
/// </summary>
/// <param name="_FileName"></param>
/// <param name="_pBone"></param>
void cXFileLoad::GetXFile(Mesh3D* pMesh, std::string _FileName)
{
	InitData();		//�ǂݍ��ނ��߂ɓǂݎ��ϐ�������������

	//X�t�@�C���Ɋ܂܂������擾����
	m_Type = CheckType(_FileName);
	m_Type &= 2;	//2�̓����X�L����񂪊܂܂�Ă��邩�H

	ifstream pFP(_FileName);		//�ǂݍ��񂾃t�@�C���̏�������
	SearchFrame(&pFP);				//"Frame"��T���ė��Ă��炤

	if (m_Type) {
		Bone _pBone;
		GetFrame(&pFP, &_pBone, "NONE_PARENT");
		GetAnimation(&pFP, pMesh->vpAnimetion);
	}

	XFileLoadMeshBuf Data;
	GetMesh(_FileName, &Data);
	CreateMesh3D(pMesh, &Data);
	int a = 10;
}

void cXFileLoad::InitData()
{
	m_Suffix = 0;
	m_MotionNum = 0;

	//�ő�{�[����������������
	for (int i = 0; i < 128; i++) {
		m_Bones[i].vParentNo = -1;
		m_Bones[i].vName = "UninitializedName";
		XMStoreFloat4x4(&m_Bones[i].vMat, XMMatrixIdentity());		//�P�ʍs������Ă���
		m_Bones[i].vBind = m_Bones[i].vMat;
		m_BoneInMotion[i] = -1;
	}
}

/// <summary>
/// ���b�V�����������Ď擾����
/// </summary>
/// <param name="_FileName"></param>
/// <param name="_output"></param>
void cXFileLoad::GetMesh(std::string _FileName, XFileLoadMeshBuf * _output)
{
	ifstream pFP(_FileName);		//�ǂݍ��񂾃t�@�C���̏�������
	string buf;		//�����������ɓǂ�ł���

	while (buf != "Mesh" && !(pFP.eof())) {
		if (buf == "template") {
			getline(pFP, buf);		//template�͈�s��΂�
		}
		else {
			pFP >> buf;
		}
	}
	getline(pFP, buf);		//Mesh�̖��O�͕K�v�Ȃ��̂ň�s��΂�
	//�܂���"Mesh"�̏��Ƃ��Ē�^�̒��_���A���_���W�A�C���f�b�N�X�����o��
	getline(pFP, buf, ';');		//�����ɓ���̂͒��_��
	_output->vFace.vVertexNum = std::stod(buf);
	_output->vFace.vVertex = new XFileVertex[_output->vFace.vVertexNum];		//���_���̃f�[�^�̈���m��

	//���_�����A�f�[�^���擾���čs���BX�t�@�C���̍Ō��;�Ȃ̂ł����܂ŗ�����I����
	for (int i = 0; buf != ";"; i++) {
		getline(pFP, buf);	//���s���΂�

		getline(pFP, buf, ';');		//X���W
		_output->vFace.vVertex[i].vPos.x = (float)std::stod(buf);

		getline(pFP, buf, ';');		//Y���W
		_output->vFace.vVertex[i].vPos.y = (float)std::stod(buf);

		getline(pFP, buf, ';');		//Z���W
		_output->vFace.vVertex[i].vPos.z = (float)std::stod(buf);

		_output->vFace.vVertex[i].vPos.w = 1.0f;
		pFP >> buf;	//�܂��p�����Ă���","���A�I����Ă�����";"���i�[�����
	}
	getline(pFP, buf);	//���s���΂�

	//���̎��ɗ���͖̂ʐ��̂͂�
	getline(pFP, buf, ';');
	_output->vFace.vFaceNum = std::stod(buf);
	_output->vFace.vIndex = new XMINT3[_output->vFace.vFaceNum];	//�̈�m��
	getline(pFP, buf);	//���s���Ă���
	//�ʐ����A�f�[�^�����[�h
	for (int i = 0; i < _output->vFace.vFaceNum; i++) {
		//�C���f�b�N�X�͏��߂Ƀ|���S���̒��_��������
		getline(pFP, buf, ';');		// TODO 4�p�|���S���Ȃ�G���[���o��

		getline(pFP, buf, ',');		//1�_��
		_output->vFace.vIndex[i].x = std::stod(buf);

		getline(pFP, buf, ',');		//2�_��
		_output->vFace.vIndex[i].y = std::stod(buf);

		getline(pFP, buf, ';');		//3�_�ځB��������';'�ŏI���
		_output->vFace.vIndex[i].z = std::stod(buf);

		getline(pFP, buf);	//���s���Ă���
	}

	//===================�����܂ł�Mesh����K�����o������======================

	//Mesh�̕��J�b�R��������܂ŌJ��Ԃ�����
	while (buf != "}" && !(pFP.eof())) {
		// TODO ����ǂݍ��ނ��̂��������Ƃ��͂������X�V����΂���
		pFP >> buf;		//�^��ǂݍ���

		//���򏈗��B�ǂ̊֐������ɓǂ񂾂當��������悤�ɋL�q���邱��
		if (buf == "MeshNormals") {	//�@��
			getline(pFP, buf);	//���s���Ă���
			GetNormal(&pFP, _output);		//�@�����擾���Ă���
		}
		else if (buf == "MeshTextureCoords") {		//UV
			getline(pFP, buf);	//���s���Ă���
			GetUV(&pFP, _output);		//UV���擾���Ă���
		}
		else if (buf == "MeshMaterialList") {
			getline(pFP, buf);	//���s���Ă���
			GetMaterialList(&pFP, _output);		//�}�e���A�������擾���Ă���
		}
		else if (buf == "XSkinMeshHeader") {
			getline(pFP, buf);	//���s���Ă���
			GetMeshBone(&pFP, _output);
		}
		else {
			SkipScope(&pFP);
		}
	}

	pFP >> buf;
}

/// <summary>
/// �擾�����f�[�^���烁�b�V������g�ݗ��Ă�
/// </summary>
/// <param name="pMesh"></param>
/// <param name=""></param>
void cXFileLoad::CreateMesh3D(Mesh3D * pMesh, XFileLoadMeshBuf *data)
{
	//�܂��͊e�������ɂ��ăf�[�^�̈���m�ۂ���
	pMesh->vTotalFace = data->vFace.vFaceNum;	//�ʐ�
	pMesh->vpVertexData = new VERTEX3D[data->vFace.vVertexNum];
	pMesh->vMaterialCnt = data->vMat.vMaterilNum;		//�}�e���A����
	//�C���f�b�N�X�̍쐬
	pMesh->ppiIndex = new int*[data->vMat.vMaterilNum];	//�}�e���A�������̃C���f�b�N�X
	//�}�e���A���̈�m��
	pMesh->vpMaterial = new Material[data->vMat.vMaterilNum];
	for (int i = 0; i < data->vMat.vMaterilNum; i++) {
		//�e�}�e���A���ɓ����Ă���ʐ��������̈���m��
		//pMesh->ppiIndex[i] = new int[data->vMat.vMaterialNo[i] * 3];		//���_�����Ȃ̂� *3
		pMesh->ppiIndex[i] = new int[data->vMat.vFaceNum[i] * 3];		//���_�����Ȃ̂� *3
		pMesh->vpMaterial[i].dwNumFace = data->vMat.vFaceNum[i];			//�}�e���A�����̃|���S����
	}

	//�܂��͒��_���W�A�@���AUV���i�[����
	for (int i = 0; i < data->vFace.vVertexNum; i++) {
		//���W
		pMesh->vpVertexData[i].vPos.x = data->vFace.vVertex[i].vPos.x;
		pMesh->vpVertexData[i].vPos.y = data->vFace.vVertex[i].vPos.y;
		pMesh->vpVertexData[i].vPos.z = data->vFace.vVertex[i].vPos.z;
		pMesh->vpVertexData[i].vPos.w = data->vFace.vVertex[i].vPos.w;

		//�@��
		pMesh->vpVertexData[i].vNorm.x = data->vFace.vVertex[i].vNormal.x;
		pMesh->vpVertexData[i].vNorm.y = data->vFace.vVertex[i].vNormal.y;
		pMesh->vpVertexData[i].vNorm.z = data->vFace.vVertex[i].vNormal.z;
		pMesh->vpVertexData[i].vNorm.w = data->vFace.vVertex[i].vNormal.w;

		//UV
		pMesh->vpVertexData[i].vTex.x = data->vFace.vVertex[i].vUV.x;
		pMesh->vpVertexData[i].vTex.y = data->vFace.vVertex[i].vUV.y;

		//�{�[���ԍ�
		pMesh->vpVertexData[i].vBones = data->vFace.vVertex[i].vBoneNo;

		//�d��
		pMesh->vpVertexData[i].vWeight = data->vFace.vVertex[i].vWeight;
	}

	//���ɃC���f�b�N�X�����擾���Ă���
	int* VertexCnt = new int[pMesh->vMaterialCnt]{ 0 };
	for (int i = 0; i < data->vMat.vTotalFaceNum; i++) {
		int Index = data->vMat.vMaterialNo[i];		//i�Ԗڂ̖ʂ��ǂ̃}�e���A�����g��������
		if (pMesh->vpMaterial[Index].dwNumFace != 0) {
			pMesh->ppiIndex[Index][VertexCnt[Index]] = data->vFace.vIndex[i].x;
			pMesh->ppiIndex[Index][VertexCnt[Index] + 1] = data->vFace.vIndex[i].y;
			pMesh->ppiIndex[Index][VertexCnt[Index] + 2] = data->vFace.vIndex[i].z;
			VertexCnt[Index] += 3;
		}
	}
	delete[] VertexCnt;


	pMesh->vppIndexBuffer = new ID3D11Buffer*[pMesh->vMaterialCnt];
	for (int i = 0; i < pMesh->vMaterialCnt; i++) {
		//�C���f�b�N�X�o�b�t�@�̍쐬
		//������A�}�e���A�����̓J�E���g����Ă��Ă����̃}�e���A�����g���Ă��Ȃ������ꍇ�̏���
		if (pMesh->vpMaterial[i].dwNumFace == 0) {

		}
		else {
			CreateIndexBuffer((pMesh->vpMaterial[i].dwNumFace * 3) * sizeof(int), pMesh->ppiIndex[i], &pMesh->vppIndexBuffer[i]);
		}
	}

	//�}�e���A������ǂ�ł���
	for (int i = 0; i < pMesh->vMaterialCnt; i++) {
		pMesh->vpMaterial[i].Ka = data->vMat.vMaterialData[i].vColor;
		pMesh->vpMaterial[i].Kd = data->vMat.vMaterialData[i].vColor;
		pMesh->vpMaterial[i].Ks = data->vMat.vMaterialData[i].vSpecular;

		//�e�N�X�`���̓ǂݍ���
		if (data->vMat.vMaterialData[i].vTexFileName != "NONE") {
			std::string ToonFolder[4] = {
				{"Default/"},
				{"Light/"},
				{"Shadow/"},
				{ "Mask/" }
			};

			//�g�D�[���p�e�N�X�`�������ꂼ��ǂݍ���
			std::string texName = pMesh->dir + ToonFolder[0] + data->vMat.vMaterialData[i].vTexFileName;
			pMesh->vpMaterial[i].pDefaultTex = cTexManager::GetInstance().LoadTexData(texName, true);

			texName = pMesh->dir + ToonFolder[1] + data->vMat.vMaterialData[i].vTexFileName;
			pMesh->vpMaterial[i].pLightTex = cTexManager::GetInstance().LoadTexData(texName, true);

			texName = pMesh->dir + ToonFolder[2] + data->vMat.vMaterialData[i].vTexFileName;
			pMesh->vpMaterial[i].pShadowTex = cTexManager::GetInstance().LoadTexData(texName, true);

			texName = pMesh->dir + ToonFolder[3] + data->vMat.vMaterialData[i].vTexFileName;
			pMesh->vpMaterial[i].pMaterialMaskTex = cTexManager::GetInstance().LoadTexData(texName, true);
		}
		else {
			pMesh->vpMaterial[i].pDefaultTex = nullptr;
		}
	}

	//�{�[������n��
	if (m_Suffix > 0) {
		pMesh->vpAnimetion->m_pBoneNum = m_Suffix;		//�{�[�����擾
		pMesh->vpAnimetion->m_pBone = new BoneData[m_Suffix];	//�̈�m��

		for (int i = 0; i < m_Suffix; i++) {
			pMesh->vpAnimetion->m_pBone[i].vInv = m_Bones[i].vMat;
			pMesh->vpAnimetion->m_pBone[i].vOffset = m_Bones[i].vBind;
			pMesh->vpAnimetion->m_pBone[i].vTransPose = m_Bones[i].vMat;
			pMesh->vpAnimetion->m_pBone[i].vName = m_Bones[i].vName;
			pMesh->vpAnimetion->m_pBone[i].vParentNo = m_Bones[i].vParentNo;
		}
	}

	//�Ō�ɒ��_�o�b�t�@���쐬
	//�o�[�e�b�N�X�o�b�t�@�[���쐬
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX3D) * data->vFace.vVertexNum;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pMesh->vpVertexData;
	if (FAILED(GetDirectX::Device()->CreateBuffer(&bd, &InitData, &pMesh->vVertexBuf)))
		return;
}

/// <summary>
/// �@�������擾����
/// </summary>
/// <param name="_pFP"></param>
/// <param name="_output"></param>
void cXFileLoad::GetNormal(std::ifstream * _pFP, XFileLoadMeshBuf * _output)
{
	string buf;	//�������i�[����o�b�t�@

	//�@�����Ƃ��Ė@���̐����ŏ��ɓ����Ă��邪�A
	//���_���Ɠ����ɂȂ�̂Ŗ������ĉ��s���Ă��܂�
	getline(*_pFP, buf);
	int cnt = _output->vFace.vVertexNum;
	if (std::stoi(buf) != _output->vFace.vVertexNum)
		cnt = std::stoi(buf);

	//���_�����f�[�^��ێ�
	for (int i = 0; i < cnt; i++) {
		//X����
		getline(*_pFP, buf, ';');
		_output->vFace.vVertex[i].vNormal.x = (float)std::stod(buf);

		//Y����
		getline(*_pFP, buf, ';');
		_output->vFace.vVertex[i].vNormal.y = (float)std::stod(buf);

		//Z����
		getline(*_pFP, buf, ';');
		_output->vFace.vVertex[i].vNormal.z = (float)std::stod(buf);

		_output->vFace.vVertex[i].vNormal.w = 0.0f;
		getline(*_pFP, buf);
	}

	//�@�����̒��ɂ͖ʂ̖@�����������Ă��邪�A�K�v�͂Ȃ��̂Ŏ���}�܂Ŕ�΂��Ă��܂�
	while (buf != "}")
	{
		*_pFP >> buf;
	}
	getline(*_pFP, buf);		//���s�B����ŏI���
}

/// <summary>
/// UV���W���擾����
/// </summary>
/// <param name="_pFP"></param>
/// <param name="_output"></param>
void cXFileLoad::GetUV(std::ifstream * _pFP, XFileLoadMeshBuf * _output)
{
	string buf;	//�������i�[����o�b�t�@

	//UV���̒���UV���������Ă��邪���_���Ɠ����ɂȂ�̂Ŕ�΂��Ă��܂�
	getline(*_pFP, buf);

	//���_���������f�[�^���擾���Ă���
	for (int i = 0; i < _output->vFace.vVertexNum; i++) {
		//U����
		getline(*_pFP, buf, ';');
		_output->vFace.vVertex[i].vUV.x = (float)std::stod(buf);

		//V����
		getline(*_pFP, buf, ';');
		_output->vFace.vVertex[i].vUV.y = (float)std::stod(buf);

		getline(*_pFP, buf); //���s
	}
	getline(*_pFP, buf); //���s�B����ŏI���
}

/// <summary>
/// �}�e���A���Ɋւ�������擾����
/// </summary>
/// <param name="_pFP"></param>
/// <param name="_output"></param>
void cXFileLoad::GetMaterialList(std::ifstream * _pFP, XFileLoadMeshBuf * _output)
{
	string buf;

	//�}�e���A���ɂ͏��߂Ƀ}�e���A����,���Ƀ}�e���A����K�p����ʐ����L�q����Ă���
	getline(*_pFP, buf, ';');
	_output->vMat.vMaterilNum = std::stoi(buf);
	getline(*_pFP, buf);		//���s

	//�e�}�e���A���ɉ����̖ʂ������Ă��邩���J�E���g����
	_output->vMat.vFaceNum = new int[_output->vMat.vMaterilNum];
	for (int i = 0; i < _output->vMat.vMaterilNum; i++) {
		//������
		_output->vMat.vFaceNum[i] = 0;
	}
	//�}�e���A�������i�[����
	_output->vMat.vMaterialData = new XFileMaterial[_output->vMat.vMaterilNum];


	getline(*_pFP, buf, ';');		//�����ɂ͖ʐ�������
	_output->vMat.vTotalFaceNum = std::stoi(buf);
	//���̖ʔԍ��ɉ��Ԃ̃}�e���A����K�p���邩���c���Ă���
	_output->vMat.vMaterialNo = new int[_output->vMat.vTotalFaceNum];
	getline(*_pFP, buf);		//���s

	//�ʐ����A�J��Ԃ�����
	for (int i = 0; i < _output->vMat.vTotalFaceNum - 1; i++) {
		getline(*_pFP, buf, ',');		//���s
		int MateNo = stoi(buf);		//�}�e���A���ԍ�
		_output->vMat.vFaceNum[MateNo]++;		//�ʐ����J�E���g
		_output->vMat.vMaterialNo[i] = MateNo;	//�ǂ̃}�e���A��No���g�����i�[
		getline(*_pFP, buf);		//���s
	}
	//�}�e���A���̍Ō��";"�Ȃ̂ōŌ�̈�񂾂�������ς���
	getline(*_pFP, buf, ';');		//���s
	int MateNo = stoi(buf);		//�}�e���A���ԍ�
	_output->vMat.vFaceNum[MateNo]++;		//�ʐ����J�E���g
	_output->vMat.vMaterialNo[_output->vMat.vTotalFaceNum - 1] = MateNo;	//�ǂ̃}�e���A��No���g�����i�[
	getline(*_pFP, buf);		//���s


	//�����܂ł��}�e���A�����X�g�̒�^�擾�ňȍ~�̓}�e���A���f�[�^�̎擾�ƂȂ�
	//�}�e���A�����������擾
	for (int i = 0; i < _output->vMat.vMaterilNum; i++) {
		*_pFP >> buf;	//GetLine�������Ɖ��s�������擾����\��������̂Ő�Ɋm���Ɍ^�����擾����
		getline(*_pFP, buf);		//�ŏ���Material�Ƃ����^���͔�΂�

		//�ŏ��ɗ���͖̂ʂ̐F�f�[�^
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vColor.x = stof(buf);
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vColor.y = stof(buf);
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vColor.z = stof(buf);
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vColor.w = stof(buf);
		getline(*_pFP, buf);		//���s

		//���ɂ���̂͋��ʔ��˂̋���
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vPow = stof(buf);
		getline(*_pFP, buf);		//���s

		//�������ʔ��ːF
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vSpecular.x = stof(buf);
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vSpecular.y = stof(buf);
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vSpecular.z = stof(buf);

		_output->vMat.vMaterialData[i].vSpecular.w = 1.0f;
		getline(*_pFP, buf);		//���s

		//�Ō�ɔ����F
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vSpecular.x = stof(buf);
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vSpecular.y = stof(buf);
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vSpecular.z = stof(buf);

		_output->vMat.vMaterialData[i].vSpecular.w = 1.0f;
		getline(*_pFP, buf);		//���s

		//�����܂ł��}�e���A����^�B���̓e�N�X�`���̗L�����m�F����
		*_pFP >> buf;
		//�e�N�X�`���������ꍇ
		if (buf == "TextureFilename") {
			//�_�u���N�H�[�e�[�V���������O���ăe�N�X�`���p�X��ǂݍ���
			getline(*_pFP, buf, '"');		//���s
			getline(*_pFP, buf, '"');		//���s
			_output->vMat.vMaterialData[i].vTexFileName = buf;	//�p�X��ۑ�
			*_pFP >> buf;
			getline(*_pFP, buf);		//�p�X�̌��̉��s���΂��_
			*_pFP >> buf;		//�e�N�X�`���X�R�[�v��}�s���΂�
		}
		else {
			_output->vMat.vMaterialData[i].vTexFileName = "NONE";		//�e�N�X�`���Ȃ�������
		}
		if (buf != "}")
			getline(*_pFP, buf);		//���s
	}
	getline(*_pFP, buf);		//���s
}

/// <summary>
/// ���b�V���̒��̃X�L���E�F�C�g��T���Ă���
/// </summary>
/// <param name="_FileName"></param>
/// <param name="_pFP"></param>
/// <param name="pMesh"></param>
void cXFileLoad::GetMeshBone(std::ifstream * _pFP, XFileLoadMeshBuf * _output)
{
	string buf;

	//�܂��̓X�L�����b�V���̊�{�I�ȏ����擾���Ă���
	getline(*_pFP, buf, ';');		//�����ɂ͒��_������̍ő�e���{�[����������(��{�I�ɂS��)
	int MaxVertexBone = stoi(buf);
	getline(*_pFP, buf);	//���s

	getline(*_pFP, buf, ';');		//�����ɂ͈�ʂ�����̍ő�e���{�[����������(��{�I��6��)
	int MaxFaceBone = stoi(buf);
	getline(*_pFP, buf);	//���s

	getline(*_pFP, buf, ';');		//�����ɓǂݎ��{�[����
	int MaxBone = stoi(buf);
	getline(*_pFP, buf);	//���s


	//�������牺��SkinWeights��ǂݎ���Ă���

	//�{�[���������ǂݍ���
	for (int i = 0; i < MaxBone; i++) {
		//�܂���SkinWeights��T��
		while (buf != "SkinWeights") {
			*_pFP >> buf;
		}
		//SkinWeights�������Ĕ�����
		getline(*_pFP, buf, '"');	//���s

		//�^���̉��ɂ̓{�[���̖��O���i�[����Ă���
		getline(*_pFP, buf, '"');	//�{�[�������擾
		int BoneNo = 0;	//�{�[�����i�[���ꂽ�z��ԍ�
		for (int j = 0; m_Bones[j].vName != buf; j++) {
			BoneNo++;		//j��������玟�̓Y������T���̂�j�Ɠ�����BoneNo��++����
		}
		getline(*_pFP, buf);	//���s

		//�����ŉe���̂��钸�_�C���f�b�N�X��������
		*_pFP >> buf;
		int FaceNum = stoi(buf);
		int* pIndex = new int[FaceNum];	//�C���f�b�N�X���i�[������ꕨ
		for (int j = 0; j < FaceNum; j++) {
			*_pFP >> buf;
			pIndex[j] = stoi(buf);	//���Ԗڂ̒��_�ɏd�݂�t���邩�i�[
		}

		//���Ɋe���_�ɏd�ݏ���n���Ă���
		for (int j = 0; j < FaceNum; j++) {
			//��قǏW�߂��f�[�^(pIndex)�̏ꏊ�Ɋi�[���ꂽ���_�ԍ������o��
			int ind = pIndex[j];

			//�E�F�C�g��ǂݍ���
			*_pFP >> buf;
			float weight = stof(buf);


			//�܂��͉��Ԗڂ̃f�[�^�����m�F����
			int ID = ind;
			switch (_output->vFace.vVertex[ID].vBoneCnt) {
			case 0:
				//���߂Ċi�[����ꍇ�͓ǂݍ��񂾎��̏��������s��
				_output->vFace.vVertex[ID].vBoneNo = { (uint32_t)BoneNo,0,0,0 };
				_output->vFace.vVertex[ID].vWeight.x = weight;
				break;
			case 1:
				_output->vFace.vVertex[ID].vBoneNo.y = (uint32_t)BoneNo;
				_output->vFace.vVertex[ID].vWeight.y = weight;
				break;
			case 2:
				_output->vFace.vVertex[ID].vBoneNo.z = (uint32_t)BoneNo;
				_output->vFace.vVertex[ID].vWeight.z = weight;
				break;
			case 3:
				_output->vFace.vVertex[ID].vBoneNo.w = (uint32_t)BoneNo;
				_output->vFace.vVertex[ID].vWeight.w = weight;
				break;
			}

		}

		//�������炪�Ō�̍s���ǂݍ���
		XMFLOAT4X4 m;
		for (int j = 0; j < 15; j++) {
			getline(*_pFP, buf, ',');		//1�v�f��ǂݍ���
			m.m[j / 4][j % 4] = stof(buf);
		}
		getline(*_pFP, buf, ';');		//1�v�f��ǂݍ���
		m.m[3][3] = stof(buf);
		m_Bones[BoneNo].vBind = m;	//�o�C���h�|�[�Y���i�[����
		getline(*_pFP, buf);		//�s��s�����s
		delete[] pIndex;	//���

		*_pFP >> buf;	//�����ɂ̓X�L���E�F�C�g��}������
	}
	*_pFP >> buf;
}

/// <summary>
/// ��ԍŏ���"Frame"�������邾���̊֐�
/// </summary>
/// <param name="_pFP"></param>
void cXFileLoad::SearchFrame(std::ifstream * _pFP)
{
	string buf;
	*_pFP >> buf;

	//Frame�Ƃ��������������Ă��Ȃ��A�����t�@�C���I�[�ł��Ȃ���ΌJ��Ԃ�
	while (buf != "Frame" && !(_pFP->eof())) {
		if (buf == "template") {
			getline(*_pFP, buf);		//template�͈�s��΂�
		}
		else {
			*_pFP >> buf;
		}
	}

	//������ = �t�@�C���ǂݍ��݂�"Frame"�܂ōs���Ă���
}

//X�t�@�C���̏�񂩂�X�^�e�B�b�N���X�L�����b�V�����𔻒肷��
int cXFileLoad::CheckType(std::string _FileName)
{
	ifstream pFP(_FileName.c_str());
	string buf;
	int dwMeshType = 0;
	pFP >> buf;

	//�t�@�C���I���܂ŒT�����s���A�l�X�ȏ����擾����
	while (!(pFP.eof())) {
		if (buf == "template") {
			getline(pFP, buf);		//template�͈�s��΂�
		}
		if (buf == "XSkinMeshHeader") {
			dwMeshType = 2;			//���̃��f���f�[�^�ɂ̓A�j���[�V�������܂܂��
		}
		if (buf == "AnimationSet") {
			//���[�V�����f�[�^��������
			//"Animation"�̐�(�{�[����)���J�E���g���Ă��̒l��Ԃ��֐�
			m_BoneInMotion[m_MotionNum] = CountAnimeBone(&pFP);
			m_MotionNum++;		//���[�V���������J�E���g
			pFP >> buf;
		}
		else {
			pFP >> buf;
		}
	}

	//������ = �t�@�C���ǂݍ��݂�"Frame"�܂ōs���Ă���
	return dwMeshType;
}

/// <summary>
/// X�t�@�C����̃{�[��(�t���[��)��ǂݍ��ފ֐��B���ׂēǂݍ��ݏI���܂ŉ�A���������
/// </summary>
/// <param name="_pFP">���߂�Frame�܂œǂݍ��񂾏�Ԃ̂��̂��n�����</param>
/// <param name="_pBone">������ǂݍ��ރ{�[���̏����i�[����A�h���X</param>
void cXFileLoad::GetFrame(std::ifstream * _pFP, Bone * _pBone, std::string _ParentName)
{
	string buf;		//�ǂݍ��񂾕�������i�[����o�b�t�@
	string Name;		//�������g�̖��O
	XMFLOAT4X4 Mat;

	//���߂�Frame�̖��O��ǂݍ��ޕK�v������B
	*_pFP >> buf;		//�����œǂݍ��񂾂��̂����O
	Name = buf;
	_pBone->vBoneName = buf;	//���O��ێ����Ă���
	getline(*_pFP, buf);		//���O��ǂ񂾌�͉��s����B

	//���O�̎��̍s�̓��[�J���s��^���A���̎��̍s�ɐ��l���i�[����Ă���̂Ŏ��o��
	while (buf != "FrameTransformMatrix") {
		*_pFP >> buf;		//�s�񐔒l���i�[���ꂽ�s�̈�O�܂łǂ�ǂ�Ă�ł���
	}
	getline(*_pFP, buf);	//���J�b�R����菜��


	//�܂��͕�����','�܂œǂݍ��ނ��Ōゾ��;;�ŏI���̂ōs��v�f�� - 1(= 15)�񕪂����ǂݍ���
	for (int i = 0; i < 15; i++) {
		getline(*_pFP, buf, ',');	//���J�b�R����菜��
		_pBone->vTransform.m[i / 4][i % 4] = (float)std::stod(buf);
	}
	//���X�g����';'�����O������Ԃœǂݍ��ݍŌ��getline�Ŏ��̍s�ɐi�߂�
	getline(*_pFP, buf, ';');
	_pBone->vTransform._44 = (float)std::stod(buf);
	getline(*_pFP, buf);		//�����͓�ڂ�';'��ǂݍ���'{'�̂��߂ɂ�����x�c
	getline(*_pFP, buf);		//����ōs��i�[�I���
	Mat = _pBone->vTransform;	//�s��ۑ�

	//�����܂œǂݍ��߂���{�[������ۑ����Ă���
	SetBoneData(_ParentName, Name, Mat);


	//============�����܂ł�Frame�X�R�[�v�̒�^�����B���������Frame or Mesh�����X�Ƀ��[�h���邱�ƂɂȂ�===============
	*_pFP >> buf;		//���[�v�J�n�O�Ɉ�x�ǂݍ���
	while (buf != "}" && !(_pFP->eof()))
	{
		//�܂��͓ǂݍ��񂾂��̂�Frame �� Mesh�̂ǂ��炩���m�F����
		if (buf == "Frame") {
			//Frame�̎��̏���
			_pBone->vNumChild++;		//�q�̐����J�E���g����
			Bone* pChildBoneAddr = new Bone;
			GetFrame(_pFP, pChildBoneAddr, Name);		//��A�����I
			_pBone->vpChild.push_back(pChildBoneAddr);
		}
		else if (buf == "Mesh") {
			//Mesh�̏ꍇ�̏���
			SkipScope(_pFP);			//���i�K�ł�{}�Ԃ��΂��Ӗ������Ȃ��B
		}
		*_pFP >> buf;		//���̕�����ǂ�
	}
}

/// <summary>
/// �{�[���̏����ꎞ�I�ɕۑ����Ă����֐�
/// </summary>
/// <param name="_ParentName"></param>
/// <param name="_Name"></param>
/// <param name="_Mat"></param>
void cXFileLoad::SetBoneData(std::string _ParentName, std::string _Name, DirectX::XMFLOAT4X4 _Mat)
{
	int ParentNo = -1;

	//�܂��͐e�̓Y������T��
	for (int i = 0; i < m_Suffix; i++) {
		if (m_Bones[i].vName == _ParentName) {
			ParentNo = i;
		}
	}

	m_Bones[m_Suffix].vParentNo = ParentNo;
	m_Bones[m_Suffix].vName = _Name;
	m_Bones[m_Suffix].vMat = _Mat;

	m_Suffix++;		//�{�[�������J�E���g����
}

/// <summary>
/// ����}�܂Ŕ�΂�
/// </summary>
/// <param name="_pFP"></param>
void cXFileLoad::SkipScope(std::ifstream* _pFP)
{
	int cnt = 1;		//'{'�������+ '}'�������-����0�ɂȂ�����߂�
	string buf;
	getline(*_pFP, buf);

	while (cnt > 0 && !(_pFP->eof()))
	{
		*_pFP >> buf;
		if (buf == "{") cnt++;
		else if (buf == "}") cnt--;
	}
}

/// <summary>
/// ���[�V�����̒��ɂ���{�[���̐����J�E���g����
/// </summary>
/// <param name="_pFP"></param>
/// <returns></returns>
int cXFileLoad::CountAnimeBone(std::ifstream * _pFP)
{
	//�܂��͉��̊K�w�ɂ���"Animetion"��������
	string buf;
	int BoneNum = 0;				//�{�[���̐����i�[����
	getline(*_pFP, buf);		//�����ɂ̓A�j���[�V�����̖��O�������Ă���
	*_pFP >> buf;		//������"Animetion"������

	while (buf != "}") {
		getline(*_pFP, buf);		//���s
		BoneNum++;		//�����J�E���g

		SkipScope(_pFP);
		*_pFP >> buf;		//������"Animetion" or "}"������
	}
	return BoneNum;			//�{�[������ԋp
}

/// <summary>
/// �A�j���[�V�����f�[�^���쐬����
/// </summary>
/// <param name="_pFP"></param>
/// <param name="_output"></param>
void cXFileLoad::GetAnimation(std::ifstream * _pFP, cAnimationData *& _output)
{
	string buf;
	//�܂��͕K�v�ȃf�[�^�̈���m�ۂ���
	_output = new cAnimationData;
	_output->m_MotionNum = m_MotionNum;		//���[�V�����̐���ۑ�
	_output->m_pMotion = new MotionData[m_MotionNum];	//���[�V���������̃f�[�^���m��

	//���Ɉ�̃��[�V�����̒��̃{�[�������̃f�[�^�̈���m�ۂ���
	for (int i = 0; i < m_MotionNum; i++) {
		_output->m_pMotion[i].vBoneNum = m_BoneInMotion[i];		//���̃��[�V�����ԍ��ɂ���{�[������ێ�
		_output->m_pMotion[i].vBoneAnime = new BoneAnime[m_BoneInMotion[i]];	//�{�[�������̃f�[�^�̈���m�ۂ���
	}

	//�������炪�f�[�^�̓ǂݍ��݁B�A�j���[�V���������̃f�[�^���擾���Ă���
	for (int i = 0; i < m_MotionNum; i++) {
		*_pFP >> buf;				//������"AnimetionSet"���擾�B
		getline(*_pFP, buf);		//���̍s�����s������

		//�{�[�������̃f�[�^��ǂݍ���ł���
		for (int j = 0; j < m_BoneInMotion[i]; j++) {
			*_pFP >> buf;				//������"Animetion"���擾�B
			getline(*_pFP, buf);		//���̍s�����s������
			GetKeyFrame(_pFP, &_output->m_pMotion[i].vBoneAnime[j]);		//�{�[���ɃL�[�t���[���f�[�^���i�[����
		}
		*_pFP >> buf;
	}
}

/// <summary>
/// �n���ꂽ�{�[���\���̂ɃL�[�t���[���f�[�^�A�{�[���ԍ���n���֐��B"Animetion"�̒��g���܂�܂���o��
/// </summary>
/// <param name="_pFP"></param>
/// <param name="_pBone"></param>
void cXFileLoad::GetKeyFrame(std::ifstream * _pFP, BoneAnime * _pBone)
{
	//�����ɗ������_�Ŏ��ɓǂݍ��߂���̂�AnimationKey�ɂȂ��Ă���O��
	string buf = "Start!";

	while (buf != "{")		//����"{"�̓L�[�t���[���̌�ɗ���{�[�����������Ă���X�R�[�v�̓���
	{
		*_pFP >> buf;
		if (buf == "AnimationKey") {
			getline(*_pFP, buf);		//���̍s�����s������
			*_pFP >> buf;		//�����ɗ���̂̓L�[�t���[���̃^�C�v(0 = ��], 1 = �g��, 2 = �ړ�, 3 = �s��)
			int type = stoi(buf);

			*_pFP >> buf;		//�����ɃL�[�t���[�����������Ă���
			int KeyNum = stoi(buf);	//���̎��̃L�[�t���[����

			//�A�h���X��nullptr�̎������V�����f�[�^�̈���m�ۂ���
			if (_pBone->vpKeyFrame == nullptr) {
				_pBone->vKeyNum = stoi(buf);
				_pBone->vpKeyFrame = new KeyFrame[_pBone->vKeyNum];
			}
			getline(*_pFP, buf);		//���s	

			//�L�[�t���[���̐������ǂݍ���
			for (int i = 0; i < KeyNum; i++) {
				getline(*_pFP, buf, ';');		//�L�[�t���[�����Ԃ��i�[
				int Time = stoi(buf);	//���ԏ����擾
				//�܂��f�[�^���i�[����Ă��Ȃ���΋����I�Ɏ��ԏ����擾����
				if (_pBone->vpKeyFrame[i].vTime == -1) {
					_pBone->vpKeyFrame[i].vTime = Time;
				}
				else {
					//���łɊi�[����Ă���ꍇ�̏���(�����t���[���ԍ������������̓Y�����ɂ���)
					int Suf = 0;	//�T���Y����
					while (_pBone->vpKeyFrame[i].vTime != Time)
					{
						i++;		//�����̈ʒu�܂œǂݍ��݂��΂�
					}
				}
				//�܂��͏��߂ɂ����̗v�f���f�[�^�Ƃ��Ď����Ă��邩���m�F����
				getline(*_pFP, buf, ';');
				int ElemNum = stoi(buf);		//�ǂݍ��ޗv�f��
				//�L�[�t���[���^�C�v�ɂ���ăf�[�^�𕪂���B
				if (type == 3) {
					//������s��f�[�^�������Ă���ꍇ
					getline(*_pFP, buf, ';');		//�����ɗv�f�������邪�s��Ȃ̂�16�ɂȂ�
					//�Ō�̈�񂾂��I����;�Ȃ̂ł܂�15��ǂݍ���
					for (int No = 0; No < 15; No++) {
						getline(*_pFP, buf, ',');
						_pBone->vpKeyFrame[i].vMat.m[No / 4][No % 4] = stof(buf);
					}
					getline(*_pFP, buf, ';');
					_pBone->vpKeyFrame[i].vMat.m[3][3] = stof(buf);		//�Ō�̈��
					getline(*_pFP, buf);		//���s
				}
				else {
					//�s��ȊO�̗v�f�̏ꍇ

					//�K�v���������f�[�^�����o������{�I��4�v�f�ڂ��s�v�Ȃ̂ŎO��
					XMFLOAT4 read = { 0.0f,0.0f,0.0f,0.0f };
					getline(*_pFP, buf, ',');			//�܂���v�f��
					read.x = stof(buf);
					getline(*_pFP, buf, ',');			//�܂���v�f��
					read.y = stof(buf);
					//3�v�f�ڂ����v�f���ɉ����ď�����؂�ւ���
					if (ElemNum == 3) {
						//3��������I���Ƃ���;�ŏI��
						getline(*_pFP, buf, ';');
					}
					else {
						//4��������,�ŏI��
						getline(*_pFP, buf, ',');
					}
					read.z = stof(buf);
					if (ElemNum == 4) {
						getline(*_pFP, buf, ';');
						read.w = stof(buf);
					}
					getline(*_pFP, buf);		//���s
					//�s����쐬����
					XMMATRIX m;
					if (type == 0) {
						//X�t�@�C����ł̓N�H�[�^�j�I��(W,X,Y,Z)�̏��Ŋi�[����Ă���
						XMFLOAT4 rotData = { read.y,read.z,read.w,read.x };
						XMVECTOR rotVec = XMLoadFloat4(&rotData);
						XMMATRIX RotMat = XMMatrixRotationQuaternion(rotVec);
						RotMat = XMMatrixInverse(NULL, RotMat);
						//m = XMMatrixRotationRollPitchYaw(read.x, read.y, read.z) * XMLoadFloat4x4(&_pBone->vpKeyFrame[i].vMat);		//���ŕς��鏈���͂���
						m = RotMat * XMLoadFloat4x4(&_pBone->vpKeyFrame[i].vMat);		//���ŕς��鏈���͂���
					}
					else if (type == 1)
						m = XMMatrixScaling(read.x, read.y, read.z) * XMLoadFloat4x4(&_pBone->vpKeyFrame[i].vMat);
					else if (type == 2)
						m = XMLoadFloat4x4(&_pBone->vpKeyFrame[i].vMat) * XMMatrixTranslation(read.x, read.y, read.z);
					else {
						MessageBox(0, "�A�j���[�V�����ɂĕs���Ȓl���ǂݍ��܂�܂���", NULL, MB_OK);
					}
					XMStoreFloat4x4(&_pBone->vpKeyFrame[i].vMat, m);
				}		// end if
			}		// end for
			*_pFP >> buf;
		}		// end "AnimationKey"
	}		// end while

	//�Ō�Ƀ{�[���ԍ����擾���Ċ���
	*_pFP >> buf;	//�{�[���̖��O������
	bool BoneNameFindFlag = false;
	for (int i = 0; i < m_Suffix; i++) {
		if (m_Bones[i].vName == buf) {
			_pBone->vBoneNo = i;		//�{�[���ԍ����������I
			BoneNameFindFlag = true;
		}
	}
	if (!BoneNameFindFlag) {
		_pBone->vBoneNo = -1;
	}
	getline(*_pFP, buf);		//�{�[���̗�����s
	getline(*_pFP, buf);		//�A�j���[�V�����̕��̉��s
}


HRESULT cXFileLoad::CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer) {
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = dwSize;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pIndex;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if (FAILED(GetDirectX::Device()->CreateBuffer(&bd, &InitData, ppIndexBuffer)))
	{
		MessageBox(0, "�C���f�b�N�X�ǂݍ��ݎ��s�I", NULL, MB_OK);
		return FALSE;
	}

	return S_OK;
}