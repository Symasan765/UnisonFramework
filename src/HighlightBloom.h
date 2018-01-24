/*=================================================
//								FILE : HighlightBloom.h
//		�t�@�C������ :
//		�n�C���C�g�����ւ̃u���[���������{��
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "RenderTarget.h"

/// <summary>
/// �u���[�����쐬����N���X
/// </summary>
class cHighlightBloom {
public:
	cHighlightBloom(const cHighlightBloom&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cHighlightBloom& operator=(const cHighlightBloom&) = delete;	//�R�s�[������Z�q�� delete �w��
	cHighlightBloom(cHighlightBloom&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cHighlightBloom& operator=(cHighlightBloom&&) = delete;		//���[�u������Z�q�� delete �w��

	static cHighlightBloom& GetInstance() {
		static cHighlightBloom inst;			// private�ȃR���X�g���N�^���Ăяo��
		return inst;
	}

	void DrawBloom(ID3D11ShaderResourceView* _EmiTex);		//�������F���i�[���ꂽ�e�N�X�`����n��
	ID3D11ShaderResourceView* GetBloomResourceView();
private:
	cHighlightBloom();					//�R���X�g���N�^�� private �ɒu��
	~cHighlightBloom();					//�f�X�g���N�^�� private �ɒu��


	cRenderTargetTex* m_pBoloomTex;		//�o�����u���[���̃e�N�X�`�����i�[���郌���_�[�^�[�Q�b�g�N���X
};