/*=================================================
//								FILE : ProcessMonitor.h
//		�t�@�C������ :
//		�v���O��������CPU�g�p���𒲂ׂ�N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include <pdh.h>

#define MAX_PROCESSORS_COUNT (36 + 1)		//�v���\�v���Z�b�T���Bcore i9��18�R�A(36�X���b�h)�ɕ��ϒl��+1��z��


class cProcessMonitor {
public:
	cProcessMonitor();
	~cProcessMonitor();
	int GetProcessRate(const int _No);
	int GetProcessorsNum()const;
private:
	PDH_HQUERY m_hQuery[MAX_PROCESSORS_COUNT];
	PDH_HCOUNTER m_hCounter[MAX_PROCESSORS_COUNT];
	int m_ProcessorsNum;			//�v���Z�b�T��
};