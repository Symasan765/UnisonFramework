/*=================================================
//								FILE : Animation.cpp
//		�t�@�C������ :
//		���b�V���f�[�^�̃A�j���[�V�����������N���X�A�\���̂��`����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "Animation.h"
#include "Mesh3D.h"

using namespace DirectX;

/// <summary>
/// �����o�ϐ��̏��������s��
/// </summary>
cAnimationData::cAnimationData()
{
	m_AnimeCnt = 0;
	m_MotionNum = 0;
	m_NowMotionNo = -1;
	m_pMotion = nullptr;
	m_pBone = nullptr;
	m_pBoneNum = 0;
}

/// <summary>
/// �������
/// </summary>
cAnimationData::~cAnimationData()
{
	delete[] m_pMotion;
	delete[] m_pBone;
}

/// <summary>
/// �w�肵���A�j���[�V�����ԍ������Ƀ{�[�������X�V����
/// </summary>
/// <param name="_AnimeNo">�������A�j���ԍ�</param>
/// <returns>�A�j���[�V����������ꂽ���ǂ����H</returns>
bool cAnimationData::ToAnimetion(int _AnimeNo)
{
	//�܂��A�j���[�V�����ԍ������݂��邩�`�F�b�N����
	if (m_MotionNum >= _AnimeNo) {
		//�O�񂩂�A�j���[�V�������ς���������`�F�b�N����
		if (m_NowMotionNo != _AnimeNo) {
			m_NowMotionNo = _AnimeNo;
			m_AnimeCnt = 0;		//�V�������[�V�����ɓ������̂ŃJ�E���g��0��
		}
		// TODO �A�j���[�V�����̍X�V�����֐��������ɋL�q����
		m_AnimeCnt++;			//�J�E���g
		return true;
	}
	return false;		//�A�j���[�V�����ł��Ȃ�����
}

/// <summary>
/// �A�j���[�V���������擾���ăV�F�[�_�ɓn��
/// </summary>
/// <param name="_AnimeNo"></param>
/// <param name="_Time"></param>
void cAnimationData::SetBoneConst(int _AnimeNo, int _Time)
{
	//�K�v�ȏ����͂��ꂼ��̍s����ƃA�j���[�V�����̍ŏI��Ԏp��
	//�Đ�����A�j���[�V�����̒��̓������{�[����������������

	if (m_pBoneNum > 0) {
		int Suf = 0;		//�ǂ��̓Y�����ԍ��̂Ƃ��낪�ŏI���W��
		int MaxKey = m_pMotion[_AnimeNo].vBoneAnime[0].vKeyNum;
		//�܂��͎��ԏ���͈͓��Ɏ��߂�
		_Time %= m_pMotion[_AnimeNo].vBoneAnime[0].vpKeyFrame[MaxKey - 1].vTime;
		bool KeyTimeFindFlag = false;		//�L�[�t���[�������������̃t���O
		for (int i = 0; i < MaxKey; i++) {
			//�����烂�[�V��������t���[���̒�����^�C�����߂����̂�T���o��
			if (m_pMotion[_AnimeNo].vBoneAnime[0].vpKeyFrame[i].vTime > _Time) {
				Suf = i;		//�b��ł��̓Y�����𓮂������ƂɂȂ�
				if (!KeyTimeFindFlag)
					break;
			}
		}

		//��Ԃ̊������Z�o����
		const int TargetTime = m_pMotion[_AnimeNo].vBoneAnime[0].vpKeyFrame[Suf].vTime;			//��Ԃ���^�C�����擾����
		float t = 0;//��Ԃ���ۂ̊���
					//�[�����Z���
		if (TargetTime > 0) {
			t = (float)_Time / (float)TargetTime;
		}
		else {
			t = 1.0f;		//�܂��ڕW��0�Ȃ�\��������̂̓^�C��0�ł��傤��
		}
		//t = t * t * (3 - 2 * t);
		if (t > 1.0f) t = 1.0f;	//�͈͌Œ�

		// i == ���ꂩ�瓮�����{�[���̔ԍ��ƂȂ�
		for (int i = 0; i < m_pMotion[_AnimeNo].vBoneNum; i++) {
			//�܂��͓������ŏI�p�������o��
			// TODO �A�j���[�V�����̕�Ԃ��s����悤�ɂ��邱��
			int BoneNo = m_pMotion[_AnimeNo].vBoneAnime[i].vBoneNo;		//�������{�[���ԍ�
			//�{�[���ԍ���-1�͑��݂��Ă��Ȃ��{�[��
			if (BoneNo != -1) {
				
				int Former = Suf - 1;		//��O�̃t���[�����Ԃ���
				if (Former < 0) Suf = 0;	//�}�C�i�X�̒l�Ȃ�[���ɂȂ��Ă�
				DirectX::XMFLOAT4X4 Before = m_pMotion[_AnimeNo].vBoneAnime[i].vpKeyFrame[Former].vMat;	//�ϊ����̃f�[�^
				DirectX::XMFLOAT4X4 After = m_pMotion[_AnimeNo].vBoneAnime[i].vpKeyFrame[Suf].vMat;		//���ꂪ��Ԃ���ŏI�f�[�^

				//=====================��Ԃ̏������s���Ă���=====================
				//�ړ��̕�ԏ��� TODO ��x���`��Ԃ̃R�[�h�𒼏������邪�֐������Ă�������
				XMVECTOR BeforePos = XMLoadFloat3(&XMFLOAT3{ Before._41, Before._42 ,Before._43 });
				XMVECTOR AfterPos = XMLoadFloat3(&XMFLOAT3{ After._41, After._42 ,After._43 });
				XMFLOAT3 LerpPosBuf{ 0,0,0 };
				XMVECTOR LerpPos = AfterPos * t + BeforePos * (1.0f - t);		//��Ԍ�̍��W
				XMStoreFloat3(&LerpPosBuf, LerpPos);

				//��]�̕�Ԃ��s��
				Before._41 = Before._42 = Before._43 = Before._44 = 0.0f;
				After._41 = After._42 = After._43 = After._44 = 0.0f;
				XMVECTOR BeforeRot = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&Before));
				XMVECTOR AfterRot = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&After));
				XMVECTOR LerpRot = XMQuaternionSlerp(BeforeRot, AfterRot, t);		//��Ԍ�̃N�H�[�^�j�I�����i�[

				//�쐬������ԏ����܂Ƃ߂�
				XMMATRIX LerpMat = DirectX::XMMatrixRotationQuaternion(LerpRot) * XMMatrixTranslation(LerpPosBuf.x, LerpPosBuf.y, LerpPosBuf.z);

				//=================������f�[�^���V�F�[�_�փZ�b�g����=================
				DirectX::XMStoreFloat4x4(&m_pBone[BoneNo].vTransPose, LerpMat);
			}
		}

		MeshManager::GetInstance().SetBoneConstant(m_pBone, m_pBoneNum);
	}
}
