/*=================================================
//								FILE : LuaFunc.h
//		ファイル説明 :
//		Luaの関数で利用する引数、戻り値に関するクラスを定義する
//		参考技術書 : スクリプト言語による効率的ゲーム開発
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include <lua.hpp>

// TODO パラメータの数を動的配列にするといい

// パラメータの１つの値を格納するクラス
class LuaFuncParamItem
{
public:
	unsigned int m_type; //パラメータタイプ：LUA_TNIL/LUA_TNUMBER/LUA_TSTRING/LUA_TBOOLEAN
	union {
		float m_number; // 数値の場合の値
		char *m_str; // 文字列の場合の値
		bool m_bool; // ブールの場合の値
	};

public:
	LuaFuncParamItem() : m_type(LUA_TNIL), m_number(0) {}
	~LuaFuncParamItem() { ReleaseValue(); }

	// 型取得関数
	unsigned int GetType() const { return m_type; }

	// 型チェック関数
	bool IsNil()    const { return m_type == LUA_TNIL; }
	bool IsNumber() const { return m_type == LUA_TNUMBER; }
	bool IsString() const { return m_type == LUA_TSTRING; }
	bool IsBool()   const { return m_type == LUA_TBOOLEAN; }

	// 値取得関数
	float GetNumber()        const { return m_number; }
	const char * GetString()  const { return m_str; }
	bool GetBool()            const { return m_bool; }

	// バッファ解放
	void ReleaseValue();

	// 数値をセット
	void SetNumber(float number);
	// 文字列値をセット
	void SetString(const char * str);
	// nil値をセット
	void SetNil();
	// bool値をセット
	void SetBool(bool value);

	// 格納している値をLuaスタックに積む
	void PushToStack(lua_State *L);
};

// ユーザーから直接使う関数パラメータクラス
// LuaFuncParam param;
// param.Number(10).String("hello").Nil().Number(50);
// のように使える。
class LuaFuncParam
{
private:
	enum {
		PARAMS_MAX = 20, // 最大パラメータ数
	};
	LuaFuncParamItem m_params[PARAMS_MAX];
	int m_params_count;

public:
	LuaFuncParam() : m_params_count(0) {}
	~LuaFuncParam() { Clear(); }

	// パラメータを全クリア
	void Clear();
	// パラメータ数を返す
	int GetCount() { return m_params_count; }

	// 各種パラメータの追加
	LuaFuncParam & Number(float number);
	LuaFuncParam & String(const char *str);
	LuaFuncParam & Nil();
	LuaFuncParam & Bool(bool value);

	// 指定インデックスのパラメータ値取得
	// (インデックスは０ベース）
	bool IsNil(int index);
	float GetNumber(int index);
	const char * GetString(int index);
	const bool GetBool(int index);

	void ParamCuntClear() { m_params_count = 0; }

	// Luaスタックに引数をプッシュして、プッシュした引数の数を返す
	int PushToStack(lua_State *L);
	// Luaスタックから値を取得
	// スタックトップからresult_count個の値を取得して格納
	void GetFromStack(lua_State *L, int result_count);
};