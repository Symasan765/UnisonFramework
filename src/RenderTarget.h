/*=================================================
//								FILE : RenderTarget.h
//		�t�@�C������ :
//		�����_�[�^�[�Q�b�g�������₷���悤�ɃN���X������
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"
#include "DirectXData.h"

struct RTTex
{
	ID3D11RenderTargetView* m_pDepthRenderTargetView;    //�`�掞�Ƀ����_�[�^�[�Q�b�g�̐ݒ�ƃN���A�Ŏg�p����B
	ID3D11DepthStencilView* m_pDepthStencilView;				  //�`�掞�Ƀ����_�[�^�[�Q�b�g�̐ݒ�ƃN���A�Ŏg�p����B
	ID3D11ShaderResourceView* m_pTextureView;        //2�p�X�ڂɗ��p����B
	D3D11_VIEWPORT m_ViewPort;
	float ClereColor[4];
};

/// <summary>
/// �����_�[�^�[�Q�b�g�r���[�Ɛ[�x�X�e���V���r���[������B
/// �������J���[�A���������A�t�H�[�}�b�g�Őݒ�\�B
/// �e�N�X�`���N���X���p�����Đݒ肷�邱�ƂŃ����_�[�^�[�Q�b�gON�Ńe�N�X�`�����쐬����
/// �`��Z�b�g���͐ݒ�p�X���w��ł���悤�ɂ��邱��
/// �g�p����Ƃ���Create���Ăяo���Ă��痘�p���钼�O��SetRender���Ă�
/// </summary>
class cRenderTargetTex {
public:
	cRenderTargetTex();		//��{�I�ɏ���������
	~cRenderTargetTex();		//��{�I�ɊJ������
	HRESULT RTCreate(const float _width,const float _height,DirectX::XMUINT3 _initColor, DXGI_FORMAT _format = DXGI_FORMAT_R32G32B32A32_FLOAT);
	/// <summary>
	/// ���̊֐����Ăԏꍇ�A�K�v�ł���΃R���e�L�X�g�n���Q�b�g���ĕێ����Ă������ƁB
	/// ��{�I��1���̃^�[�Q�b�g�ł����ꍇ�������p����B
	/// �������K�v�ł���΃e�N�X�`����r���[���Q�b�g���Ă����𗘗p����B
	/// </summary>
	/// <param name="_pathNo">�V�F�[�_�p�X�ԍ�</param>
	void SetRenderTargetTex(const int _pathNo = 0);
	RTTex GetRenderTextureStruct();			//��{�I�ɂ̓A�v��������͎g��Ȃ��B�f�B�t�@�[�h�����_�����O�ɏ����ʂɑ���̂ł܂Ƃ߂Ă邾��
	ID3D11ShaderResourceView* GetTextureResourceView();
	ID3D11DepthStencilView* GetDepthStencilView();
	DirectXDrawData GetDrawData();
private:
	HRESULT RenderTex(const float _width, const float _height, DXGI_FORMAT _format);
	HRESULT DepthStencilView(const float _width, const float _height, DXGI_FORMAT _format);
	HRESULT ShaderResourceView(const float _width, const float _height, DXGI_FORMAT _format);

	DirectX::XMUINT3 m_InitColor;		//��ʃN���A�J���[

	ID3D11Texture2D* m_pDepthTexture;								 //���̌�̐ݒ�ŕK�v�B�`�掞�ɂ͎g��Ȃ�
	ID3D11RenderTargetView* m_pDepthRenderTargetView;    //�`�掞�Ƀ����_�[�^�[�Q�b�g�̐ݒ�ƃN���A�Ŏg�p����B
	ID3D11Texture2D* m_pDepthStencil;								//�`�掞�ɂ͗��p���Ȃ��B
	ID3D11DepthStencilView* m_pDepthStencilView;				  //�`�掞�Ƀ����_�[�^�[�Q�b�g�̐ݒ�ƃN���A�Ŏg�p����B
	ID3D11ShaderResourceView* m_pTextureView;        //2�p�X�ڂɗ��p����B
	D3D11_VIEWPORT m_ViewPort;
};