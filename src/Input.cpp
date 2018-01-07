/*=========================================================//
//		概要	:	入力処理を記述する
//					作成者　松本 雄之介 (AT12A606)
//						作成日	2016/11/2
//					  更新日
//
//=========================================================*/
#include "Input.h"
#include "WinMain.h"
#include "LuaConvert.h"
using namespace LuaConv;

#define DIRECTINPUT_VERSION 0x0800
//#define DEADZONE XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
#define DEADZONE 12000

//他のファイルから隠ぺい
namespace {
	const int NUM_KEY_MAX = 256;
	LPDIRECTINPUT8			pDInput = NULL;						// IDirectInput8インターフェースへのポインタ
	LPDIRECTINPUTDEVICE8	pDIDevKeyboard = NULL;				// IDirectInputDevice8インターフェースへのポインタ(キーボード)
	LPDIRECTINPUTDEVICE8	pDIDevMouse = NULL;					// IDirectInputDevice8インターフェースへのポインタ(マウス)
	BYTE						keyState[NUM_KEY_MAX];				// キーボードの状態を受け取るワーク
	BYTE						keyStateTrigger[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
	BYTE						keyStateRepeat[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
	BYTE						keyStateRelease[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
	int							keyStateRepeatCnt[NUM_KEY_MAX];	// キーボードのリピートカウンタ
	DIMOUSESTATE2			mouseState;							// マウスの状態を受け取るワーク
	DIMOUSESTATE2			mouseStateTrigger;					// マウスの状態を受け取るワーク
	POINT						MousePoint;							// マウスの現在座標

	struct XInputLuaGlueStruct {
		std::string vVarName;		//Luaに登録する変数名
		unsigned int vNumber;		//登録する実数値
	};
}
using namespace Input;

cKeyboard::cKeyboard() {
	HRESULT hr;
	if (!pDInput)
	{
		// DirectInputオブジェクトの作成
		hr = DirectInput8Create(GetDirectX::HInstance(), DIRECTINPUT_VERSION,
			IID_IDirectInput8, (void**)&pDInput, NULL);
	}
	InitKeyboard(GetDirectX::HInstance(), GetDirectX::Hwnd());
}

HRESULT cKeyboard::InitKeyboard(HINSTANCE hInst, HWND hWnd) {
	HRESULT hr = 0;

	// デバイスオブジェクトを作成
	hr = pDInput->CreateDevice(GUID_SysKeyboard, &pDIDevKeyboard, NULL);
	if (FAILED(hr) || pDIDevKeyboard == NULL)
	{
		MessageBox(hWnd, "キーボードがねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	// データフォーマットを設定
	hr = pDIDevKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "キーボードのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr = pDIDevKeyboard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if (FAILED(hr))
	{
		MessageBox(hWnd, "キーボードの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// キーボードへのアクセス権を獲得(入力制御開始)
	pDIDevKeyboard->Acquire();

	return S_OK;
}

HRESULT cKeyboard::UpdateKeyboard(void) {
	HRESULT hr;
	BYTE keyStateOld[256];

	// 前回のデータを保存
	memcpy(keyStateOld, keyState, NUM_KEY_MAX);

	// デバイスからデータを取得
	hr = pDIDevKeyboard->GetDeviceState(sizeof(keyState), keyState);
	if (SUCCEEDED(hr))
	{
		for (int cnt = 0; cnt < NUM_KEY_MAX; cnt++)
		{
			// トリガーとリリースを取得
			keyStateTrigger[cnt] = (keyState[cnt] ^ keyStateOld[cnt]) & keyState[cnt];
			keyStateRelease[cnt] = (keyState[cnt] ^ keyStateOld[cnt]) & keyStateOld[cnt];

			// キーが押されているならリピートの判定処理
			if (keyState[cnt])
			{
				if (keyStateRepeatCnt[cnt] < 20)
				{
					keyStateRepeatCnt[cnt]++;
					// 「初回入力」もしくは「ボタンを押してから20フレーム経過」
					// した場合、リピート用配列のデータを有効にする
					// 上記条件以外はリピート用配列のデータを無効にする
					if (keyStateRepeatCnt[cnt] == 1 || keyStateRepeatCnt[cnt] >= 20) {
						keyStateRepeat[cnt] = 0x80;
					}
					else {
						keyStateRepeat[cnt] = 0;
					}
				}
			}
			else
			{
				keyStateRepeatCnt[cnt] = 0;
				keyStateRepeat[cnt] = 0;
			}
		}
	}
	else
	{
		// キーボードへのアクセス権を取得
		hr = pDIDevKeyboard->Acquire();
	}

	return hr;
}

void cKeyboard::UninitKeyboard(void) {
	if (pDIDevKeyboard)
	{
		pDIDevKeyboard->Unacquire();

		pDIDevKeyboard->Release();
		pDIDevKeyboard = nullptr;
	}
}

cKeyboard::~cKeyboard() {
	// キーボードの終了処理
	UninitKeyboard();

	if (pDInput) {
		pDInput->Release();
		pDInput = nullptr;
	}
}

bool cKeyboard::Press(int key) {
	return ((keyState[key] & 0x80) != 0);
}

bool cKeyboard::Trigger(int key) {
	return ((keyStateTrigger[key] & 0x80) != 0);
}

bool Input::cKeyboard::Release(int key)
{
	return (keyStateRelease[key]);
}


bool XInput::Press(const int key, const DWORD no) const
{
	if (conect[no]) {
		if (m_State[no].Gamepad.wButtons & key) {
			return true;
		}
	}
	else {
		if (GetKey->Press(GetGamePad->ConvertKeyboardToGamePad(key)))
			return true;
	}
	return false;
}

bool Input::XInput::Trigger(const int key, const DWORD no) const
{
	if (conect[no]) {
		//現在のキーと前のキーとの差分処理で判定を行う
		bool nowKey = m_State[no].Gamepad.wButtons & key;
		bool oldKey = m_OldState[no].Gamepad.wButtons & key;
		//トリガー判定処理
		if ((nowKey ^ oldKey) & nowKey)
			return true;
	}
	else {
		if (GetKey->Trigger(GetGamePad->ConvertKeyboardToGamePad(key)))
			return true;
	}
	return false;
}

bool Input::XInput::Release(const int key, const DWORD no) const
{
	if (conect[no]) {
		//現在のキーと前のキーとの差分処理で判定を行う
		bool nowKey = m_State[no].Gamepad.wButtons & key;
		bool oldKey = m_OldState[no].Gamepad.wButtons & key;
		//リリース判定処理
		if ((nowKey ^ oldKey) & oldKey)
			return true;
	}
	else {
		if (GetKey->Release(GetGamePad->ConvertKeyboardToGamePad(key)))
			return true;
	}
	return false;
}

float Input::XInput::LeftTrigger(const DWORD no) const
{
	if (conect[no]) {
		//数値をチェックし、0.0~1.0の間で返却する
		float buf = (float)m_State[no].Gamepad.bLeftTrigger / 255.0f;
		return buf;
	}
	else {
		if (GetKey->Press(DIK_Q)) {
			return 1.0f;
		}
	}
	return 0.0f;
}

float Input::XInput::RightTrigger(const DWORD no) const
{
	if (conect[no]) {
		//数値をチェックし、0.0~1.0の間で返却する
		float buf = (float)m_State[no].Gamepad.bRightTrigger / 255.0f;
		return buf;
	}
	else {
		if (GetKey->Press(DIK_E)) {
			return 1.0f;
		}
	}
	return 0.0f;
}

DirectX::XMFLOAT2 Input::XInput::LeftStick(const DWORD no) const
{
	DirectX::XMFLOAT2 judge{ 0 , 0 };
	if (conect[no]) {
		judge.x = (float)m_State[no].Gamepad.sThumbLX / 32767;
		judge.y = (float)m_State[no].Gamepad.sThumbLY / 32767;
		return judge;
	}
	else {
		//コントローラがつながっていない
		if (GetKey->Press(DIK_A)) {
			judge.x -= 1.0f;
		}
		if (GetKey->Press(DIK_D)) {
			judge.x += 1.0f;
		}
		if (GetKey->Press(DIK_W)) {
			judge.y += 1.0f;
		}
		if (GetKey->Press(DIK_S)) {
			judge.y -= 1.0f;
		}
	}

	return judge;
}

DirectX::XMFLOAT2 Input::XInput::RightStick(const DWORD no) const
{
	DirectX::XMFLOAT2 judge{ 0 , 0 };
	if (conect[no]) {
		judge.x = (float)m_State[no].Gamepad.sThumbRX / 32767;
		judge.y = (float)m_State[no].Gamepad.sThumbRY / 32767;
		return judge;
	}
	else {
		//コントローラがつながっていない
		if (GetKey->Press(DIK_LEFT)) {
			judge.x -= 1.0f;
		}
		if (GetKey->Press(DIK_RIGHT)) {
			judge.x += 1.0f;
		}
		if (GetKey->Press(DIK_UP)) {
			judge.y += 1.0f;
		}
		if (GetKey->Press(DIK_DOWN)) {
			judge.y -= 1.0f;
		}
	}
	return judge;
}

void XInput::UpdateGamePad(void)
{
	//ステートとストロークを取得する
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
		m_OldState[i] = m_State[i];		//古いキー情報を格納する
		DWORD hr = XInputGetState(i, &m_State[i]);	//キー取得

		//ゲームパッドが接続されていない
		if (hr == ERROR_DEVICE_NOT_CONNECTED) {
			conect[i] = false;		//接続フラグオフ
			continue;
		}

		//接続されている
		conect[i] = true;

		//デッドゾーンの設定。デッドゾーン内なら数値を0に変更しておく
		if ((m_State[i].Gamepad.sThumbLX < DEADZONE &&
			m_State[i].Gamepad.sThumbLX > -DEADZONE)) {
			m_State[i].Gamepad.sThumbLX = 0;
		}
		if (m_State[i].Gamepad.sThumbLY < DEADZONE &&
			m_State[i].Gamepad.sThumbLY > -DEADZONE) {

			m_State[i].Gamepad.sThumbLY = 0;
		}

		if ((m_State[i].Gamepad.sThumbRX < DEADZONE &&
			m_State[i].Gamepad.sThumbRX > -DEADZONE)) {
			m_State[i].Gamepad.sThumbRX = 0;
		}
		if (m_State[i].Gamepad.sThumbRY < DEADZONE &&
			m_State[i].Gamepad.sThumbRY > -DEADZONE) {

			m_State[i].Gamepad.sThumbRY = 0;
		}
	}
}

void Input::XInput::AddFunctionToLua(lua_State * L, std::string LuaVarName)
{
	//関数の登録を行う
	const luaL_Reg funcs[] = {
		{ "Press", Input::XInput::PressGlue },
		{ "Trigger", Input::XInput::TriggerGlue },
		{ "Release", Input::XInput::ReleaseGlue },
		{ "LeftTrigger", Input::XInput::LeftTriggerGlue },
		{ "RightTrigger", Input::XInput::RightTriggerGlue },
		{ "LeftStick", Input::XInput::LeftStickGlue },
		{ "RightStick", Input::XInput::RightStickGlue },
		{ NULL, NULL }
	};

	SetLuaGlobalObjct<XInput>(L, LuaVarName.c_str(), funcs, this);


	//キー値の登録を行う
	XInputLuaGlueStruct addVar[] = {
		{ "UpKey" ,XINPUT_GAMEPAD_DPAD_UP },
		{ "DownKey" ,XINPUT_GAMEPAD_DPAD_DOWN },
		{ "LeftKey" ,XINPUT_GAMEPAD_DPAD_LEFT },
		{ "RightKey" ,XINPUT_GAMEPAD_DPAD_RIGHT },
		{ "StartKey" ,XINPUT_GAMEPAD_START },
		{ "BackKey" ,XINPUT_GAMEPAD_BACK },
		{ "L3Key" ,XINPUT_GAMEPAD_LEFT_THUMB },
		{ "R3Key" ,XINPUT_GAMEPAD_RIGHT_THUMB },
		{ "L1Key" ,XINPUT_GAMEPAD_LEFT_SHOULDER },
		{ "R1Key" ,XINPUT_GAMEPAD_RIGHT_SHOULDER },
		{ "AKey" ,XINPUT_GAMEPAD_A },
		{ "BKey" ,XINPUT_GAMEPAD_B },
		{ "XKey" ,XINPUT_GAMEPAD_X },
		{ "YKey" ,XINPUT_GAMEPAD_Y },
	};

	for (int i = 0; i < sizeof(addVar) / sizeof(XInputLuaGlueStruct); i++) {
		lua_pushnumber(L, addVar[i].vNumber);
		lua_setglobal(L, addVar[i].vVarName.c_str());
	}
}

unsigned Input::XInput::ConvertKeyboardToGamePad(unsigned key)
{
	unsigned type = 0;
	switch (key)
	{
	case XINPUT_GAMEPAD_DPAD_UP:
		type = DIK_W;
		break;
	case XINPUT_GAMEPAD_DPAD_DOWN:
		type = DIK_S;
		break;
	case XINPUT_GAMEPAD_DPAD_LEFT:
		type = DIK_A;
		break;
	case XINPUT_GAMEPAD_DPAD_RIGHT:
		type = DIK_D;
		break;
	case XINPUT_GAMEPAD_START:
		type = DIK_C;
		break;
	case XINPUT_GAMEPAD_BACK:
		type = DIK_Z;
		break;
	case XINPUT_GAMEPAD_LEFT_THUMB:
		type = DIK_R;
		break;
	case XINPUT_GAMEPAD_RIGHT_THUMB:
		type = DIK_T;
		break;
	case XINPUT_GAMEPAD_LEFT_SHOULDER:
		type = DIK_Q;
		break;
	case XINPUT_GAMEPAD_RIGHT_SHOULDER:
		type = DIK_E;
		break;
	case XINPUT_GAMEPAD_A:
		type = DIK_DOWN;
		break;
	case XINPUT_GAMEPAD_B:
		type = DIK_RIGHT;
		break;
	case XINPUT_GAMEPAD_X:
		type = DIK_LEFT;
		break;
	case XINPUT_GAMEPAD_Y:
		type = DIK_UP;
		break;
	}

	return type;
}

int Input::XInput::PressGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<XInput>(L);

	//引数を取得していく
	const int key = (float)lua_tonumber(L, 2);

	//関数実行
	bool flag = p->Press(key);
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	//戻り値を格納
	lua_pushboolean(L, flag);

	return 1;		//戻り値
}

int Input::XInput::TriggerGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<XInput>(L);

	//引数を取得していく
	const int key = (float)lua_tonumber(L, 2);

	//関数実行
	bool flag = p->Trigger(key);
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	//戻り値を格納
	lua_pushboolean(L, flag);

	return 1;		//戻り値
}

int Input::XInput::ReleaseGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<XInput>(L);

	//引数を取得していく
	const int key = (float)lua_tonumber(L, 2);

	//関数実行
	bool flag = p->Release(key);
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

						//戻り値を格納
	lua_pushboolean(L, flag);

	return 1;		//戻り値
}

int Input::XInput::LeftTriggerGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<XInput>(L);

	//関数実行
	float strength = p->LeftTrigger();
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	//戻り値を格納
	lua_pushnumber(L, strength);

	return 1;		//戻り値
}

int Input::XInput::RightTriggerGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<XInput>(L);

	//関数実行
	float strength = p->RightTrigger();
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

						//戻り値を格納
	lua_pushnumber(L, strength);

	return 1;		//戻り値
}

int Input::XInput::LeftStickGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<XInput>(L);

	//関数実行
	DirectX::XMFLOAT2 strength = p->LeftStick();
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

						//戻り値を格納
	lua_pushnumber(L, strength.x);
	lua_pushnumber(L, strength.y);


	return 2;		//戻り値
}

int Input::XInput::RightStickGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<XInput>(L);

	//関数実行
	DirectX::XMFLOAT2 strength = p->RightStick();
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

						//戻り値を格納
	lua_pushnumber(L, strength.x);
	lua_pushnumber(L, strength.y);


	return 2;		//戻り値
}

