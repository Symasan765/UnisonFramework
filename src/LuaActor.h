/*=================================================
//								FILE : LuaActor.h
//		ファイル説明 :
//		Luaのスクリプトを取り扱うクラスを定義する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include <lua.hpp>
#include <string>
#include <vector>
#include "LuaFunc.h"
class cLuaCoroutine;

/// <summary>
/// このクラスでゲームアクター1つ分の機能と取り扱いする。LuaVMが作成されるので作りすぎに注意
/// </summary>
class cLuaActor {
public:
	cLuaActor(const std::string _FileName,bool LibLoadFlag = true);
	virtual ~cLuaActor();
	//======================== 関数 ===============================
	inline lua_State *GetLuaState() { return m_L; };
	bool CallFunc(std::string funcname, int result_count = 0);
	bool DoFile(std::string path, int result_count = 0);
	void ScriptCommit();
	void PushCoroutine(cLuaCoroutine* pCol);
	void ScriptReload();

	LuaFuncParam m_Ret;		//戻り値用
	LuaFuncParam m_Param;		//返値用
protected:
	void Exit();
	void SetErr(std::string location, std::string message);
	inline const std::string GetErr() { return m_err; }
	void AnalyzeError(int res_call, std::string location);
	int CallSub(LuaFuncParam* result, int result_count, LuaFuncParam* params, int errfunc_index);

	//======================== 変数 ===============================
	lua_State *m_L;		// Luaステート
	std::string m_err;	// エラー文字列
	std::string m_FileName;
	std::vector<cLuaCoroutine*> m_pCoroutines;		//このスクリプトから派生したコルーチンの参照
	lua_CFunction m_pGetStackTraceFunc;	// debug.tracebackの実装へのポインタ
};