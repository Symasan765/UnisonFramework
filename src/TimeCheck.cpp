/*=================================================
//								FILE : TimeCheck.cpp
//		ファイル説明 :
//		時間計測を行うクラスを定義する。
//		名称としてはタイマーがふさわしいが名前の競合が怖いのでTimeCheckとする
//		参考サイト : http://vivi.dyndns.org/tech/cpp/timeMeasurement.html 様
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "TimeCheck.h"

//他ファイルから利用することがない
namespace {
	bool  g_UseFlag = false;		//使用済み = true : 未使用 = false
}
LARGE_INTEGER cTimeCheck::m_Freq;

/// <summary>
/// タイマの初期化を行う
/// </summary>
cTimeCheck::cTimeCheck()
{
	//単位時間はタイマの周波数でプログラム実行中に変更されないそうなので一度だけの測定でOK
	if (!g_UseFlag) {
		QueryPerformanceFrequency(&m_Freq);
		g_UseFlag = true;
	}

	//初期化の変わりにクラス作成時のタイムをstartとendに格納しておく
	QueryPerformanceCounter(&m_Start);
	QueryPerformanceCounter(&m_End);
}

/// <summary>
/// 時間計測を開始する
/// </summary>
void cTimeCheck::TimerStart()
{
	QueryPerformanceCounter(&m_Start);
}

/// <summary>
/// 時間計測を終了する
/// </summary>
void cTimeCheck::TimerEnd()
{
	QueryPerformanceCounter(&m_End);
}

/// <summary>
/// 計測した時間を秒単位に変換してfloatで返す関数
/// </summary>
/// <returns></returns>
float cTimeCheck::GetProcessingTime() const
{
	//測定した時間を秒単位に変換してfloat値として返す
	return (float)(m_End.QuadPart - m_Start.QuadPart) / m_Freq.QuadPart;
}
