/*=================================================
//								FILE : Shader.h
//		�t�@�C������ :
//		�V�F�[�_�̃��[�h��̈�m�ۂ��s���N���X���`����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once

#include <string>
#include <memory>
#include <map>
#include "WinMain.h"
#include "ShaderStruct.h"

//�e��V�F�[�_���i�[�ł���
struct ShaderData {
	ID3D11VertexShader* vpVertexShader;
	ID3D11GeometryShader* vpGeometryShader;
	ID3D11PixelShader* vpPixelShader;
	ID3D11InputLayout* vpInputLayout;

	void Set() {
		GetDirectX::Context()->IASetInputLayout(vpInputLayout);

		GetDirectX::Context()->VSSetShader(vpVertexShader, NULL, 0);
		GetDirectX::Context()->GSSetShader(vpGeometryShader, NULL, 0);
		GetDirectX::Context()->PSSetShader(vpPixelShader, NULL, 0);
	}

	~ShaderData() {
		SAFE_RELEASE(vpVertexShader);
		SAFE_RELEASE(vpGeometryShader);
		SAFE_RELEASE(vpPixelShader);
		SAFE_RELEASE(vpInputLayout);
	}

	//�߂�l��GS�̗L��
	bool release() {
		bool GSflag = false;
		if (vpGeometryShader) GSflag = true;
		SAFE_RELEASE(vpVertexShader);
		SAFE_RELEASE(vpGeometryShader);
		SAFE_RELEASE(vpPixelShader);
		/*SAFE_RELEASE(vpInputLayout);*/		//�C���v�b�g���C�A�E�g�͍X�V�̕K�v�Ȃ�
		return GSflag;
	}
};

//�擾����C���v�b�g���C�A�E�g���w�肷��(�r�b�g���Z)
enum InpLayout {
	inPOSITION = 1,
	inNORMAL = 2,
	inTEX_UV = 4,
	inBone = 8,
	inWeight = 16
};

/// <summary>
/// �V�F�[�_�̊Ǘ����s���B�V���O���g��
/// </summary>
class SHADER {
public:
	//�X�V���������������s�����߂�cDirectX�N���X���t�����h�����Ă���
	friend class cDirectX11;
	SHADER(const SHADER&) = delete;
	SHADER& operator=(const SHADER&) = delete;
	SHADER(SHADER&&) = delete;
	SHADER& operator=(SHADER&&) = delete;

	static SHADER* GetInstance() {
		static SHADER inst;			// private�ȃR���X�g���N�^���Ăяo��
		return &inst;
	}

	std::shared_ptr<ShaderData> LoadShaderFile(std::string _fileName, const short bitFlag, bool _GSflag = false);
	
	template<typename T>
	static bool GetConstBuffer(ID3D11Buffer** _constBuf) {
		D3D11_BUFFER_DESC cb1;
		cb1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cb1.ByteWidth = sizeof(T);
		cb1.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cb1.MiscFlags = 0;
		cb1.StructureByteStride = 0;
		cb1.Usage = D3D11_USAGE_DYNAMIC;

		if (FAILED(GetDirectX::Device()->CreateBuffer(&cb1, NULL, _constBuf)))
		{
			return false;
		}
		return true;
	}

	template<typename T>
	static bool SetConstBufferData(ID3D11Buffer* _constBuf, T* data) {
		bool succes = false;
		D3D11_MAPPED_SUBRESOURCE pData1;
		if (SUCCEEDED(GetDirectX::Context()->Map(_constBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData1)))
		{
			memcpy_s(pData1.pData, pData1.RowPitch, (void*)(data), sizeof(T));
			succes = true;
		}
		GetDirectX::Context()->Unmap(_constBuf, 0);

		return succes;
	}

	HRESULT ShaderReload(std::string _fileName);
private:
	HRESULT LoadShader(std::string _ShaderFileName, const short bitFlag, ShaderData* _data, bool _GSflag);
	HRESULT GetInputLayout(ShaderData* _data, ID3DBlob* blob, const short bitFlag);
	void CheckDelete();
	SHADER();
	~SHADER();

	std::map<std::string, std::shared_ptr<ShaderData>> m_Map;		//�e�b�Z���[�V������ێ�����}�b�v

	//================�f�t�H���g�̒萔�o�b�t�@===================//
	ID3D11Buffer* ConstWorldMtx;		//���[���h�A�v���W�F�N�V�����s���]������萔�o�b�t�@
	ID3D11Buffer* ConstLightVec;		//���C�g�A�J�������W���Ǘ�
	ID3D11Buffer* ConstMaterial;		//�}�e���A�����Ǘ�
};
