/*=================================================
//								FILE : ProcessMonitor.cpp
//		ファイル説明 :
//		プログラム中のCPU使用率を調べるクラス
//		測定はデバッグモード時のみ利用する
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "ProcessMonitor.h"
#include <iostream>

/// <summary>
/// コンストラクタでプロセッサ数を取得する
/// </summary>
cProcessMonitor::cProcessMonitor()
{
#if defined(DEBUG) || defined(_DEBUG)
	//コア数を取得する
	m_ProcessorsNum = atoi(getenv("NUMBER_OF_PROCESSORS"));
	for (int i = 0; i < m_ProcessorsNum + 1; i++) {
		if (PdhOpenQuery(NULL, 0, &m_hQuery[i]) != ERROR_SUCCESS) {
			MessageBox(0, "プロセッサ数取得失敗", NULL, MB_OK);
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
	//解放処理
	for (int i = 0; i < m_ProcessorsNum + 1; i++)
		PdhCloseQuery(m_hQuery[i]);
#endif
}

/// <summary>
/// 指定したプロセッサ番号のCPU使用率を返す
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
