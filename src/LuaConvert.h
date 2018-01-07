/*=================================================
//								FILE : LuaConvert.h
//		ファイル説明 :
//		LuaとC++間でデータ変換を行う関数を登録しておく
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once

/// <summary>
/// Luaのデータ変換と扱う関数群
/// </summary>
namespace LuaConv {

	/// <summary>
	/// LuaStateのスタック先頭を指定型のアドレスに変換して返す関数
	/// </summary>
	template<typename T>
	T* toPointer(lua_State* L) {
		T** buf = static_cast<T**>(lua_touserdata(L, lua_upvalueindex(1)));
		T* point = static_cast<T*>(*buf);

		return point;
	};


	/// <summary>
	/// 渡されたLuaState上にグローバル変数をセットし、そこに関数群もセットする
	/// </summary>
	template<typename T>
	void SetLuaGlobalObjct(lua_State* L, std::string LuaVarName, const luaL_Reg* funcs, T* addr) {
		luaL_newlibtable(L, funcs);
		T** object = (T**)lua_newuserdata(L, sizeof(T*));
		*object = addr;
		luaL_setfuncs(L, funcs, 1);
		lua_setglobal(L, LuaVarName.c_str());
	};
}