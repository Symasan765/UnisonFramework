/*=================================================
//								FILE : LuaCoroutine
//		ファイル説明 :
//		Lua上のコルーチンを取り扱うクラスを定義する
//		LuaActorクラスからLuaStateを取得して使用する
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "LuaCoroutine.h"
#include <Windows.h>

cLuaCoroutine::cLuaCoroutine(cLuaActor* pActor, const std::string _FuncName, int result_count)
{
	pActor->PushCoroutine(this);	//参照をアクターに管理してもらう
	m_ParentL = pActor->GetLuaState();
	m_FuncName = _FuncName;
	m_RetNum = result_count;
	
	SetLuaNewCoroutine(m_ParentL);
}

cLuaCoroutine::~cLuaCoroutine()
{
	// コルーチンの削除
	//LUA_REGISTRYINDEX[m_RegistryIndex] = nilという登録を行う
	/*lua_pushnil(m_ParentL);
	lua_pushlightuserdata(m_ParentL, (void*)m_RegistryIndex.c_str());
	lua_settable(m_ParentL, LUA_REGISTRYINDEX);*/		// TODO 現状ここでランタイムエラーが発生する。原因不明
}

void cLuaCoroutine::SetLuaNewCoroutine(lua_State * L)
{
	int top = lua_gettop(L);
	static int TortalNum = 0;
	//ガーベジコレクタ対策でコルーチンを親のレジストリに保持しておく
	//LUA_REGISTRYINDEXはLuaが標準で用意するレジストリテーブル
	m_RegistryIndex = m_FuncName + "_CorName" + std::to_string(TortalNum);
	TortalNum++;
	lua_pushlightuserdata(L, (void*)m_RegistryIndex.c_str());
	m_Thread = lua_newthread(L);
	// 以下の命令でLUA_REGISTRYINDEX[m_RegistryIndex] = m_Threadという登録になる。これでスタックからスレッドを消してもGCの対象から外れる
	lua_settable(L, LUA_REGISTRYINDEX);

	//コルーチンを取得してスタックに積む
	lua_getglobal(m_Thread, m_FuncName.c_str());

	lua_settop(L, top);
}

void cLuaCoroutine::Reload()
{
	SetLuaNewCoroutine(m_ParentL);
}

/// <summary>
/// 
/// </summary>
/// <param name="funcname"></param>
/// <param name="result_count"></param>
/// <returns>true : 実行できた</returns>
bool cLuaCoroutine::Execution()
{
	static int s = 0;
	if (m_Thread == NULL)
		int a = 10;
	s++;
	int old_top = lua_gettop(m_Thread);
	// 返り値はクリアしておく
	m_Ret.Clear();

	int params_count = m_Param.PushToStack(m_Thread);
	// resumeおよびスタック処理
	int res_call = lua_resume(m_Thread, nullptr, params_count);
	while (res_call != 0) {
		if (res_call == LUA_YIELD) {
			// 返り値解析
			m_Ret.GetFromStack(m_Thread, m_RetNum);
			lua_settop(m_Thread, old_top); // スタックを元に戻す
			return true;		//中断しただけなら正常
		}
		// エラーメッセージ生成
		std::string location = "calling function<" + m_FuncName + ">";
		AnalyzeError(res_call, location);
		int res = MessageBox(0, "再度コルーチンの実行を試しますか？", NULL, MB_OKCANCEL);
		//キャンセルされたらループを抜ける
		if (res == IDCANCEL) {
			break;
		}
		res_call = lua_resume(m_Thread, nullptr, params_count);
	}
	// 返り値解析
	m_Ret.GetFromStack(m_Thread, m_RetNum);
	lua_settop(m_Thread, old_top); // スタックを元に戻す
	return (res_call == 0);	// true : 関数成功
}


void cLuaCoroutine::AnalyzeError(int res_call, std::string location)
{
	std::string reason = "";
	switch (res_call) {
	case LUA_ERRRUN: reason = "SCRIPT RUNTIME ERROR"; break;
	case LUA_ERRSYNTAX: reason = "SCRIPT SYNTAX ERROR"; break;
	case LUA_ERRMEM: reason = "SCRIPT MEMORY ERROR"; break;
	case LUA_ERRFILE: reason = "SCRIPT FILE ERROR"; break;
	default: break;
	}
	// エラーメッセージ取得
	std::string err_mes = reason + lua_tostring(m_Thread, -1);
	//デバッグモード中のみエラーが出たことを表示させる
#if defined(DEBUG) || defined(_DEBUG)
	MessageBox(0, (location + err_mes).c_str(), NULL, MB_OK);
#endif
}