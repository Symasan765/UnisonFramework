/*=================================================
//								FILE : TimeCheck.h
//		ファイル説明 :
//		時間計測を行うクラスを定義する。
//		名称としてはタイマーがふさわしいが名前の競合が怖いのでTimeCheckとする
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"

/// <summary>
/// 時間計測を行うクラス
/// </summary>
class cTimeCheck {
public:
	cTimeCheck();
	~cTimeCheck() = default;					//解放処理とくになかった
	void TimerStart();
	void TimerEnd();
	float GetProcessingTime()const;		//かかった処理時間をfloatで返す関数
private:
	LARGE_INTEGER m_Start, m_End;		//処理開始の時間と終了時間を格納する変数
	static LARGE_INTEGER m_Freq;		//単位時間を格納。関数で求めた数値をこの値で割るとsec単位に変換できる。
};