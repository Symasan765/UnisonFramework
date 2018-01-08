/*=================================================
//								FILE : Mesh3D.cpp
//		�t�@�C������ :
//		���b�V���}�l�[�W���𗘗p���ă��b�V�����̎擾���s��
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "Mesh3D.h"
#include "FbxLoad.h"
#include "XFileLoad.h"

using namespace DirectX;

std::shared_ptr<Mesh3D> MeshManager::LoadMeshData(std::string _fileName){
	//�܂��̓p�X������������
	int point = (int)_fileName.find(".");		//.��T�����Ĉʒu�𒲂ׂ�
	std::string name = _fileName.substr(0, point);	//�t�@�C����
	std::string extension = _fileName.substr(point + 1, _fileName.size());	//�g���q
	std::string path = "asset/Mesh/" + name + "/" + _fileName;		//[��]asset/model/model.fbx

	//�܂����̖��O��map�ɓo�^����Ă��邩�m�F����
	auto itr = m_MeshMap.find(path);        // "xyz" ���ݒ肳��Ă��邩�H
	if (itr != m_MeshMap.end()) {
		//�ݒ肳��Ă���ꍇ�̏���
		return m_MeshMap[path];		//shared_ptr�̃R�s�[
	}
	//���ɂȂ����map�ɗ̈���m�ۂ��Ă���g���q�𔻕ʂ��Ċe�p�[�T�[���Ăяo���ăf�[�^���擾����
	else {
		//�ݒ肳��Ă��Ȃ��ꍇ�̏���
		m_MeshMap[path] = std::make_shared<Mesh3D>();
		m_MeshMap[path]->dir = "asset/Mesh/" + name + "/";


		//�����Ńp�[�T�[���Ăяo���B�ǂݍ��݃t�H���_���w�肷��̖Y�ꂸ�ɂ�
		if (extension == "fbx") {	//FBX�ǂݍ���
			//�����I�ɂ�����FBX�̏ꍇ�ɓ���鏈���ɂ���
			cFbxLoad::GetInstance().GetFBXLoader(&*m_MeshMap[path], path);
		}
		else if (extension == "x") {	//X�ǂݍ���
			cXFileLoad::GetInstance().GetXFile(&*m_MeshMap[path], path);
			int a = 10;
		}
	}

	return m_MeshMap[path];
}

void MeshManager::SetBoneConstant(const BoneData* _bone, const int _Num, int pass)
{
	DirectX::XMMATRIX Mat[128];	//�v�Z�����f�[�^������ꏊ
	DirectX::XMMATRIX Cur[128];

	//�܂��s��ɂ��ꂼ��̎p���s��������Ċi�[����
	for (int i = 0; i < _Num; i++) {
		Cur[i] = XMLoadFloat4x4(&_bone[i].vInv);
		Mat[i] =  XMLoadFloat4x4(&_bone[i].vTransPose);
	}

	//���ɐe�q�֌W�����Ă���
	for (int i = 0; i < _Num; i++) {
		//�e�����݂���ꍇ�̂ݏ�������
		if (_bone[i].vParentNo != -1) {
			DirectX::XMFLOAT4X4 Child;
			DirectX::XMFLOAT4X4 Parent;
			XMStoreFloat4x4(&Child,Mat[i]);
			XMStoreFloat4x4(&Parent, Mat[_bone[i].vParentNo]);
			Mat[i] *= Mat[_bone[i].vParentNo];
		}
	}

	for (int i = 0; i < _Num; i++) {
		XMMATRIX offset = DirectX::XMLoadFloat4x4(&_bone[i].vOffset);
		XMFLOAT4X4 m;
		XMStoreFloat4x4(&m, Mat[i]);
		Mat[i] = offset * Mat[i];
		XMStoreFloat4x4(&m, Mat[i]);
		DirectX::XMStoreFloat4x4(&m_Bone.data.m[i], DirectX::XMMatrixTranspose(Mat[i]));		//�{�[�����Z�b�g����
		int a = 10;
	}

	m_Bone.Set(pass);
}

void MeshManager::CheckDelete()
{
	//map��̂��ׂẴf�[�^���m�F���ăJ�E���^�����m�F����
	//���̃J�E���g��map�̂�(�܂�1)�Ȃ炻��map���������
	//�����shared_ptr���f�X�g���N�^���Ăт�����Mesh�f�[�^����������
	
	for (auto itr = m_MeshMap.begin(); itr != m_MeshMap.end(); ++itr) {
		//map�����|�C���^��ێ����Ă��Ȃ�
		if (itr->second.use_count() == 1) {
			m_MeshMap.erase(itr->first);		//�f�[�^�폜
			itr = m_MeshMap.begin();
			if (itr == m_MeshMap.end())
				return;
		}
	}
}
