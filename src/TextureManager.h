/*=================================================
//								FILE : TextureManager.h
//		�t�@�C������ :
//		�e�N�X�`���̕����ǂݍ��݂�h�~���邽�߂Ƀe�N�X�`�����Ǘ�����
//		�}�l�[�W���N���X���`���Ă�������ǂނ悤�ɂ���B
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once

#include <d3d11.h>
#include <memory>
#include <map>

/// <summary>
/// �e�N�X�`���̊Ǘ����s���N���X
/// �e�N�X�`����ǂݍ��ލۂ�LoadTex�̖߂�l��shard_ptr�Ŏ󂯎��΃e�N�X�`�����擾�ł���
/// </summary>
struct Texture2D {
	ID3D11ShaderResourceView* vpTexture;

	~Texture2D() {
		if (vpTexture) {
			vpTexture->Release();
			vpTexture = nullptr;
		}
	}
};

class cTexManager {
public:
	//�X�V���������������s�����߂�cDirectX�N���X���t�����h�����Ă���
	friend class cDirectX11;
	cTexManager(const cTexManager&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cTexManager& operator=(const cTexManager&) = delete;	//�R�s�[������Z�q�� delete �w��
	cTexManager(cTexManager&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cTexManager& operator=(cTexManager&&) = delete;		//���[�u������Z�q�� delete �w��

	static cTexManager& GetInstance() {
		static cTexManager inst;			// private�ȃR���X�g���N�^���Ăяo��
		return inst;
	}

	enum SampleState {
		WRAP = 0,
		CLAMP
	};

	std::shared_ptr<Texture2D> LoadTexData(std::string _fileName,bool _meshFlag = false);	//�ǂݍ��݊֐�		TODO:
	HRESULT CreateTexSampler(ID3D11SamplerState** m_pSampleLinear, SampleState samp = WRAP);
private:
	HRESULT GetTex(std::string _fileName, Texture2D* pTex);		//�e�N�X�`����V�������[�h����ꍇ�Ɏg�p����
	void CheckDelete();
	cTexManager() = default;					//�R���X�g���N�^�� private �ɒu��
	~cTexManager() = default;					//�f�X�g���N�^�� private �ɒu��

	std::map<std::string, std::shared_ptr<Texture2D>> m_TexMap;		//�e�N�X�`����ێ�����}�b�v
};