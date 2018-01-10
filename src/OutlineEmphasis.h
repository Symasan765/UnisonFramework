/*=================================================
//								FILE : OutlineEmphasis.h
//		�t�@�C������ :
//		�֊s�������������s���V�F�[�_�N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"

class cOutlineEmphasis : public cPostEffects {
public:
	cOutlineEmphasis();
	~cOutlineEmphasis();

	void DrawOutLine(ID3D11ShaderResourceView * _color, ID3D11ShaderResourceView * _normal);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cRenderTargetTex* m_pBufRenderTarget;		//�ꎞ�I�ɕ`����s���e�N�X�`��
};