/*=================================================
//								FILE : SpriteDraw.h
//		�t�@�C������ :
//		2D�X�v���C�g��`�悷��N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"
#include "ConstantBuffer.h"

//��{�I��static�֐��ō\�����Ă���
//�K�v�ȃV�F�[�_�ƒ��_���͎���ێ�����
class cSprite2DDraw {
public:
	//�X�V���������������s�����߂�cDirectX�N���X���t�����h�����Ă���
	friend class cDirectX11;
	cSprite2DDraw(const cSprite2DDraw&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cSprite2DDraw& operator=(const cSprite2DDraw&) = delete;	//�R�s�[������Z�q�� delete �w��
	cSprite2DDraw(cSprite2DDraw&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cSprite2DDraw& operator=(cSprite2DDraw&&) = delete;		//���[�u������Z�q�� delete �w��

	static cSprite2DDraw& GetInstance() {
		static cSprite2DDraw inst;			// private�ȃR���X�g���N�^���Ăяo��
		return inst;
	}
	void Draw(ID3D11ShaderResourceView* _tex,DirectX::XMFLOAT2 _LeftTop, DirectX::XMFLOAT2 _RightDown);
private:
	cSprite2DDraw();					//�R���X�g���N�^�� private �ɒu��
	~cSprite2DDraw();					//�f�X�g���N�^�� private �ɒu��
	void Create();			//���_�V�F�[�_��V�F�[�_��ǂݍ���

	HRESULT CreateVertex();
	HRESULT CreateShader();

	//2D�X�v���C�g�͓���Ȃ̂ő��̂��̂Ƃ͕��p���Ȃ�
	ID3D11VertexShader* vpVertexShader;
	ID3D11PixelShader* vpPixelShader;
	ID3D11InputLayout* vpInputLayout;
	ID3D11Buffer* vVertexBuf;			//���_�o�b�t�@
	ID3D11SamplerState* m_pSampleLinear;

	cConstBuf<DirectX::XMFLOAT4X4> m_ShaderConst;		//�V�F�[�_�ɓn���萔�o�b�t�@
	float m_NowDepth;			//1�t���[�����ɒl�������A�ォ��`�悳�ꂽ���̒����ɕ`�悷��悤�ɂ���
};