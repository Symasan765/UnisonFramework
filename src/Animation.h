/*=================================================
//								FILE : Animation.h
//		�t�@�C������ :
//		���b�V���f�[�^�̃A�j���[�V�����������N���X�A�\���̂��`����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"
#include "BoneStruct.h"

/// <summary>
/// 1�L�[�t���[�����Ǘ�����\����
/// </summary>
struct KeyFrame {
	int vTime;								//�A�j���[�V��������t���[��
	DirectX::XMFLOAT4X4 vMat;		//��Ԃ���p��

	KeyFrame() {
		vTime = -1;		//�����^�C��
		DirectX::XMStoreFloat4x4(&vMat, DirectX::XMMatrixIdentity());		//�P�ʍs����Z�b�g
	}
};

/// <summary>
/// ��̃{�[���̃A�j���[�V�������Ǘ�
/// </summary>
struct BoneAnime {
	int vBoneNo;						//�������{�[���ԍ�
	int vKeyNum;						//�L�[�t���[���̐�
	KeyFrame* vpKeyFrame;		//�L�[�t���[��

	BoneAnime() {
		vBoneNo = 0;
		vKeyNum = 0;
		vpKeyFrame = nullptr;
	}

	~BoneAnime() {
		delete[] vpKeyFrame;
	}
};

/// <summary>
/// ��̓���ɑ΂���f�[�^��ێ�����
/// </summary>
struct MotionData {
	int vBoneNum;						//�������{�[����
	int vMaxTime;						//�����Ă���ő�t���[����
	BoneAnime* vBoneAnime;		//�����̃{�[���̃A�j���[�V����

	MotionData() {
		vBoneNum = 0;
		vMaxTime = 0;
		vBoneAnime = nullptr;
	}

	~MotionData() {
		delete[] vBoneAnime;
	}
};

/// <summary>
/// ��̃��f�����ێ�����A�j���[�V�����f�[�^���Ǘ�����
/// </summary>
class cAnimationData {
public:
	//�A�j���[�V�����������炤���߂Ƀt�@�C�����[�_�[���t�����h������
	friend class cXFileLoad;
	cAnimationData();
	~cAnimationData();
	bool ToAnimetion(int _AnimeNo);
	void SetBoneConst(int _AnimeNo,float _Time);			//�{�[������萔�o�b�t�@�ɃZ�b�g����
private:
	MotionData* m_pMotion;		//���앪�̃��[�V������ێ�����
	int m_MotionNum;				//���[�V������
	int m_NowMotionNo;			//���ݓ������Ă��郂�[�V�����ԍ�
	int m_AnimeCnt;					//�A�j���[�V���������Ă���t���[�����[�g���J�E���g���Ă���

	BoneData* m_pBone;						//�{�[���f�[�^
	int m_pBoneNum;								//�{�[����
};