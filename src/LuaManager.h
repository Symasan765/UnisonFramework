/*=================================================
//								FILE : LuaManager.h
//		ファイル説明 :
//		Luaスクリプトを管理するマネージャクラス
//		リロード処理などを担当することになる
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include <map>
#include <vector>
#include "LuaActor.h"

class cComboBox;

/// <summary>
/// Luaスクリプトを管理するクラス
/// </summary>
class cLuaManager {
public:
	cLuaManager(const cLuaManager&) = delete;			//コピーコンストラクタを delete 指定
	cLuaManager& operator=(const cLuaManager&) = delete;	//コピー代入演算子も delete 指定
	cLuaManager(cLuaManager&&) = delete;			//ムーブコンストラクタを delete 指定
	cLuaManager& operator=(cLuaManager&&) = delete;		//ムーブ代入演算子も delete 指定

	static cLuaManager& GetInstance() {
		static cLuaManager inst;			// privateなコンストラクタを呼び出す
		return inst;
	}

	void AddLuaScript(cLuaActor* pLuaAct,std::string FileName);
	void EraseLuaScript(cLuaActor* pLuaAct, std::string FileName);
	void SetTheNameInTheComboBox(cComboBox* pComboBox);
	void ReloadTheScript(std::string FileName);
private:
	cLuaManager() = default;					//コンストラクタを private に置く
	~cLuaManager() = default;					//デストラクタを private に置く

	std::map<std::string, std::vector<cLuaActor*>> m_ScriptMap;		//スクリプトの利用状況を管理する
};