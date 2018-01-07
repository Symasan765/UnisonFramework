/*=================================================
//								FILE : Tessellation.h
//		�t�@�C������ :
//		�e�b�Z���[�V�����X�e�[�W�ł���n���V�F�[�_�A�h���C���V�F�[�_�[��
//		�R���p�C����S������
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"
#include <memory>
#include <map>
#include <string>

/// <summary>
/// �e�b�Z���[�V�����ŗ��p����V�F�[�_�e���ێ�����
/// </summary>
struct TessellationStruct {
	ID3D11HullShader* m_pHullShader;
	ID3D11DomainShader* m_pDomainShader;

	TessellationStruct() {
		m_pHullShader = nullptr;
		m_pDomainShader = nullptr;
	}

	void SetTessellation() {
		GetDirectX::Context()->HSSetShader(m_pHullShader, NULL, 0);
		GetDirectX::Context()->DSSetShader(m_pDomainShader, NULL, 0);
	}

	~TessellationStruct() {
		release();
	}

	void release() {
		SAFE_RELEASE(m_pHullShader);
		SAFE_RELEASE(m_pDomainShader);
	}
};

/// <summary>
/// �e�b�Z���[�V�����̃��[�h�Ȃǂ̊Ǘ���S������
/// </summary>
class cTessellationManager {
public:
	//�X�V���������������s�����߂�cDirectX�N���X���t�����h�����Ă���
	friend class cDirectX11;
	cTessellationManager(const cTessellationManager&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cTessellationManager& operator=(const cTessellationManager&) = delete;	//�R�s�[������Z�q�� delete �w��
	cTessellationManager(cTessellationManager&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cTessellationManager& operator=(cTessellationManager&&) = delete;		//���[�u������Z�q�� delete �w��

	static cTessellationManager& GetInstance() {
		static cTessellationManager inst;			// private�ȃR���X�g���N�^���Ăяo��
		return inst;
	}

	std::shared_ptr<TessellationStruct> LoadTessellationData(std::string);
	void SetTessellationToNull();
	HRESULT ShaderReload(std::string _fileName);
private:
	HRESULT GetShader(std::string _fileName, TessellationStruct* pTesse);
	void CheckDelete();
	cTessellationManager() = default;					//�R���X�g���N�^�� private �ɒu��
	~cTessellationManager() = default;					//�f�X�g���N�^�� private �ɒu��
	std::map<std::string, std::shared_ptr<TessellationStruct>> m_Map;		//�e�b�Z���[�V������ێ�����}�b�v
};