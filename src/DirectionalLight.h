/*=================================================
//								FILE : DirectionalLight.h
//		�t�@�C������ :
//		�Q�[���V�[�����Ǘ�����f�B�N�V���i�����C�g���`����B
//		
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"
#include "ShaderStruct.h"
#include "ConstantBuffer.h"

/// <summary>
/// �f�B�N�V���i�����C�g�N���X�B�V�[�����̊�����ێ�����B
/// </summary>
class cDirectionalLight {
public:
	cDirectionalLight();
	~cDirectionalLight();
	// TODO �ȑO�쐬�����ܓx�ƌo�x���瑾�z�ʒu���Z�o����v���O������g�ݍ��݂���
	void SetDiffuseColor(DirectX::XMFLOAT4 _diffuse);
	void SetDiffuseColor(int _r,int _g,int _b);
	void SetAmbientColor(DirectX::XMFLOAT4 _ambient);
	void SetAmbientColor(int _r, int _g, int _b);
	void SetLightPos(DirectX::XMFLOAT4 _pos);
	void SetConstantBuffer(const int _passNo);			//�����V�F�[�_�ɃZ�b�g����
	DirectX::XMFLOAT3 GetLightPos()const;
private:
	cConstBuf<DirectionalLightData> m_ShaderConst;
};