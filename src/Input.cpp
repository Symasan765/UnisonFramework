/*=========================================================//
//		�T�v	:	���͏������L�q����
//					�쐬�ҁ@���{ �Y�V�� (AT12A606)
//						�쐬��	2016/11/2
//					  �X�V��
//
//=========================================================*/
#include "Input.h"
#include "WinMain.h"
#include "LuaConvert.h"
using namespace LuaConv;

#define DIRECTINPUT_VERSION 0x0800
//#define DEADZONE XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
#define DEADZONE 12000

//���̃t�@�C������B�؂�
namespace {
	const int NUM_KEY_MAX = 256;
	LPDIRECTINPUT8			pDInput = NULL;						// IDirectInput8�C���^�[�t�F�[�X�ւ̃|�C���^
	LPDIRECTINPUTDEVICE8	pDIDevKeyboard = NULL;				// IDirectInputDevice8�C���^�[�t�F�[�X�ւ̃|�C���^(�L�[�{�[�h)
	LPDIRECTINPUTDEVICE8	pDIDevMouse = NULL;					// IDirectInputDevice8�C���^�[�t�F�[�X�ւ̃|�C���^(�}�E�X)
	BYTE						keyState[NUM_KEY_MAX];				// �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
	BYTE						keyStateTrigger[NUM_KEY_MAX];		// �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
	BYTE						keyStateRepeat[NUM_KEY_MAX];		// �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
	BYTE						keyStateRelease[NUM_KEY_MAX];		// �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
	int							keyStateRepeatCnt[NUM_KEY_MAX];	// �L�[�{�[�h�̃��s�[�g�J�E���^
	DIMOUSESTATE2			mouseState;							// �}�E�X�̏�Ԃ��󂯎�郏�[�N
	DIMOUSESTATE2			mouseStateTrigger;					// �}�E�X�̏�Ԃ��󂯎�郏�[�N
	POINT						MousePoint;							// �}�E�X�̌��ݍ��W

	struct XInputLuaGlueStruct {
		std::string vVarName;		//Lua�ɓo�^����ϐ���
		unsigned int vNumber;		//�o�^��������l
	};
}
using namespace Input;

cKeyboard::cKeyboard() {
	HRESULT hr;
	if (!pDInput)
	{
		// DirectInput�I�u�W�F�N�g�̍쐬
		hr = DirectInput8Create(GetDirectX::HInstance(), DIRECTINPUT_VERSION,
			IID_IDirectInput8, (void**)&pDInput, NULL);
	}
	InitKeyboard(GetDirectX::HInstance(), GetDirectX::Hwnd());
}

HRESULT cKeyboard::InitKeyboard(HINSTANCE hInst, HWND hWnd) {
	HRESULT hr = 0;

	// �f�o�C�X�I�u�W�F�N�g���쐬
	hr = pDInput->CreateDevice(GUID_SysKeyboard, &pDIDevKeyboard, NULL);
	if (FAILED(hr) || pDIDevKeyboard == NULL)
	{
		MessageBox(hWnd, "�L�[�{�[�h���˂��I", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	hr = pDIDevKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�L�[�{�[�h�̃f�[�^�t�H�[�}�b�g��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	hr = pDIDevKeyboard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�L�[�{�[�h�̋������[�h��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �L�[�{�[�h�ւ̃A�N�Z�X�����l��(���͐���J�n)
	pDIDevKeyboard->Acquire();

	return S_OK;
}

HRESULT cKeyboard::UpdateKeyboard(void) {
	HRESULT hr;
	BYTE keyStateOld[256];

	// �O��̃f�[�^��ۑ�
	memcpy(keyStateOld, keyState, NUM_KEY_MAX);

	// �f�o�C�X����f�[�^���擾
	hr = pDIDevKeyboard->GetDeviceState(sizeof(keyState), keyState);
	if (SUCCEEDED(hr))
	{
		for (int cnt = 0; cnt < NUM_KEY_MAX; cnt++)
		{
			// �g���K�[�ƃ����[�X���擾
			keyStateTrigger[cnt] = (keyState[cnt] ^ keyStateOld[cnt]) & keyState[cnt];
			keyStateRelease[cnt] = (keyState[cnt] ^ keyStateOld[cnt]) & keyStateOld[cnt];

			// �L�[��������Ă���Ȃ烊�s�[�g�̔��菈��
			if (keyState[cnt])
			{
				if (keyStateRepeatCnt[cnt] < 20)
				{
					keyStateRepeatCnt[cnt]++;
					// �u������́v�������́u�{�^���������Ă���20�t���[���o�߁v
					// �����ꍇ�A���s�[�g�p�z��̃f�[�^��L���ɂ���
					// ��L�����ȊO�̓��s�[�g�p�z��̃f�[�^�𖳌��ɂ���
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
		// �L�[�{�[�h�ւ̃A�N�Z�X�����擾
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
	// �L�[�{�[�h�̏I������
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
		//���݂̃L�[�ƑO�̃L�[�Ƃ̍��������Ŕ�����s��
		bool nowKey = m_State[no].Gamepad.wButtons & key;
		bool oldKey = m_OldState[no].Gamepad.wButtons & key;
		//�g���K�[���菈��
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
		//���݂̃L�[�ƑO�̃L�[�Ƃ̍��������Ŕ�����s��
		bool nowKey = m_State[no].Gamepad.wButtons & key;
		bool oldKey = m_OldState[no].Gamepad.wButtons & key;
		//�����[�X���菈��
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
		//���l���`�F�b�N���A0.0~1.0�̊Ԃŕԋp����
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
		//���l���`�F�b�N���A0.0~1.0�̊Ԃŕԋp����
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
		//�R���g���[�����Ȃ����Ă��Ȃ�
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
		//�R���g���[�����Ȃ����Ă��Ȃ�
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
	//�X�e�[�g�ƃX�g���[�N���擾����
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
		m_OldState[i] = m_State[i];		//�Â��L�[�����i�[����
		DWORD hr = XInputGetState(i, &m_State[i]);	//�L�[�擾

		//�Q�[���p�b�h���ڑ�����Ă��Ȃ�
		if (hr == ERROR_DEVICE_NOT_CONNECTED) {
			conect[i] = false;		//�ڑ��t���O�I�t
			continue;
		}

		//�ڑ�����Ă���
		conect[i] = true;

		//�f�b�h�]�[���̐ݒ�B�f�b�h�]�[�����Ȃ琔�l��0�ɕύX���Ă���
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
	//�֐��̓o�^���s��
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


	//�L�[�l�̓o�^���s��
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
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<XInput>(L);

	//�������擾���Ă���
	const int key = (float)lua_tonumber(L, 2);

	//�֐����s
	bool flag = p->Press(key);
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	//�߂�l���i�[
	lua_pushboolean(L, flag);

	return 1;		//�߂�l
}

int Input::XInput::TriggerGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<XInput>(L);

	//�������擾���Ă���
	const int key = (float)lua_tonumber(L, 2);

	//�֐����s
	bool flag = p->Trigger(key);
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	//�߂�l���i�[
	lua_pushboolean(L, flag);

	return 1;		//�߂�l
}

int Input::XInput::ReleaseGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<XInput>(L);

	//�������擾���Ă���
	const int key = (float)lua_tonumber(L, 2);

	//�֐����s
	bool flag = p->Release(key);
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

						//�߂�l���i�[
	lua_pushboolean(L, flag);

	return 1;		//�߂�l
}

int Input::XInput::LeftTriggerGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<XInput>(L);

	//�֐����s
	float strength = p->LeftTrigger();
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	//�߂�l���i�[
	lua_pushnumber(L, strength);

	return 1;		//�߂�l
}

int Input::XInput::RightTriggerGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<XInput>(L);

	//�֐����s
	float strength = p->RightTrigger();
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

						//�߂�l���i�[
	lua_pushnumber(L, strength);

	return 1;		//�߂�l
}

int Input::XInput::LeftStickGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<XInput>(L);

	//�֐����s
	DirectX::XMFLOAT2 strength = p->LeftStick();
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

						//�߂�l���i�[
	lua_pushnumber(L, strength.x);
	lua_pushnumber(L, strength.y);


	return 2;		//�߂�l
}

int Input::XInput::RightStickGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<XInput>(L);

	//�֐����s
	DirectX::XMFLOAT2 strength = p->RightStick();
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

						//�߂�l���i�[
	lua_pushnumber(L, strength.x);
	lua_pushnumber(L, strength.y);


	return 2;		//�߂�l
}

