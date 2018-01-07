/*=========================================================//
//		�T�v	:	���͏������L�q����
//					�쐬�ҁ@���{ �Y�V�� (AT12A606)
//						�쐬��	2016/11/2
//					  �X�V��
//	���͌n�̏������L�q����B�����I�ɂ͂����ɃQ�[���p�b�h��}�E�X������������
//=========================================================*/
#ifndef INC_INPUT_H
#define INC_INPUT_H

#define GetKey Input::cKeyboard::getInstance()		//�L�[�{�[�h���擾(�|�C���^)
#define GetGamePad Input::XInput::getInstance()		//XInput���擾(�|�C���^)

#include <dinput.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

#include <XInput.h>
#include <lua.hpp>
#include <string>
#include "WinMain.h"

//���͏����n�̃N���X���L�q����
namespace Input {

	//�L�[�{�[�h�N���X(�V���O���g��)
	class cKeyboard {
	public:
		//�X�V���������������s�����߂�cDirectX�N���X���t�����h�����Ă���
		friend class cDirectX11;
		bool Press(int key);
		bool Trigger(int key);
		bool Repeat(int key);
		bool Release(int key);
		HRESULT UpdateKeyboard(void);

		//����Ăяo�����Ɏ��̐���
		static cKeyboard* getInstance() {
			static cKeyboard key;
			return &key;
		}
	private:
		cKeyboard();
		HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd);
		void UninitKeyboard(void);

		cKeyboard(const cKeyboard&) = delete;				//�R�s�[�R���X�g���N�^�� delete �w��
		cKeyboard& operator=(const cKeyboard&) = delete;	//�R�s�[������Z�q�� delete �w��
		cKeyboard(cKeyboard&&) = delete;						//���[�u�R���X�g���N�^�� delete �w��
		cKeyboard& operator=(cKeyboard&&) = delete;			//���[�u������Z�q�� delete �w��
		~cKeyboard();
	};


	//XInput�N���X(�V���O���g��)
	class XInput {
	public:
		//�X�V���������������s�����߂�cDirectX�N���X���t�����h�����Ă���
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

		//����Ăяo�����Ɏ��̐���
		static XInput* getInstance() {
			static XInput xinput;
			return &xinput;
		}

		//�L�[���͊֐���Lua�ɓn���֐�
		void AddFunctionToLua(lua_State* L, std::string LuaVarName = "GamePad");

	private:
		XInput() = default;

		XInput(const XInput&) = delete;				//�R�s�[�R���X�g���N�^�� delete �w��
		XInput& operator=(const XInput&) = delete;	//�R�s�[������Z�q�� delete �w��
		XInput(XInput&&) = delete;						//���[�u�R���X�g���N�^�� delete �w��
		XInput& operator=(XInput&&) = delete;			//���[�u������Z�q�� delete �w��
		~XInput() = default;

		unsigned ConvertKeyboardToGamePad(unsigned);		//�Q�[���p�b�h���ڑ�����Ă��Ȃ��Ƃ��p�̃R���o�[�^�[�֐�

		//�e���ԕϐ�
		XINPUT_STATE m_State[4];		//�R���g���[��4�䕪�̏��
		XINPUT_STATE m_OldState[4];		//�R���g���[��4�䕪�̏��
		int conect[4];						//�ڑ�����Ă��邩�H



		//======================== Lua�pGlue�֐� ===============================
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