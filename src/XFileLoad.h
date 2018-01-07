/*=================================================
//								FILE : XFileLoad.cpp
//		�t�@�C������ :
//		�A�j���[�V�����t��X�t�@�C���̓ǂݍ��݂��s���N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include <string>
#include <Windows.h>
#include <DirectXMath.h>
#include <fstream>

#include "Mesh3D.h"
#include "BoneStruct.h"

struct XFileLoadMeshBuf;		//X�t�@�C���ǂݍ��ݎ��ɏ����ꂩ���ɂ܂Ƃ߂邽�߂̍\����

namespace XFileManager {
	//X�t�@�C������ǂݍ��񂾃{�[�������ꎞ�I�Ɋi�[���Ă����\����
	struct XFileBone {
		std::string vName;					//�����̖��O
		int vParentNo;						//�e�̓Y�����ԍ�
		DirectX::XMFLOAT4X4 vMat;		//�����p���s��
		DirectX::XMFLOAT4X4 vBind;		//�����p���s��
	};
}

/// <summary>
/// X�t�@�C�����[�_�[�B�V���O���g��
/// </summary>
class cXFileLoad {
public:
	cXFileLoad(const cXFileLoad&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cXFileLoad& operator=(const cXFileLoad&) = delete;	//�R�s�[������Z�q�� delete �w��
	cXFileLoad(cXFileLoad&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cXFileLoad& operator=(cXFileLoad&&) = delete;		//���[�u������Z�q�� delete �w��

	static cXFileLoad& GetInstance() {
		static cXFileLoad inst;			// private�ȃR���X�g���N�^���Ăяo��
		return inst;
	}
	void GetXFile(Mesh3D* pMesh,std::string _FileName);		//�������Ăяo���ă��[�h����

private:
	void InitData();		//�ǂݍ��ޏ���������
	void GetMesh(std::string _FileName, XFileLoadMeshBuf* _output);
	void CreateMesh3D(Mesh3D* pMesh, XFileLoadMeshBuf*);
	void GetNormal(std::ifstream* _pFP, XFileLoadMeshBuf* _output);
	void GetUV(std::ifstream* _pFP, XFileLoadMeshBuf* _output);
	void GetMaterialList(std::ifstream* _pFP, XFileLoadMeshBuf* _output);
	void GetMeshBone(std::ifstream* _pFP, XFileLoadMeshBuf * _output);

	void SearchFrame(std::ifstream* _pFP);
	int CheckType(std::string _FileName);
	void GetFrame(std::ifstream* _pFP,Bone* _pBone,std::string _ParentName);
	void SetBoneData(std::string _ParentName,std::string _Name,DirectX::XMFLOAT4X4 _Mat);
	void SkipScope(std::ifstream* _pFP);		// �X�R�[�v�𔲂��邽�߂̊֐�

	//�A�j���[�V�����ǂݍ���
	int CountAnimeBone(std::ifstream* _pFP);		//�A�j���[�V�����̒��ɂ���{�[���̐��𐔂��ăX�R�[�v�𔲂���
	void GetAnimation(std::ifstream* _pFP,cAnimationData*& _output);	//���ۂɃf�[�^���i�[���Ă����֐�
	void GetKeyFrame(std::ifstream* _pFP, BoneAnime* _pBone);

	HRESULT CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer);

	cXFileLoad() = default;						//�R���X�g���N�^�� private �ɒu��
	~cXFileLoad() = default;					//�f�X�g���N�^�� private �ɒu��

	XFileManager::XFileBone m_Bones[128];	//�ǂݍ��񂾃{�[�����ꎞ�I�ɓ����
	short m_BoneInMotion[128];					//��̃��[�V�����Ɋi�[����Ă���{�[����(�ő僂�[�V������128)
	int m_MotionNum;	//���[�V������
	int m_Suffix;			//���݉��Ԗڂ̃{�[����ǂ�ł��邩���`�F�b�N����
	int m_Type;			//���f�������m�F����()
};
