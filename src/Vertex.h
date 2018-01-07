/*=================================================
//								FILE : Vertex.h
//		�t�@�C������ :
//		�V�F�[�_�[��ň������_�\���̂��`����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"

struct VERTEX3D {
	DirectX::XMFLOAT4 vPos;		//���W
	DirectX::XMFLOAT4 vNorm;	//�@��
	DirectX::XMFLOAT2 vTex;		//UV���W
	DirectX::XMUINT4 vBones;	//�K�p�{�[��
	DirectX::XMFLOAT4 vWeight;	//�{�[���̏d��

	VERTEX3D() {
		vPos = { 0.0f,0.0f,0.0f,0.0f };
		vNorm = { 0.0f,0.0f,0.0f,0.0f };
		vTex = { 0.0f,0.0f };
		vBones = { 128,128,128,128 };		//�{�[���ő吔�����Ă����ƃV�F�[�_��Ń{�[���Ȃ��ƂȂ�
		vWeight = { 0.0f,0.0f,0.0f,0.0f };
	}

	VERTEX3D(DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 norm, DirectX::XMFLOAT2 uv, DirectX::XMUINT4 bone,DirectX::XMFLOAT4 weight) {
		vPos = pos;
		vNorm = norm;
		vTex = uv;
		vBones = bone;		//�{�[���ő吔�����Ă����ƃV�F�[�_��Ń{�[���Ȃ��ƂȂ�
		vWeight = weight;
	}
} ;

typedef struct _XYZUV {
	DirectX::XMFLOAT4 vPos;		//���W
	DirectX::XMFLOAT2 vTex;		//UV���W
}XYZUV;