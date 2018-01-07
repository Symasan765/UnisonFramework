#include "LuaFunc.h"
#include <string.h>

// バッファ解放
void LuaFuncParamItem::ReleaseValue() {
	if (m_type == LUA_TSTRING) {
		if (m_str) delete[] m_str;
		m_str = NULL;
	}
}

// 数値をセット
void LuaFuncParamItem::SetNumber(float number) {
	ReleaseValue();
	m_type = LUA_TNUMBER;
	m_number = number;
}
// 文字列値をセット
void LuaFuncParamItem::SetString(const char * str) {
	ReleaseValue();
	m_type = LUA_TSTRING;
	size_t len = strlen(str);
	m_str = new char[len + 1];
	if (m_str) {
		strncpy(m_str, str, len);
		m_str[len] = '\0';
	}
}
// nil値をセット
void LuaFuncParamItem::SetNil() {
	ReleaseValue();
	m_type = LUA_TNIL;
}
// bool値をセット
void LuaFuncParamItem::SetBool(bool value) {
	ReleaseValue();
	m_type = LUA_TBOOLEAN;
	m_bool = value;
}

// 格納している値をLuaスタックに積む
void LuaFuncParamItem::PushToStack(lua_State *L) {
	switch (m_type) {
	case LUA_TNUMBER: lua_pushnumber(L, m_number); break;
	case LUA_TSTRING: lua_pushstring(L, m_str); break;
	case LUA_TNIL: lua_pushnil(L); break;
	case LUA_TBOOLEAN: lua_pushboolean(L, m_bool); break;
	}
}

//
//  class LuaFuncParam：複数のパラメータや返り値を表すクラス
//

// パラメータを全クリア
void LuaFuncParam::Clear() {
	for (int i = 0; i<m_params_count; i++) {
		m_params[i].ReleaseValue();
	}
	m_params_count = 0;
}

// 数値パラメータの追加
LuaFuncParam & LuaFuncParam::Number(float number) {
	m_params[m_params_count].SetNumber(number);
	m_params_count++;
	return *this;
}
// 文字列パラメータの追加
LuaFuncParam & LuaFuncParam::String(const char *str) {
	m_params[m_params_count].SetString(str);
	m_params_count++;
	return *this;
}
// nilパラメータの追加
LuaFuncParam & LuaFuncParam::Nil() {
	m_params[m_params_count].SetNil();
	m_params_count++;
	return *this;
}
// ブール値パラメータの追加
LuaFuncParam & LuaFuncParam::Bool(bool value) {
	m_params[m_params_count].SetBool(value);
	m_params_count++;
	return *this;
}

// 指定インデックスのパラメータのNULLチェック
// (インデックスは０ベース）
bool LuaFuncParam::IsNil(int index) {
	if (index < 0 || index >= m_params_count) return true;
	return m_params[index].IsNil();
}
// 指定インデックスのパラメータ数値取得
// (インデックスは０ベース）
float LuaFuncParam::GetNumber(int index) {
	if (index < 0 || index >= m_params_count) return 0;
	if (m_params[index].GetType() != LUA_TNUMBER) {
		return 0;
	}
	return m_params[index].GetNumber();
}
// 指定インデックスのパラメータ文字列取得
// (インデックスは０ベース）
const char * LuaFuncParam::GetString(int index) {
	if (index < 0 || index >= m_params_count) return NULL;
	if (m_params[index].GetType() != LUA_TSTRING) {
		return NULL;
	}
	return m_params[index].GetString();
}
// 指定インデックスのブール値取得
// (インデックスは０ベース）
const bool LuaFuncParam::GetBool(int index) {
	if (index < 0 || index >= m_params_count) return NULL;
	if (m_params[index].GetType() != LUA_TBOOLEAN) {
		return NULL;
	}
	return m_params[index].GetBool();
}

// Luaスタックに引数をプッシュして、プッシュした引数の数を返す
int LuaFuncParam::PushToStack(lua_State *L) {
	for (int i = 0; i<m_params_count; i++) {
		m_params[i].PushToStack(L);
	}
	return m_params_count;
}

// Luaスタックから値を取得
// スタックトップからresult_count個の値を取得して格納
void LuaFuncParam::GetFromStack(lua_State *L, int result_count) {
	for (int i = 0; i<result_count; i++) {
		int index = -result_count + i;
		int type = lua_type(L, index);
		switch (type) {
		case LUA_TNIL: this->Nil(); break;
		case LUA_TSTRING: this->String(lua_tostring(L, index)); break;
		case LUA_TNUMBER: this->Number((float)lua_tonumber(L, index)); break;
		case LUA_TBOOLEAN: this->Bool(lua_toboolean(L, index) ? true : false); break;
		default: this->Nil(); break;
		}
	}
}