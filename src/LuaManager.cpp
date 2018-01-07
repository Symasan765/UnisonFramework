/*=================================================
//								FILE : LuaManager.cpp
//		ファイル説明 :
//		Luaスクリプトを管理するマネージャクラス
//		リロード処理などを担当することになる
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "LuaManager.h"
#include "ComboBox.h"

/// <summary>
/// スクリプトをマネージャに登録する
/// </summary>
/// <param name="pL"></param>
/// <param name="FileName"></param>
/// <returns></returns>
void cLuaManager::AddLuaScript(cLuaActor* pLuaAct, std::string FileName)
{
	m_ScriptMap[FileName].push_back(pLuaAct);
}

/// <summary>
/// スクリプト名からmapを検索してその要素をvectorから削除する
/// </summary>
/// <param name="pLuaAct"></param>
/// <param name="FileName"></param>
void cLuaManager::EraseLuaScript(cLuaActor * pLuaAct, std::string FileName)
{
	//その名前がmapに登録されているか確認する
	auto map_itr = m_ScriptMap.find(FileName);
	if (map_itr != m_ScriptMap.end()) {
		//設定されている場合の処理
		std::vector<cLuaActor*>& data = map_itr->second;
		for (auto itr = data.begin(); itr != data.end(); itr++) {
			if (pLuaAct == *itr) {
				map_itr->second.erase(itr);
				return;
			}
		}
	}
}

/// <summary>
/// すべてのスクリプト名をコンボボックスに追加する
/// </summary>
/// <param name="pComboBox"></param>
void cLuaManager::SetTheNameInTheComboBox(cComboBox * pComboBox)
{
	pComboBox->DeleteAllItem();
	for (auto itr = m_ScriptMap.begin(); itr != m_ScriptMap.end(); ++itr) {
		pComboBox->AddItem(itr->first);
	}
}

void cLuaManager::ReloadTheScript(std::string FileName)
{
	auto map_itr = m_ScriptMap.find(FileName);
	if (map_itr == m_ScriptMap.end()) return;

	//設定されている場合の処理
	std::vector<cLuaActor*>& data = map_itr->second;
	for (auto itr = data.begin(); itr != data.end(); itr++) {
		// TODO ここでcLuaActorクラスで定義したリロード関数を呼ぶ
		(*itr)->ScriptReload();
	}
}