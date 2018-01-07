/*=================================================
//								FILE : Mesh3D.h
//		�t�@�C������ :
//		�`��ɗ��p���郁�b�V����������舵���N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include <memory>
#include <map>

#include "TextureManager.h"
#include "Vertex.h"
#include "WinMain.h"
#include "BoneStruct.h"
#include "ConstantBuffer.h"
#include "Animation.h"

struct Material {
	DirectX::XMFLOAT4 Ka;//�A���r�G���g
	DirectX::XMFLOAT4 Kd;//�f�B�t���[�Y
	DirectX::XMFLOAT4 Ks;//�X�y�L�����[
	std::shared_ptr<Texture2D> pDefaultTex;				//�ʏ�f�B�t���[�Y
	std::shared_ptr<Texture2D> pShadowTex;				//�e�f�B�t���[�Y
	std::shared_ptr<Texture2D> pLightTex;					//���C�g�f�B�t���[�Y
	std::shared_ptr<Texture2D> pMaterialMaskTex;		//���ꏈ���}�X�N(R���� : SSSSS,G���� : �N�b�N�g�����X)
	DWORD dwNumFace;//���̃}�e���A���ł���|���S����
	Material() {
		Ka = { 1.0f,1.0f,1.0f,1.0f };
		Kd = { 1.0f,1.0f,1.0f,1.0f };
		Ks = { 1.0f,1.0f,1.0f,1.0f };
		dwNumFace = 0;
		pDefaultTex = nullptr;
		pShadowTex = nullptr;
		pLightTex = nullptr;
		pMaterialMaskTex = nullptr;
	}
};

struct Mesh3D {
	ID3D11Buffer* vVertexBuf;			//���_�o�b�t�@
	ID3D11Buffer** vppIndexBuffer;		//�C���f�b�N�X�o�b�t�@(�}�e���A�������쐬�����)
	Material* vpMaterial;					//�}�e���A�����
	std::string dir;							//�f�B���N�g����
	VERTEX3D* vpVertexData;				//���_�f�[�^��ێ�����
	cAnimationData* vpAnimetion;		//�A�j���[�V�������
	int** ppiIndex;							//�C���f�b�N�X�f�[�^
	int vMaterialCnt;							//�}�e���A�������������Ă邩�H
	int vTotalFace;							//���b�V���������|���S����

	Mesh3D() {
		vVertexBuf = nullptr;
		vppIndexBuffer = nullptr;
		vpMaterial = nullptr;
		vpVertexData = nullptr;
		ppiIndex = nullptr;
		vpAnimetion = nullptr;
	}

	void Draw() {
		//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g�i�o�[�e�b�N�X�o�b�t�@�[�͈�ł����j
		UINT stride = sizeof(VERTEX3D);
		UINT offset = 0;
		GetDirectX::Context()->IASetVertexBuffers(0, 1, &vVertexBuf, &stride, &offset);

		//�}�e���A���̐������A���ꂼ��̃}�e���A���̃C���f�b�N�X�o�b�t�@�|��`��
		for (int i = 0; i<vMaterialCnt; i++)
		{
			//�g�p����Ă��Ȃ��}�e���A���΍�
			if (vpMaterial[i].dwNumFace == 0)
			{
				continue;
			}

			GetDirectX::Context()->IASetIndexBuffer(vppIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

			//�v���~�e�B�u�E�g�|���W�[���Z�b�g
			GetDirectX::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//�e�N�X�`���[���V�F�[�_�[�ɓn��
			if (vpMaterial[i].pDefaultTex != NULL)
			{
				GetDirectX::Context()->PSSetShaderResources(0, 1, &vpMaterial[i].pDefaultTex->vpTexture);
				GetDirectX::Context()->VSSetShaderResources(0, 1, &vpMaterial[i].pDefaultTex->vpTexture);
			}

			//Draw
			GetDirectX::Context()->DrawIndexed(vpMaterial[i].dwNumFace * 3, 0, 0);
		}
	}

	//�������
	~Mesh3D() {
		if (vVertexBuf) {
			vVertexBuf->Release();
		}
		for (int i = 0; i < vMaterialCnt; i++) {
			if (vppIndexBuffer[i])
				vppIndexBuffer[i]->Release();
			delete[] ppiIndex[i];
		}
		delete vpAnimetion;
		delete[] vpMaterial;
		delete[] ppiIndex;
		delete[] vpVertexData;
	}
};

struct ConstantBone {
	DirectX::XMFLOAT4X4 m[128];
};

//���b�V���f�[�^���Ǘ�����
//�K�v�@�\
//���̃t�@�C������map�ɑ��݂��邩�H
//�t�@�C�������[�h���ꂽ�疼�O���m�F���Ă��łɑ��݂���΂����shared_ptr��n��
//���t���[���m�ۂ��Ă���shared_ptr���m�F���ăJ�E���^��map�̂�(�܂�1)��������map����shared_ptr���폜����ƃf�X�g���N�^�𔭓����Ă����

//�t�@�C�������n���ꂽ��܂��g���q���m�F���Ă���ɑΉ��������[�_�[���Ăяo��

//MainFrame���t�����h�����ăf�X�g���N�^���肷��Ƃ�������
class MeshManager {
public:
	friend class cDirectX11;
	MeshManager(const MeshManager&) = delete;
	MeshManager& operator=(const MeshManager&) = delete;
	MeshManager(MeshManager&&) = delete;
	MeshManager& operator=(MeshManager&&) = delete;

	static MeshManager& GetInstance() {
		static MeshManager inst;			// private�ȃR���X�g���N�^���Ăяo��
		return inst;
	}
	std::shared_ptr<Mesh3D> LoadMeshData(std::string _fileName);	//�ǂݍ��݊֐�		TODO:
	void SetBoneConstant(const BoneData* _bone, const int _Num,int pass = 2);	//�{�[���f�[�^���V�F�[�_�[�ɃZ�b�g����
private:
	MeshManager() = default;					//�R���X�g���N�^�� private �ɒu��
	~MeshManager() = default;					//�f�X�g���N�^�� private �ɒu��
	void CheckDelete();		//����Ńf�[�^�����������m�F����

	std::map<std::string, std::shared_ptr<Mesh3D>> m_MeshMap;		//Mesh��ێ�����}�b�v
	cConstBuf<ConstantBone> m_Bone;
};