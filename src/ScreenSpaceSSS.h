/*=================================================
//								FILE : ScreenSpaceSSS.h
//		�t�@�C������ :
//		�X�N���[���X�y�[�X�T�u�T�[�t�F�X�X�L���b�^�����O�̃|�X�g�G�t�F�N�g����
//		���s���V�F�[�_��ێ�����N���X
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"

class cScreenSpaceSSS : public cPostEffects{
public:
	cScreenSpaceSSS();
	~cScreenSpaceSSS();

	void DrawSSS(ID3D11ShaderResourceView * _sssss, ID3D11ShaderResourceView * _normal, ID3D11ShaderResourceView * _color);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cRenderTargetTex* m_pBufRenderTarget;		//�ꎞ�I�ɕ`����s���e�N�X�`��
};