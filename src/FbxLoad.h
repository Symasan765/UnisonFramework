/*=================================================
//								FILE : FbxLoad.h
//		�t�@�C������ :
//		FBX�����[�h���郍�[�_�[
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once

#include "Mesh3D.h"
#include <fbxsdk.h>

/// <summary>
/// ��{�I�ɂ̓��b�V���}�l�[�W�����炵���Ăяo���Ȃ�
/// </summary>
class cFbxLoad {
public:
	cFbxLoad(const cFbxLoad&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cFbxLoad& operator=(const cFbxLoad&) = delete;	//�R�s�[������Z�q�� delete �w��
	cFbxLoad(cFbxLoad&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cFbxLoad& operator=(cFbxLoad&&) = delete;		//���[�u������Z�q�� delete �w��

	static cFbxLoad& GetInstance() {
		static cFbxLoad inst;			// private�ȃR���X�g���N�^���Ăяo��
		return inst;
	}

	HRESULT GetFBXLoader(Mesh3D* mesh,std::string);

private:
	HRESULT cFbxLoad::Init(std::string _fileName);
	void cFbxLoad::GetMesh(FbxNode* node, Mesh3D* mesh);
	void cFbxLoad::GetMeshData(FbxMesh* pFbxMesh, Mesh3D* meshdata);
	HRESULT cFbxLoad::CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer);
	cFbxLoad() = default;					//�R���X�g���N�^�� private �ɒu��
	~cFbxLoad() = default;					//�f�X�g���N�^�� private �ɒu��

	FbxManager* m_pManager;
	FbxScene* m_pScene;
};