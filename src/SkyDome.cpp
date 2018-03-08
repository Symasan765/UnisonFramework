#include "SkyDome.h"
#include "WinMain.h"

/// <summary>
/// �X�J�C�h�[����������������
/// </summary>
cSkyDome::cSkyDome()
{
	m_SkyDome = new cDeferredModel;
	m_SkyDome->LoadData("ToonSkyDome.x", 1);
	m_SkyDome->Rotation(-90.0f, 0.0, 0.0f);
	m_SkyDome->Scaling(5.0f);
	m_RotAngle = 0.0f;

	//�V�F�[�_�f�[�^���[�h�BIA���Ȃǂ͂��ׂăf�B�t�@�[�h�����_�����O������OK
	m_Shader = SHADER::GetInstance()->LoadShaderFile("DeferredSkyDraw.hlsl", inPOSITION | inNORMAL | inTEX_UV | inBone | inWeight, true);

	// �[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_DEPTH_STENCIL_DESC DepthStencil;
	DepthStencil.DepthEnable = false; // �[�x�e�X�g����
	DepthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // ��������
	DepthStencil.DepthFunc = D3D11_COMPARISON_LESS; // ��O�̕��̂�`��
	DepthStencil.StencilEnable = FALSE; // �X�e���V���E�e�X�g�Ȃ�
	DepthStencil.StencilReadMask = 0;     // �X�e���V���ǂݍ��݃}�X�N�B
	DepthStencil.StencilWriteMask = 0;     // �X�e���V���������݃}�X�N�B
										   // �ʂ��\�������Ă���ꍇ�̃X�e���V���E�e�X�g�̐ݒ�
	DepthStencil.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilFunc = D3D11_COMPARISON_NEVER; // ��Ɏ��s
																 // �ʂ����������Ă���ꍇ�̃X�e���V���E�e�X�g�̐ݒ�
	DepthStencil.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // ��ɐ���
	HRESULT hr = GetDirectX::Device()->CreateDepthStencilState(&DepthStencil,
		&m_pDepthStencilState);
}

/// <summary>
/// �������
/// </summary>
cSkyDome::~cSkyDome()
{
	delete m_SkyDome;
	SAFE_RELEASE(m_pDepthStencilState);
}

/// <summary>
/// �X�J�C�h�[����`�悷��
/// </summary>
/// <param name="camera"></param>
void cSkyDome::DrawSkyDome(CameraData camera)
{
	m_SkyDome->SetTrans(camera.vPos);
	m_SkyDome->SetRotate(-90.0f, m_RotAngle,0.0f);
	m_RotAngle += 0.02f;
	m_Shader->Set();

	//�X�J�C�h�[���͕K�������~�ł����̂�Z�e�X�g��OFF�ɂ���
	GetDirectX::Context()->OMSetDepthStencilState(m_pDepthStencilState, 1);
	m_SkyDome->DrawMesh();
}