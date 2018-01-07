/*=================================================
//								FILE : GBuffer.h
//		�t�@�C������ :
//		�f�B�t�@�[�h�����_�����O�ŗ��p����G-Buffer���\���̂Ƃ���
//		��`����B
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"

struct GBuffer {
	ID3D11ShaderResourceView* vDiffuse;					//�J���[���
	ID3D11ShaderResourceView* vPosition;					//���W���
	ID3D11ShaderResourceView* vNormal;						//�@��(w�����ɐ[�x�l���i�[)
	ID3D11ShaderResourceView* vHighBrightness;		//���P�x���
	ID3D11ShaderResourceView* vSSSSS;						//SSS

	/// <summary>
	/// G-Buffer���ׂĂ��V�F�[�_�X�e�[�W�ɃZ�b�g����
	/// </summary>
	void SetAllGBuffer() {
		GetDirectX::Context()->PSSetShaderResources(0, sizeof(GBuffer) / sizeof(ID3D11ShaderResourceView*), &vDiffuse);
		GetDirectX::Context()->VSSetShaderResources(0, sizeof(GBuffer) / sizeof(ID3D11ShaderResourceView*), &vDiffuse);
	}

	/// <summary>
	/// �w�肵���o�b�t�@���w��p�X�ɃZ�b�g����
	/// </summary>
	/// <param name="_no">0 = �J���[,1 = ���W,2 = �@��,3 = �A���r�G���g,4 = �X�y�L����,5 = ������,6 = �V���h�E�}�b�v</param>
	/// <param name="_pass"></param>
	void SetSelectView(const int _no,const int _pass) {
		GetDirectX::Context()->PSSetShaderResources(_no, 1, (&vDiffuse + _no));
		GetDirectX::Context()->VSSetShaderResources(_no, 1, (&vDiffuse + _no));
	}
};