/*=================================================
//								FILE : Tessellation.cpp
//		�t�@�C������ :
//		�e�b�Z���[�V�����X�e�[�W�ł���n���V�F�[�_�A�h���C���V�F�[�_�[��
//		�R���p�C����S������
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "Tessellation.h"
#include "ShaderMonitoring.h"

namespace {
	std::string dire = "HLSL/";		//HLSL���i�[�����f�B���N�g��
}

std::shared_ptr<TessellationStruct> cTessellationManager::LoadTessellationData(std::string _FileName)
{
	std::string path = dire + _FileName;

	//���̖��O��map�ɓo�^����Ă��邩�m�F����
	auto itr = m_Map.find(path);
	if (itr != m_Map.end()) {
		//�ݒ肳��Ă���ꍇ�̏���
		return m_Map[path];		//shared_ptr�̃R�s�[
	}
	//���ɂȂ����map�ɗ̈���m�ۂ��Ă���g���q�𔻕ʂ��Ċe�p�[�T�[���Ăяo���ăf�[�^���擾����
	else {
		//�ݒ肳��Ă��Ȃ��ꍇ�̏���
		m_Map[path] = std::make_shared<TessellationStruct>();

		GetShader(path, &*m_Map[path]);
		cShaderMonitoring::GetInstance().AddFileDate(path);
	}
	return m_Map[path];		//�R�s�[
}

/// <summary>
/// �V�F�[�_�𗘗p���Ȃ��ۂɂ܂Ƃ߂ăe�b�Z���[�V�����X�e�[�W���I�t�ɂ���
/// </summary>
void cTessellationManager::SetTessellationToNull()
{
	GetDirectX::Context()->HSSetShader(NULL, NULL, 0);
	GetDirectX::Context()->DSSetShader(NULL, NULL, 0);
}

HRESULT cTessellationManager::ShaderReload(std::string path)
{
	auto itr = m_Map.find(path);
	if (itr != m_Map.end()) {
		//�ݒ肳��Ă���ꍇ�̏���
		m_Map[path]->release();
		return GetShader(path, &*m_Map[path]);
	}
	
	return S_OK;		//�Ȃ���Εʂɖ��Ȃ�
}

HRESULT cTessellationManager::GetShader(std::string _fileName, TessellationStruct * pTesse)
{
	ID3DBlob *pCompiledShader = nullptr;
	ID3DBlob *pErr = nullptr;
	//�u���u����n���V�F�[�_�[�쐬
	if (FAILED(D3DX11CompileFromFile(_fileName.c_str(), NULL, NULL, "HS", "hs_5_0", 0, 0, NULL, &pCompiledShader, &pErr, NULL)))
	{
		MessageBox(0, (_fileName + std::string("�̓ǂݍ��݂Ɏ��s")).c_str(), NULL, MB_OKCANCEL);
		return E_FAIL;
	}
	SAFE_RELEASE(pErr);
	if (FAILED(GetDirectX::Device()->CreateHullShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &pTesse->m_pHullShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, (_fileName + std::string("��HS�쐬�Ɏ��s")).c_str(), NULL, MB_OKCANCEL);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	//�u���u����h���C���V�F�[�_�[�쐬
	if (FAILED(D3DX11CompileFromFile(_fileName.c_str(), NULL, NULL, "DS", "ds_5_0", 0, 0, NULL, &pCompiledShader, &pErr, NULL)))
	{
		MessageBox(0, (_fileName + std::string("�̓ǂݍ��݂Ɏ��s")).c_str(), NULL, MB_OKCANCEL);
		return E_FAIL;
	}
	SAFE_RELEASE(pErr);
	if (FAILED(GetDirectX::Device()->CreateDomainShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &pTesse->m_pDomainShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, (_fileName + std::string("��DS�쐬�Ɏ��s")).c_str(), NULL, MB_OKCANCEL);
		return E_FAIL;
	}

	SAFE_RELEASE(pCompiledShader);
	return S_OK;
}

/// <summary>
/// �폜����Ďg���Ă��Ȃ��f�[�^���c���Ă��邩�m�F
/// </summary>
void cTessellationManager::CheckDelete()
{
	for (auto itr = m_Map.begin(); itr != m_Map.end(); ++itr) {
		//map�����|�C���^��ێ����Ă��Ȃ�
		if (itr->second.use_count() == 1) {
			m_Map.erase(itr->first);		//�f�[�^�폜
			itr = m_Map.begin();
			if (itr == m_Map.end())
				return;
		}
	}
}
