/*=================================================
//								FILE : BillBoard.h
//		�t�@�C������ :
//		3D��ԏ�ŏ�ɃJ����������������2D�e�N�X�`����\������
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include <string>
#include "WinMain.h"
#include "Camera.h"
#include "MatrixCoord.h"
#include "DeferredConst.h"
#include "TextureManager.h"

/// <summary>
/// �r���{�[�h����舵���N���X
/// </summary>
class cBillBoard : public cMatrix {
public:
	cBillBoard();
	cBillBoard(std::string _fileName);		//LoadData�𓯎��ɌĂяo���R���X�g���N�^
	~cBillBoard() = default;
	void LoadData(std::string _fileName);	//�e�N�X�`���̃t�@�C�������w�肷��
	void Draw();
	void SetEmission(DirectX::XMFLOAT4 _color);	//�������F��ݒ肷��
	void SetRotate(const float damy) {};			//�r���{�[�h�̉�]�֐��͕��󂷂�
	void Rotation(const float, const float, const float) {};
private:
	HRESULT CreateVertexBuf();					//�r���{�[�h�N���X�Ŏg���܂킷
	void SetConstant();							//�ۑ����ꂽ�f�[�^��萔�o�b�t�@�ɐݒ肷��
	std::shared_ptr<Texture2D> m_pTex;		//�\��������e�N�X�`����ێ�����
	cDeferredConst m_Constant;				//�萔�o�b�t�@
	int m_ShaderType;								//�r�b�g���Z�����V�F�[�_�ԍ����i�[����
	static ID3D11Buffer* m_VertexBuf;	// TODO ����������Ȃ�Ƃ����Ă����B���̂܂܂��ƃf�X�g���N�^�ɂ����ƃr���{�[�h����폜����ƕs��ɂȂ���
};