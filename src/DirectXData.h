/*=================================================
//								FILE : DirectXData.h
//		�t�@�C������ :
//		DirectX���쐬�����f�[�^���Ǘ����邽�߂̍\���̂��`����B
//		��ɃQ�[���V�[�����ŕۊǂ���ړI�Ŏg�p����
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"

/// <summary>
/// DirectX���쐬�����f�[�^��ێ����A�Ǘ�����
/// </summary>
struct DirectXData {
	ID3D11Device* v_pD3DDevice;									//�f�o�C�X�n���h��
	ID3D11DepthStencilState* v_pDepthStencilState;
	ID3D11DeviceContext* v_pImmediateContext;			// �f�o�C�X�E�R���e�L�X�g
	IDXGISwapChain* v_pSwapChain;								 // �X���b�v�E�`�F�C��
	ID3D11RenderTargetView* v_pRenderTargetView;	//�^�[�Q�b�g�r���[
	ID3D11Texture2D* v_pDepthStencil;						//�[�x�E�X�e���V��
	ID3D11DepthStencilView* v_pDepthStencilView;		//�[�x�X�e���V���r���[
	D3D11_VIEWPORT v_ViewPort;									//�r���[�|�[�g
	ID3D11RasterizerState* v_pCullNoneRasterizerState;		//���X�^���C�U
	ID3D11RasterizerState* v_pCullLeftRasterizerState;
	ID3D11RasterizerState* v_pCullRightRasterizerState;
	ID3D11RasterizerState* v_pWIREFRAMERasterizerState;
	ID3D11BlendState* v_pBlendState;							//�f�t�H���g�u�����h�X�e�[�g
	ID3D11BlendState* v_pAlphaState;							//�A���t�@�u�����h�X�e�[�g
	ID3D11BlendState* v_pAddState;							//���Z����
};

/// <summary>
/// �悭���p����f�o�C�X�ƃR���e�L�X�g���܂Ƃ߂��\����
/// </summary>
struct DirectXDevice {
	ID3D11Device* v_pD3DDevice;									//�f�o�C�X�n���h��
	ID3D11DeviceContext* v_pImmediateContext;			// �f�o�C�X�E�R���e�L�X�g
};

/// <summary>
/// �`��ɂ悭���p���郌���_�[�^�[�Q�b�g�Ȃǂ����ꂽ�\����
/// </summary>
struct DirectXDrawData {
	D3D11_VIEWPORT v_ViewPort;									//�r���[�|�[�g
	ID3D11ShaderResourceView* v_ShaderResourceView;
	ID3D11RenderTargetView* v_pRenderTargetView;	//�^�[�Q�b�g�r���[
	ID3D11DepthStencilView* v_pDepthStencilView;		//�[�x�X�e���V���r���[	ID3D11DepthStencilState* v_pDepthStencilState;
};