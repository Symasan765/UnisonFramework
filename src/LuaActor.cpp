/*=================================================
//								FILE : LuaActor.cpp
//		ファイル説明 :
//		Luaのスクリプトを取り扱うクラスを定義する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "LuaActor.h"
#include "LuaManager.h"
#include "LuaCoroutine.h"
#include <Windows.h>
#include <stdlib.h>

namespace {
	//スクリプトを格納しているフォルダまでのパス
	std::string ScriptFileDIrePathName = "Script/";
}

cLuaActor::cLuaActor(const std::string _FileName, bool LibLoadFlag)
{
	m_FileName = _FileName;
	//自身がデータを読み込んだことをマネージャに管理させる
	cLuaManager::GetInstance().AddLuaScript(this, _FileName);

	m_L = luaL_newstate();
	if (LibLoadFlag)
		luaL_openlibs(m_L);		//すべてのライブラリを開いておく

		//Lua_Stateにデバッグ情報を積んでおく
	lua_getglobal(m_L, "debug");	//"debug"という文字列をスタックに積む
	//スタックの最上部がnilかどうか？
	if (!lua_isnil(m_L, -1)) {
		lua_getfield(m_L, -1, "traceback");	//この場合-1に積まれた値[traceback]の値をスタックに積むのでdebug.tracebackを積んだことになる
		m_pGetStackTraceFunc = lua_tocfunction(m_L, -1);		//debug.tracebackをCの関数に変換してそのポインタをメンバで保持した
	}
}

cLuaActor::~cLuaActor()
{
	//マネージャに削除したことを伝える
	cLuaManager::GetInstance().EraseLuaScript(this, m_FileName);
	Exit();
}

bool cLuaActor::CallFunc(std::string funcname, int result_count)
{
	int old_top = lua_gettop(m_L);
	// 返り値はクリアしておく
	m_Ret.Clear();
	// ランタイムエラー処理関数を積む
	lua_pushcfunction(m_L, m_pGetStackTraceFunc);

	// 関数をみつける
	lua_getglobal(m_L, funcname.c_str());
	//ちゃんと関数見つけられた？
	if (!lua_isfunction(m_L, -1)) {
		std::string location = "calling function<" + funcname + ">";
		SetErr(location, "the function not found.");
		return false;
	}

	// pcallおよびスタック処理
	int res_call = CallSub(&m_Ret, result_count, &m_Param, old_top + 1);
	if (res_call != 0) {
		// エラーメッセージ生成
		std::string location = "calling function<" + funcname + ">";
		AnalyzeError(res_call, location);
	}
	lua_settop(m_L, old_top); // スタックを元に戻す
	return (res_call == 0);	// true : 関数成功
}

bool cLuaActor::DoFile(std::string path,int result_count)
{
	int old_top = lua_gettop(m_L);
	// 返り値はクリアしておく
	m_Ret.Clear();
	// ランタイムエラー処理関数を積む
	lua_pushcfunction(m_L, m_pGetStackTraceFunc);

	// ファイルをロード
	int res_load = luaL_loadfile(m_L, path.c_str());
	if (res_load != 0) {
		// エラーメッセージ生成
		std::string location = "loading file<" + path + ">";
		AnalyzeError(res_load, location);
		lua_settop(m_L, old_top); // スタックを元に戻す
		return false;
	}
	// pcallおよびスタック処理
	int res_call = CallSub(&m_Ret, result_count, &m_Param, old_top + 1);
	if (res_call != 0) {
		// エラーメッセージ生成
		std::string location = "executing file<" + path + ">";
		AnalyzeError(res_load, location);
		AnalyzeError(res_call, location);
	}
	m_Param.ParamCuntClear();	//引数の数をクリアしておく
	lua_settop(m_L, old_top); // スタックを元に戻す
	return (res_call == 0);
}

void cLuaActor::ScriptCommit()
{
	//読み込みのエラーが無くなるまでロード処理を行う
	while (luaL_dofile(m_L, (ScriptFileDIrePathName + m_FileName).c_str()) != 0) { //引数でフォルダのパスを完成させてファイル実行
																				  //エラー。読み込み失敗
		std::string ErrMessage = m_FileName + " のロード失敗。リロードします";
		int res = MessageBox(0, ErrMessage.c_str(), NULL, MB_OKCANCEL);
		//キャンセルされたらループを抜ける
		if (res == IDCANCEL)
			break;
	}

	//デバッグモード中のみエディタを開く
#if defined(DEBUG) || defined(_DEBUG)
	/*char szFullPath[256] = { "\0" };
	char* szFilePath = nullptr;
	const char* path = (ScriptFileDIrePathName + m_FileName).c_str();

	_fullpath(szFullPath, (ScriptFileDIrePathName).c_str(), sizeof(szFullPath) / sizeof(szFullPath[0]));

	std::string AllPath = std::string(szFullPath) + (m_FileName);
	HINSTANCE hr = ShellExecute(NULL, "open", AllPath.c_str(), NULL, NULL, SW_HIDE);*/
#endif

	//Luaのスタックを空にしておく
	lua_settop(m_L, 0);
}

void cLuaActor::PushCoroutine(cLuaCoroutine * pCol)
{
	m_pCoroutines.push_back(pCol);
}

void cLuaActor::ScriptReload()
{
	//まずファイルを読み直してからコルーチンを読み直す
	ScriptCommit();
	for (auto itr = m_pCoroutines.begin(); itr != m_pCoroutines.end(); itr++) {
		(*itr)->SetLuaNewCoroutine(m_L);
	}
}

void cLuaActor::Exit()
{
	lua_close(m_L);
	m_L = nullptr;
}

void cLuaActor::SetErr(std::string location, std::string message)
{
	m_err = location + message;
	//デバッグモード中のみエラーが出たことを表示させる
#if defined(DEBUG) || defined(_DEBUG)
	MessageBox(0, m_err.c_str(), NULL, MB_OK);
#endif
}

/// <summary>
/// ここに来るのはエラーが起きたことが確定してから。エラー数値からエラー内容に変換する
/// </summary>
/// <param name="res_call"></param>
/// <param name="location"></param>
void cLuaActor::AnalyzeError(int res_call, std::string location)
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
	std::string err_mes = reason + lua_tostring(m_L, -1);
	SetErr(location, err_mes);
}

/// <summary>
/// 実際に関数を呼ぶ
/// </summary>
/// <param name="result"></param>
/// <param name="result_count"></param>
/// <param name="params"></param>
/// <param name="errfunc_index"></param>
/// <returns></returns>
int cLuaActor::CallSub(LuaFuncParam * result, int result_count, LuaFuncParam * params, int errfunc_index)
{
	// paramsによって指定された可変個の引数をスタックに置く
	int params_count = 0;
	if (params) {
		params_count = params->PushToStack(m_L);
	}
	// ロードされたチャンクをコール
	int res_call = lua_pcall(m_L, params_count, result_count, errfunc_index);
	if (res_call != 0) {
		// エラー処理
		return res_call;
	}
	// 返り値解析
	if (result) result->GetFromStack(m_L, result_count);
	return res_call;
}
