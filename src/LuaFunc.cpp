#include "LuaFunc.h"
#include <string.h>

// �o�b�t�@���
void LuaFuncParamItem::ReleaseValue() {
	if (m_type == LUA_TSTRING) {
		if (m_str) delete[] m_str;
		m_str = NULL;
	}
}

// ���l���Z�b�g
void LuaFuncParamItem::SetNumber(float number) {
	ReleaseValue();
	m_type = LUA_TNUMBER;
	m_number = number;
}
// ������l���Z�b�g
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
// nil�l���Z�b�g
void LuaFuncParamItem::SetNil() {
	ReleaseValue();
	m_type = LUA_TNIL;
}
// bool�l���Z�b�g
void LuaFuncParamItem::SetBool(bool value) {
	ReleaseValue();
	m_type = LUA_TBOOLEAN;
	m_bool = value;
}

// �i�[���Ă���l��Lua�X�^�b�N�ɐς�
void LuaFuncParamItem::PushToStack(lua_State *L) {
	switch (m_type) {
	case LUA_TNUMBER: lua_pushnumber(L, m_number); break;
	case LUA_TSTRING: lua_pushstring(L, m_str); break;
	case LUA_TNIL: lua_pushnil(L); break;
	case LUA_TBOOLEAN: lua_pushboolean(L, m_bool); break;
	}
}

//
//  class LuaFuncParam�F�����̃p�����[�^��Ԃ�l��\���N���X
//

// �p�����[�^��S�N���A
void LuaFuncParam::Clear() {
	for (int i = 0; i<m_params_count; i++) {
		m_params[i].ReleaseValue();
	}
	m_params_count = 0;
}

// ���l�p�����[�^�̒ǉ�
LuaFuncParam & LuaFuncParam::Number(float number) {
	m_params[m_params_count].SetNumber(number);
	m_params_count++;
	return *this;
}
// ������p�����[�^�̒ǉ�
LuaFuncParam & LuaFuncParam::String(const char *str) {
	m_params[m_params_count].SetString(str);
	m_params_count++;
	return *this;
}
// nil�p�����[�^�̒ǉ�
LuaFuncParam & LuaFuncParam::Nil() {
	m_params[m_params_count].SetNil();
	m_params_count++;
	return *this;
}
// �u�[���l�p�����[�^�̒ǉ�
LuaFuncParam & LuaFuncParam::Bool(bool value) {
	m_params[m_params_count].SetBool(value);
	m_params_count++;
	return *this;
}

// �w��C���f�b�N�X�̃p�����[�^��NULL�`�F�b�N
// (�C���f�b�N�X�͂O�x�[�X�j
bool LuaFuncParam::IsNil(int index) {
	if (index < 0 || index >= m_params_count) return true;
	return m_params[index].IsNil();
}
// �w��C���f�b�N�X�̃p�����[�^���l�擾
// (�C���f�b�N�X�͂O�x�[�X�j
float LuaFuncParam::GetNumber(int index) {
	if (index < 0 || index >= m_params_count) return 0;
	if (m_params[index].GetType() != LUA_TNUMBER) {
		return 0;
	}
	return m_params[index].GetNumber();
}
// �w��C���f�b�N�X�̃p�����[�^������擾
// (�C���f�b�N�X�͂O�x�[�X�j
const char * LuaFuncParam::GetString(int index) {
	if (index < 0 || index >= m_params_count) return NULL;
	if (m_params[index].GetType() != LUA_TSTRING) {
		return NULL;
	}
	return m_params[index].GetString();
}
// �w��C���f�b�N�X�̃u�[���l�擾
// (�C���f�b�N�X�͂O�x�[�X�j
const bool LuaFuncParam::GetBool(int index) {
	if (index < 0 || index >= m_params_count) return NULL;
	if (m_params[index].GetType() != LUA_TBOOLEAN) {
		return NULL;
	}
	return m_params[index].GetBool();
}

// Lua�X�^�b�N�Ɉ������v�b�V�����āA�v�b�V�����������̐���Ԃ�
int LuaFuncParam::PushToStack(lua_State *L) {
	for (int i = 0; i<m_params_count; i++) {
		m_params[i].PushToStack(L);
	}
	return m_params_count;
}

// Lua�X�^�b�N����l���擾
// �X�^�b�N�g�b�v����result_count�̒l���擾���Ċi�[
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