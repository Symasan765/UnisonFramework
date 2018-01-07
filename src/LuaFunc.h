/*=================================================
//								FILE : LuaFunc.h
//		�t�@�C������ :
//		Lua�̊֐��ŗ��p��������A�߂�l�Ɋւ���N���X���`����
//		�Q�l�Z�p�� : �X�N���v�g����ɂ������I�Q�[���J��
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include <lua.hpp>

// TODO �p�����[�^�̐��𓮓I�z��ɂ���Ƃ���

// �p�����[�^�̂P�̒l���i�[����N���X
class LuaFuncParamItem
{
public:
	unsigned int m_type; //�p�����[�^�^�C�v�FLUA_TNIL/LUA_TNUMBER/LUA_TSTRING/LUA_TBOOLEAN
	union {
		float m_number; // ���l�̏ꍇ�̒l
		char *m_str; // ������̏ꍇ�̒l
		bool m_bool; // �u�[���̏ꍇ�̒l
	};

public:
	LuaFuncParamItem() : m_type(LUA_TNIL), m_number(0) {}
	~LuaFuncParamItem() { ReleaseValue(); }

	// �^�擾�֐�
	unsigned int GetType() const { return m_type; }

	// �^�`�F�b�N�֐�
	bool IsNil()    const { return m_type == LUA_TNIL; }
	bool IsNumber() const { return m_type == LUA_TNUMBER; }
	bool IsString() const { return m_type == LUA_TSTRING; }
	bool IsBool()   const { return m_type == LUA_TBOOLEAN; }

	// �l�擾�֐�
	float GetNumber()        const { return m_number; }
	const char * GetString()  const { return m_str; }
	bool GetBool()            const { return m_bool; }

	// �o�b�t�@���
	void ReleaseValue();

	// ���l���Z�b�g
	void SetNumber(float number);
	// ������l���Z�b�g
	void SetString(const char * str);
	// nil�l���Z�b�g
	void SetNil();
	// bool�l���Z�b�g
	void SetBool(bool value);

	// �i�[���Ă���l��Lua�X�^�b�N�ɐς�
	void PushToStack(lua_State *L);
};

// ���[�U�[���璼�ڎg���֐��p�����[�^�N���X
// LuaFuncParam param;
// param.Number(10).String("hello").Nil().Number(50);
// �̂悤�Ɏg����B
class LuaFuncParam
{
private:
	enum {
		PARAMS_MAX = 20, // �ő�p�����[�^��
	};
	LuaFuncParamItem m_params[PARAMS_MAX];
	int m_params_count;

public:
	LuaFuncParam() : m_params_count(0) {}
	~LuaFuncParam() { Clear(); }

	// �p�����[�^��S�N���A
	void Clear();
	// �p�����[�^����Ԃ�
	int GetCount() { return m_params_count; }

	// �e��p�����[�^�̒ǉ�
	LuaFuncParam & Number(float number);
	LuaFuncParam & String(const char *str);
	LuaFuncParam & Nil();
	LuaFuncParam & Bool(bool value);

	// �w��C���f�b�N�X�̃p�����[�^�l�擾
	// (�C���f�b�N�X�͂O�x�[�X�j
	bool IsNil(int index);
	float GetNumber(int index);
	const char * GetString(int index);
	const bool GetBool(int index);

	void ParamCuntClear() { m_params_count = 0; }

	// Lua�X�^�b�N�Ɉ������v�b�V�����āA�v�b�V�����������̐���Ԃ�
	int PushToStack(lua_State *L);
	// Lua�X�^�b�N����l���擾
	// �X�^�b�N�g�b�v����result_count�̒l���擾���Ċi�[
	void GetFromStack(lua_State *L, int result_count);
};