/*=================================================
//								FILE : BoneStruct.h
//		�t�@�C������ : 
//		�{�[�������i�[����\���̂��`����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <string>
#include <vector>


// TODO X�t�@�C���̓ǂݍ��݂ňꎞ�I�ɍ�������́B����K������
struct Bone {
	std::string vBoneName;		//�{�[���̖��O
	int vBoneIndex;					//�{�[���̃C���f�b�N�X�ԍ�
	DirectX::XMFLOAT4X4 vTransform;		//�p��
	DirectX::XMFLOAT4X4 vBindPose;			//�����p��
	int vNumChild;									//�q�̐�
	std::vector<Bone*> vpChild;				//���̎��Ԃ��i�[���Ă���

	Bone() {
		//������
		vBoneIndex = 0;
		DirectX::XMStoreFloat4x4(&vTransform, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&vBindPose, DirectX::XMMatrixIdentity());
		vNumChild = 0;
	}
	~Bone() {
		//vector�̓����ɓ������{�[�����������
		for (int i = 0; i < vNumChild; i++) {
			delete vpChild[i];
			vpChild[i] = nullptr;
		}
	}
};

//Mesh�Ɏ������鐳���ȃf�[�^
struct BoneData {
	std::string vName;		//�{�[����
	int vParentNo;			//�e�ԍ�
	DirectX::XMFLOAT4X4 vOffset;
	DirectX::XMFLOAT4X4 vTransPose;		//�ړ����̃|�[�Y
	DirectX::XMFLOAT4X4 vInv;			//�����p��

	BoneData() {
		vParentNo = -1;		//�e�Ȃ�
		//�P�ʍs����Z�b�g
		DirectX::XMStoreFloat4x4(&vOffset, DirectX::XMMatrixIdentity());
		vInv = vTransPose = vOffset;
	}
};