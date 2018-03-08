/*=================================================
//								FILE : TextureManager.h
//		�t�@�C������ :
//		�e�N�X�`���̕����ǂݍ��݂�h�~���邽�߂Ƀe�N�X�`�����Ǘ�����
//		�}�l�[�W���N���X���`���Ă�������ǂނ悤�ɂ���B
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "TextureManager.h"
#include "WinMain.h"
#include <d3dx11.h>

/// <summary>
/// �e�N�X�`����ǂݍ��ގ��ɗ��p����֐�
/// </summary>
/// <param name="_fileName">�t�@�C�����B(�t�H���_�܂ł̃p�X�͎����ŕt�^����)</param>
/// <param name="_meshFlag">���b�V���ɕt�^����f�[�^�Ȃ̂��H(���b�V���̃f�[�^�̏ꍇ�A�p�X���ς��)</param>
/// <returns>�e�N�X�`����ێ������V�F�A�[�h�|�C���^</returns>
std::shared_ptr<Texture2D> cTexManager::LoadTexData(std::string _fileName,bool _meshFlag)
{
	//�܂����̃e�N�X�`�������f������ǂݍ��܂ꂽ�̂��ʏ��Ԃœǂ܂��̂��Ńp�X��ύX����
	std::string path;
	if (_meshFlag) {
		path = _fileName;
	}
	else {
		path = "asset/Texture/";
		path += _fileName;
	}

	//���̖��O��map�ɓo�^����Ă��邩�m�F����
	auto itr = m_TexMap.find(path);        // "xyz" ���ݒ肳��Ă��邩�H
	if (itr != m_TexMap.end()) {
		//�ݒ肳��Ă���ꍇ�̏���
		return m_TexMap[path];		//shared_ptr�̃R�s�[
	}
	//���ɂȂ����map�ɗ̈���m�ۂ��Ă���g���q�𔻕ʂ��Ċe�p�[�T�[���Ăяo���ăf�[�^���擾����
	else {
		//�ݒ肳��Ă��Ȃ��ꍇ�̏���
		m_TexMap[path] = std::make_shared<Texture2D>();

		GetTex(path, &*m_TexMap[path]);
	}
	return m_TexMap[path];		//�R�s�[
}

/// <summary>
/// �e�N�X�`���T���v��������Ă����֐�
/// </summary>
/// <param name="m_pSampleLinear">�o�͂��邽�߂̃T���v���X�e�[�g</param>
/// <param name="samp">�쐬����T���v����ގw��</param>
/// <returns>����</returns>
HRESULT cTexManager::CreateTexSampler(ID3D11SamplerState** m_pSampleLinear, SampleState samp)
{
	//�e�N�X�`���[�p�T���v���[�쐬
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));

	switch (samp)
	{
	case cTexManager::WRAP:
		SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case cTexManager::CLAMP:
		SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	default:
		break;
	}
	
	GetDirectX::Device()->CreateSamplerState(&SamDesc, m_pSampleLinear);

	// TODO �����I�ɂ͂����Ǝ��R�Ƀp�����[�^���w�肵�Đݒ�ł���悤�ɂ���B

	return S_OK;
}

/// <summary>
/// �����ŉB�����ꂽ�e�N�X�`���ǂݍ��ݕ����BLoadTexData����Ăяo�����
/// </summary>
/// <param name="_fileName">�t�@�C����</param>
/// <param name="pTex">�o�͂���e�N�X�`��</param>
/// <returns>����</returns>
HRESULT cTexManager::GetTex(std::string _fileName, Texture2D* pTex) {

	if (FAILED(D3DX11CreateShaderResourceViewFromFileA(GetDirectX::Device(), _fileName.c_str(), NULL, NULL, &pTex->vpTexture, NULL))) {
		if (FAILED(D3DX11CreateShaderResourceViewFromFileA(GetDirectX::Device(), _fileName.c_str(), NULL, NULL, &pTex->vpTexture, NULL))) {
			MessageBox(0, "�e�N�X�`���[�ǂݍ��ݎ��s", NULL, MB_OK);
			return E_FAIL;
		}
	}
	return S_OK;
}

/// <summary>
/// ���[�v�̍ŏ��Ƀe�N�X�`����������ꂽ���ǂ������`�F�b�N���邽�߂Ɋ֐��B
/// </summary>
void cTexManager::CheckDelete()
{
	for (auto itr = m_TexMap.begin(); itr != m_TexMap.end(); ++itr) {
		//map�����|�C���^��ێ����Ă��Ȃ�
		if (itr->second.use_count() == 1) {
			m_TexMap.erase(itr->first);		//�f�[�^�폜
			itr = m_TexMap.begin();
			if (itr == m_TexMap.end())
				return;
		}
	}
}