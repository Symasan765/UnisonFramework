/*=================================================
//								FILE : TimeCheck.h
//		�t�@�C������ :
//		���Ԍv�����s���N���X���`����B
//		���̂Ƃ��Ă̓^�C�}�[���ӂ��킵�������O�̋������|���̂�TimeCheck�Ƃ���
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"

/// <summary>
/// ���Ԍv�����s���N���X
/// </summary>
class cTimeCheck {
public:
	cTimeCheck();
	~cTimeCheck() = default;					//��������Ƃ��ɂȂ�����
	void TimerStart();
	void TimerEnd();
	float GetProcessingTime()const;		//���������������Ԃ�float�ŕԂ��֐�
private:
	LARGE_INTEGER m_Start, m_End;		//�����J�n�̎��ԂƏI�����Ԃ��i�[����ϐ�
	static LARGE_INTEGER m_Freq;		//�P�ʎ��Ԃ��i�[�B�֐��ŋ��߂����l�����̒l�Ŋ����sec�P�ʂɕϊ��ł���B
};