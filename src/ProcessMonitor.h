/*=================================================
//								FILE : ProcessMonitor.h
//		ファイル説明 :
//		プログラム中のCPU使用率を調べるクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include <pdh.h>

#define MAX_PROCESSORS_COUNT (36 + 1)		//計測可能プロセッサ数。core i9の18コア(36スレッド)に平均値の+1を想定


class cProcessMonitor {
public:
	cProcessMonitor();
	~cProcessMonitor();
	int GetProcessRate(const int _No);
	int GetProcessorsNum()const;
private:
	PDH_HQUERY m_hQuery[MAX_PROCESSORS_COUNT];
	PDH_HCOUNTER m_hCounter[MAX_PROCESSORS_COUNT];
	int m_ProcessorsNum;			//プロセッサ数
};