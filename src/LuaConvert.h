/*=================================================
//								FILE : LuaConvert.h
//		�t�@�C������ :
//		Lua��C++�ԂŃf�[�^�ϊ����s���֐���o�^���Ă���
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once

/// <summary>
/// Lua�̃f�[�^�ϊ��ƈ����֐��Q
/// </summary>
namespace LuaConv {

	/// <summary>
	/// LuaState�̃X�^�b�N�擪���w��^�̃A�h���X�ɕϊ����ĕԂ��֐�
	/// </summary>
	template<typename T>
	T* toPointer(lua_State* L) {
		T** buf = static_cast<T**>(lua_touserdata(L, lua_upvalueindex(1)));
		T* point = static_cast<T*>(*buf);

		return point;
	};


	/// <summary>
	/// �n���ꂽLuaState��ɃO���[�o���ϐ����Z�b�g���A�����Ɋ֐��Q���Z�b�g����
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