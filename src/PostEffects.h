/*=================================================
//								FILE : PostEffects.h
//		�t�@�C������ :
//		�|�X�g�G�t�F�N�g�����������₷������悤�ȃN���X���쐬
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "Shader.h"
#include "ConstantBuffer.h"
#include <string>

/// <summary>
/// �|�X�g�G�t�F�N�g����舵���N���X
/// </summary>
class cPostEffects {
public:
	friend class cDirectX11;	//����������s��

	//�e�N�X�`���ɓK�p����T���v���B�r�b�g���Z�ɑΉ�������
	enum Sampler {
		LINEAR = 1,
		POINT = 2
	};
	cPostEffects(std::string _fileName);
	virtual ~cPostEffects();
	void SetTexSamplerState(const int slot, Sampler flag);
	
protected:
	void Draw(ID3D11ShaderResourceView** _tex, int _ResourceNum = 1);
	void SetShader();
private:
	static void Destroy();
	
	void Create();
	HRESULT CreateVertex();
	std::shared_ptr<ShaderData> m_Shader;		//�����V�F�[�_

	//�������牺�̓N���X�S�̂ŋ��L����
	static ID3D11Buffer* vVertexBuf;			//���_�o�b�t�@
	static ID3D11SamplerState* m_pSampleLinear;
	static ID3D11SamplerState* m_pSamplePoint;
	cConstBuf<DirectX::XMFLOAT4X4> m_ShaderConst;		//�V�F�[�_�ɓn���萔�o�b�t�@
};