/*=================================================
//								FILE : TimeCheck.cpp
//		�t�@�C������ :
//		���Ԍv�����s���N���X���`����B
//		���̂Ƃ��Ă̓^�C�}�[���ӂ��킵�������O�̋������|���̂�TimeCheck�Ƃ���
//		�Q�l�T�C�g : http://vivi.dyndns.org/tech/cpp/timeMeasurement.html �l
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "TimeCheck.h"

//���t�@�C�����痘�p���邱�Ƃ��Ȃ�
namespace {
	bool  g_UseFlag = false;		//�g�p�ς� = true : ���g�p = false
}
LARGE_INTEGER cTimeCheck::m_Freq;

/// <summary>
/// �^�C�}�̏��������s��
/// </summary>
cTimeCheck::cTimeCheck()
{
	//�P�ʎ��Ԃ̓^�C�}�̎��g���Ńv���O�������s���ɕύX����Ȃ������Ȃ̂ň�x�����̑����OK
	if (!g_UseFlag) {
		QueryPerformanceFrequency(&m_Freq);
		g_UseFlag = true;
	}

	//�������̕ς��ɃN���X�쐬���̃^�C����start��end�Ɋi�[���Ă���
	QueryPerformanceCounter(&m_Start);
	QueryPerformanceCounter(&m_End);
}

/// <summary>
/// ���Ԍv�����J�n����
/// </summary>
void cTimeCheck::TimerStart()
{
	QueryPerformanceCounter(&m_Start);
}

/// <summary>
/// ���Ԍv�����I������
/// </summary>
void cTimeCheck::TimerEnd()
{
	QueryPerformanceCounter(&m_End);
}

/// <summary>
/// �v���������Ԃ�b�P�ʂɕϊ�����float�ŕԂ��֐�
/// </summary>
/// <returns></returns>
float cTimeCheck::GetProcessingTime() const
{
	//���肵�����Ԃ�b�P�ʂɕϊ�����float�l�Ƃ��ĕԂ�
	return (float)(m_End.QuadPart - m_Start.QuadPart) / m_Freq.QuadPart;
}
