/*=========================================================//
//		概要	:	入力処理を記述する
//					作成者　松本 雄之介 (AT12A606)
//						作成日	2016/11/2
//					  更新日
//	入力系の処理を記述する。将来的にはここにゲームパッドやマウス処理を加える
//=========================================================*/
#ifndef INC_INPUT_H
#define INC_INPUT_H

#define GetKey Input::cKeyboard::getInstance()		//キーボード情報取得(ポインタ)
#define GetGamePad Input::XInput::getInstance()		//XInput情報取得(ポインタ)

#include <dinput.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

#include <XInput.h>
#include <lua.hpp>
#include <string>
#include "WinMain.h"

//入力処理系のクラスを記述する
namespace Input {

	//キーボードクラス(シングルトン)
	class cKeyboard {
	public:
		//更新処理や解放処理を行うためにcDirectXクラスをフレンド化しておく
		friend class cDirectX11;
		bool Press(int key);
		bool Trigger(int key);
		bool Repeat(int key);
		bool Release(int key);
		HRESULT UpdateKeyboard(void);

		//初回呼び出し時に実体生成
		static cKeyboard* getInstance() {
			static cKeyboard key;
			return &key;
		}
	private:
		cKeyboard();
		HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd);
		void UninitKeyboard(void);

		cKeyboard(const cKeyboard&) = delete;				//コピーコンストラクタを delete 指定
		cKeyboard& operator=(const cKeyboard&) = delete;	//コピー代入演算子も delete 指定
		cKeyboard(cKeyboard&&) = delete;						//ムーブコンストラクタを delete 指定
		cKeyboard& operator=(cKeyboard&&) = delete;			//ムーブ代入演算子も delete 指定
		~cKeyboard();
	};


	//XInputクラス(シングルトン)
	class XInput {
	public:
		//更新処理や解放処理を行うためにcDirectXクラスをフレンド化しておく
		friend class cDirectX11;
		bool Press(const int key, const DWORD no = 0)const;
		bool Trigger(const int key, const DWORD no = 0)const;
		bool Repeat(const int key, const DWORD no = 0)const;
		bool Release(const int key, const DWORD no = 0)const;
		float LeftTrigger(const DWORD no = 0)const;
		float RightTrigger(const DWORD no = 0)const;
		DirectX::XMFLOAT2 LeftStick(const DWORD no = 0)const;
		DirectX::XMFLOAT2 RightStick(const DWORD no = 0)const;

		void UpdateGamePad(void);

		//初回呼び出し時に実体生成
		static XInput* getInstance() {
			static XInput xinput;
			return &xinput;
		}

		//キー入力関数をLuaに渡す関数
		void AddFunctionToLua(lua_State* L, std::string LuaVarName = "GamePad");

	private:
		XInput() = default;

		XInput(const XInput&) = delete;				//コピーコンストラクタを delete 指定
		XInput& operator=(const XInput&) = delete;	//コピー代入演算子も delete 指定
		XInput(XInput&&) = delete;						//ムーブコンストラクタを delete 指定
		XInput& operator=(XInput&&) = delete;			//ムーブ代入演算子も delete 指定
		~XInput() = default;

		unsigned ConvertKeyboardToGamePad(unsigned);		//ゲームパッドが接続されていないとき用のコンバーター関数

		//各種状態変数
		XINPUT_STATE m_State[4];		//コントローラ4台分の状態
		XINPUT_STATE m_OldState[4];		//コントローラ4台分の状態
		int conect[4];						//接続されているか？



		//======================== Lua用Glue関数 ===============================
		static int PressGlue(lua_State* L);
		static int TriggerGlue(lua_State* L);
		static int ReleaseGlue(lua_State* L);
		static int LeftTriggerGlue(lua_State* L);
		static int RightTriggerGlue(lua_State* L);
		static int LeftStickGlue(lua_State* L);
		static int RightStickGlue(lua_State* L);
	};

	/*
	#define XINPUT_GAMEPAD_DPAD_UP          0x00000001
	#define XINPUT_GAMEPAD_DPAD_DOWN        0x00000002
	#define XINPUT_GAMEPAD_DPAD_LEFT        0x00000004
	#define XINPUT_GAMEPAD_DPAD_RIGHT       0x00000008
	#define XINPUT_GAMEPAD_START            0x00000010
	#define XINPUT_GAMEPAD_BACK             0x00000020
	#define XINPUT_GAMEPAD_LEFT_THUMB       0x00000040
	#define XINPUT_GAMEPAD_RIGHT_THUMB      0x00000080
	#define XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
	#define XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
	#define XINPUT_GAMEPAD_A                0x1000
	#define XINPUT_GAMEPAD_B                0x2000
	#define XINPUT_GAMEPAD_X                0x4000
	#define XINPUT_GAMEPAD_Y                0x8000
	*/
}
#endif