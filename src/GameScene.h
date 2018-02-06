/*=================================================
//								FILE :GameScene.h
//		�t�@�C������ :
//		�Q�[���V�[���̊��N���X���쐬����B
//		�Q�[���V�[���ɂ�DX�̃f�o�C�X��J�����A�f�B�N�V���i�����C�g�����ێ�������
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"
#include "DirectXData.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "GBuffer.h"
#include "ToonParam.h"

/// <summary>
/// �V�[���ɓK�p����V�F�[�_�̃t���O���Ǘ�����
/// </summary>
struct SeneRenderFlag {
	bool vShaowFlag;		//�V���h�E��K�p���邩�H
};

//�u�����h�X�e�[�g��ݒ肷����
enum class BrendStateNo {
	NONE = 0,
	ALPHA = 1,
	ADD = 2
};

//�J�����O���@��I������
enum class CullMode {
	NONE = 0,		//�J�����O�Ȃ�
	LEFT,				//����n�\
	RIGHT,			//�E��n�\
	WIRE
};

/// <summary>
/// �e�Q�[���V�[���ɔh����������N���X
/// ���p���鎞�� Scene::Push<�V�[���N���X��>(); �ƋL�q���Ĉ���
/// </summary>
class cGameScene {
public:
	//�X�V���������������s�����߂�cDirectX�N���X���t�����h�����Ă���
	friend class cDirectX11;
	cGameScene();
	virtual ~cGameScene();
	virtual void Update() = 0;
	virtual void MeshDraw() = 0;				//�f�B�t�@�[�h�����_�����O�ŗ��p����Gbuffer���쐬����
	virtual void UIDraw() = 0;				//UI��`�悷��
	
	void SetDefaultRender();
	void SetBlendState(BrendStateNo _OnFlag);
	void SetFogColor(const DirectX::XMFLOAT4& _Color);
	void SetFogFactor(const float _f);
	void SetShadowFlag(bool _Flag);
	void SetRasterizerState(CullMode mode = CullMode::NONE);
	void SetRendBuffer();
	void SetRadialBlur(DirectX::XMFLOAT3 _Pos, float _Delta, int _Num);
	void SetDepthStencilState();
	bool IsRadialBlur();
	RadialBlurData GetRadialBlur(const DirectX::XMFLOAT4X4& _VP);
	const FogData& GetFogData() { return m_Fog; };
	const DoFData& GetDoFData() { return m_DoF; };

	cCamera m_CameraData;
	cDirectionalLight m_DirectionalLight;	//�f�B�N�V���i�����C�g
	SeneRenderFlag m_RenderFlag;				//�K�p����V�F�[�_�t���O

	cToonParam m_ToonParam;		//�g�D�[���p�����[�^�p�N���X
private:
	static void SetDirectXData(DirectXData _data);
	static DirectXData m_DirectXData;		//�V�X�e���f�[�^���V�[���S�̂ŕێ�����
	static DirectXDrawData m_TexDrawData;
	
	FogData m_Fog;	//�t�H�O�̏��
	DoFData m_DoF;	//��ʊE�[�x���
	RadialBlurBuf m_Blur;	//���W�A���u���[�f�[�^
};