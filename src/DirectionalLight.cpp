#include "DirectionalLight.h"
/*=================================================
//								FILE : DirectionalLight.cpp
//		�t�@�C������ :
//		�Q�[���V�[�����Ǘ�����f�B�N�V���i�����C�g���`����B
//
//							HAL��� : ���{ �Y�V��
=================================================*/

cDirectionalLight::cDirectionalLight()
{
	//�����ݒ�ł͎΂ߏォ��̃��C�g�𓖂Ă�
	SetLightPos({ 1.0f,1.0f,0.0f ,1.0f});
	SetDiffuseColor({ 0.7f,0.7f,0.7f,1.0f });
	SetAmbientColor({ 0.3f,0.3f,0.3f,1.0f });
}

cDirectionalLight::~cDirectionalLight()
{
}

/// <summary>
/// ���ڃJ���[��0.0 ~ 1.0f�̊ԂŎw�肷��
/// </summary>
/// <param name="_diffuse"></param>
void cDirectionalLight::SetDiffuseColor(DirectX::XMFLOAT4 _diffuse)
{
	m_ShaderConst.data.I_Deffuse = _diffuse;
}

/// <summary>
/// �J���[��RGB�l�A������256�i�K�Ŏw�肷��
/// </summary>
/// <param name="_r"></param>
/// <param name="_g"></param>
/// <param name="_b"></param>
void cDirectionalLight::SetDiffuseColor(int _r, int _g, int _b)
{
	m_ShaderConst.data.I_Deffuse = { (float)_r / 255.0f,(float)_g / 255.0f,(float)_b / 255.0f,1.0f };
}

/// <summary>
/// ���ڃJ���[��0.0 ~ 1.0f�̊ԂŎw�肷��
/// </summary>
/// <param name="_ambient"></param>
void cDirectionalLight::SetAmbientColor(DirectX::XMFLOAT4 _ambient)
{
	m_ShaderConst.data.I_Ambient = _ambient;
}

/// <summary>
/// �J���[��RGB�l�A������256�i�K�Ŏw�肷��
/// </summary>
/// <param name="_r"></param>
/// <param name="_g"></param>
/// <param name="_b"></param>
void cDirectionalLight::SetAmbientColor(int _r, int _g, int _b)
{
	m_ShaderConst.data.I_Ambient = { (float)_r / 255.0f,(float)_g / 255.0f,(float)_b / 255.0f,1.0f };
}

/// <summary>
/// ���C�g���W���Z�b�g����
/// </summary>
/// <param name="_pos"></param>
void cDirectionalLight::SetLightPos(DirectX::XMFLOAT4 _pos)
{
	m_ShaderConst.data.LightDir = _pos;
}

/// <summary>
/// �f�[�^���V�F�[�_���ɐݒ肷��
/// </summary>
/// <param name="_passNo"></param>
void cDirectionalLight::SetConstantBuffer(const int _passNo)
{
	//���C�g�ʒu�𐳋K������
	m_ShaderConst.Set(_passNo);
}

DirectX::XMFLOAT3 cDirectionalLight::GetLightPos() const
{
	DirectX::XMFLOAT3 tmp = { m_ShaderConst.data.LightDir.x,m_ShaderConst.data.LightDir.y,m_ShaderConst.data.LightDir.z };
	return tmp;
}
