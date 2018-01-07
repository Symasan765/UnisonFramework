/*=================================================
//								FILE : ShaderMonitoring.h
//		ファイル説明 :
//		シェーダファイルの更新を監視して見つければ
//		リロード処理を命令するクラス
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include <vector>
#include <string>
#include "WinMain.h"
#include "TimeCheck.h"

#define SHADER_REROAD_TIME_SEC 5.0f

struct ShaderMonitStruct {
	std::string vFileName;
	DWORD vTime;				//時間情報を保持
	HANDLE vFileHnd;

	ShaderMonitStruct() {
		vTime = 0;
		vFileHnd = NULL;
	}
};

class cShaderMonitoring {
public:
	cShaderMonitoring(const cShaderMonitoring&) = delete;			//コピーコンストラクタを delete 指定
	cShaderMonitoring& operator=(const cShaderMonitoring&) = delete;	//コピー代入演算子も delete 指定
	cShaderMonitoring(cShaderMonitoring&&) = delete;			//ムーブコンストラクタを delete 指定
	cShaderMonitoring& operator=(cShaderMonitoring&&) = delete;		//ムーブ代入演算子も delete 指定

	static cShaderMonitoring& GetInstance() {
		static cShaderMonitoring inst;			// privateなコンストラクタを呼び出す
		return inst;
	}

	void MonitorChanges();
	void AddFileDate(std::string _FileName);
private:
	cShaderMonitoring() = default;					//コンストラクタを private に置く
	~cShaderMonitoring();					//デストラクタを private に置く
	DWORD GetFileChangeTime(HANDLE fileHnd);
	cTimeCheck m_TimeCnt;

	std::vector<ShaderMonitStruct> m_Data;
};