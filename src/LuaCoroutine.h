/*=================================================
//								FILE : LuaCoroutine
//		�t�@�C������ :
//		Lua��̃R���[�`������舵���N���X���`����
//		LuaActor�N���X����LuaState���擾���Ďg�p����
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include <lua.hpp>
#include <string>
#include "LuaActor.h"

/// <summary>
/// �ЂƂ�Lua�R���[�`������舵���N���X
/// </summary>
class cLuaCoroutine {
public:
	cLuaCoroutine(cLuaActor* pActor, const std::string _FuncName, int result_count);
	~cLuaCoroutine();
	void SetLuaNewCoroutine(lua_State* L);
	void Reload();

	bool Execution();

	LuaFuncParam m_Ret;		//�߂�l�p
	LuaFuncParam m_Param;		//�Ԓl�p
private:
	void AnalyzeError(int res_call, std::string location);
	lua_State* m_Thread;
	std::string m_RegistryIndex;		//�R���[�`�����X�^�b�N���珜���������Ƃ�GC�΍��p
	lua_State* m_ParentL;				//���ӁI�R���[�`���폜�̍ۂ������p����B
	std::string m_FuncName;					//���s����֐���
	int m_RetNum;		//�߂�l�̐�
};