/*=================================================
//								FILE : MemoryMonitor.cpp
//		�t�@�C������ :
//		�������[�g�p�󋵂��擾���Ǘ�����N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "MemoryMonitor.h"

MemoryData cMemoryMonitor::GetMemoryData() const
{
	return m_MemoryData;
}

/// <summary>
/// �������̗��p�󋵂��X�V����
/// </summary>
void cMemoryMonitor::Update()
{
	MEMORYSTATUSEX msex = { sizeof(MEMORYSTATUSEX) };
	GlobalMemoryStatusEx(&msex);
	//�g�p��������
	m_MemoryData.vUsed = (msex.ullTotalPhys - msex.ullAvailPhys);
	//�󂫗e��
	m_MemoryData.vFree = (msex.ullAvailPhys);
}

cMemoryMonitor::cMemoryMonitor()
{
	MEMORYSTATUSEX msex = { sizeof(MEMORYSTATUSEX) };
	GlobalMemoryStatusEx(&msex);

	//�����ł̃������g�p��
	m_MemoryData.vInit = (msex.ullTotalPhys - msex.ullAvailPhys);
	//�����������ʎ擾
	m_MemoryData.vSize = (msex.ullTotalPhys);
	//�g�p��������
	m_MemoryData.vUsed = (msex.ullTotalPhys - msex.ullAvailPhys);
	//�󂫗e��
	m_MemoryData.vFree = (msex.ullAvailPhys);
}
