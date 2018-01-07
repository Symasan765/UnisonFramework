/*=================================================
//								FILE : LuaCoroutine
//		ファイル説明 :
//		Lua上のコルーチンを取り扱うクラスを定義する
//		LuaActorクラスからLuaStateを取得して使用する
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include <lua.hpp>
#include <string>
#include "LuaActor.h"

/// <summary>
/// ひとつのLuaコルーチンを取り扱うクラス
/// </summary>
class cLuaCoroutine {
public:
	cLuaCoroutine(cLuaActor* pActor, const std::string _FuncName, int result_count);
	~cLuaCoroutine();
	void SetLuaNewCoroutine(lua_State* L);
	void Reload();

	bool Execution();

	LuaFuncParam m_Ret;		//戻り値用
	LuaFuncParam m_Param;		//返値用
private:
	void AnalyzeError(int res_call, std::string location);
	lua_State* m_Thread;
	std::string m_RegistryIndex;		//コルーチンをスタックから除去したあとのGC対策用
	lua_State* m_ParentL;				//注意！コルーチン削除の際だけ利用する。
	std::string m_FuncName;					//実行する関数名
	int m_RetNum;		//戻り値の数
};