/*=================================================
//								FILE : ProcessMonitor.cpp
//		�t�@�C������ :
//		�v���O��������CPU�g�p���𒲂ׂ�N���X
//		����̓f�o�b�O���[�h���̂ݗ��p����
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "ProcessMonitor.h"
#include <iostream>

/// <summary>
/// �R���X�g���N�^�Ńv���Z�b�T�����擾����
/// </summary>
cProcessMonitor::cProcessMonitor()
{
#if defined(DEBUG) || defined(_DEBUG)
	//�R�A�����擾����
	m_ProcessorsNum = atoi(getenv("NUMBER_OF_PROCESSORS"));
	for (int i = 0; i < m_ProcessorsNum + 1; i++) {
		if (PdhOpenQuery(NULL, 0, &m_hQuery[i]) != ERROR_SUCCESS) {
			MessageBox(0, "�v���Z�b�T���擾���s", NULL, MB_OK);
		}
	}

	for (int i = 0; i < m_ProcessorsNum; i++) {
		char query[128];
		sprintf(query, "\\Processor(%d)\\%% Processor Time", i);
		PdhAddCounter(m_hQuery[i], query, 0, &m_hCounter[i]);
		PdhCollectQueryData(m_hQuery[i]);
	}
	PdhAddCounter(m_hQuery[m_ProcessorsNum], "\\Processor(_Total)\\% Processor Time", 0, &m_hCounter[m_ProcessorsNum]);
	PdhCollectQueryData(m_hQuery[m_ProcessorsNum]);
#endif
}

cProcessMonitor::~cProcessMonitor()
{
#if defined(DEBUG) || defined(_DEBUG)
	//�������
	for (int i = 0; i < m_ProcessorsNum + 1; i++)
		PdhCloseQuery(m_hQuery[i]);
#endif
}

/// <summary>
/// �w�肵���v���Z�b�T�ԍ���CPU�g�p����Ԃ�
/// </summary>
/// <param name="_No"></param>
/// <returns></returns>
int cProcessMonitor::GetProcessRate(const int _No)
{
#if defined(DEBUG) || defined(_DEBUG)
	PDH_FMT_COUNTERVALUE    fntValue;
	PdhCollectQueryData(m_hQuery[_No]);
	PdhGetFormattedCounterValue(m_hCounter[_No], PDH_FMT_LONG, NULL, &fntValue);

	return (int)fntValue.longValue;
#else
	return 0;
#endif
}

int cProcessMonitor::GetProcessorsNum() const
{
#if defined(DEBUG) || defined(_DEBUG)
	return m_ProcessorsNum;
#else
	return 0;
#endif
}
