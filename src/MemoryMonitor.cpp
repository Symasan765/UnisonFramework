/*=================================================
//								FILE : MemoryMonitor.cpp
//		ファイル説明 :
//		メモリー使用状況を取得し管理するクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "MemoryMonitor.h"

MemoryData cMemoryMonitor::GetMemoryData() const
{
	return m_MemoryData;
}

/// <summary>
/// メモリの利用状況を更新する
/// </summary>
void cMemoryMonitor::Update()
{
	MEMORYSTATUSEX msex = { sizeof(MEMORYSTATUSEX) };
	GlobalMemoryStatusEx(&msex);
	//使用中メモリ
	m_MemoryData.vUsed = (msex.ullTotalPhys - msex.ullAvailPhys);
	//空き容量
	m_MemoryData.vFree = (msex.ullAvailPhys);
}

cMemoryMonitor::cMemoryMonitor()
{
	MEMORYSTATUSEX msex = { sizeof(MEMORYSTATUSEX) };
	GlobalMemoryStatusEx(&msex);

	//初期でのメモリ使用量
	m_MemoryData.vInit = (msex.ullTotalPhys - msex.ullAvailPhys);
	//物理メモリ量取得
	m_MemoryData.vSize = (msex.ullTotalPhys);
	//使用中メモリ
	m_MemoryData.vUsed = (msex.ullTotalPhys - msex.ullAvailPhys);
	//空き容量
	m_MemoryData.vFree = (msex.ullAvailPhys);
}
