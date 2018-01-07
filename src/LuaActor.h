/*=================================================
//								FILE : LuaActor.h
//		�t�@�C������ :
//		Lua�̃X�N���v�g����舵���N���X���`����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include <lua.hpp>
#include <string>
#include <vector>
#include "LuaFunc.h"
class cLuaCoroutine;

/// <summary>
/// ���̃N���X�ŃQ�[���A�N�^�[1���̋@�\�Ǝ�舵������BLuaVM���쐬�����̂ō�肷���ɒ���
/// </summary>
class cLuaActor {
public:
	cLuaActor(const std::string _FileName,bool LibLoadFlag = true);
	virtual ~cLuaActor();
	//======================== �֐� ===============================
	inline lua_State *GetLuaState() { return m_L; };
	bool CallFunc(std::string funcname, int result_count = 0);
	bool DoFile(std::string path, int result_count = 0);
	void ScriptCommit();
	void PushCoroutine(cLuaCoroutine* pCol);
	void ScriptReload();

	LuaFuncParam m_Ret;		//�߂�l�p
	LuaFuncParam m_Param;		//�Ԓl�p
protected:
	void Exit();
	void SetErr(std::string location, std::string message);
	inline const std::string GetErr() { return m_err; }
	void AnalyzeError(int res_call, std::string location);
	int CallSub(LuaFuncParam* result, int result_count, LuaFuncParam* params, int errfunc_index);

	//======================== �ϐ� ===============================
	lua_State *m_L;		// Lua�X�e�[�g
	std::string m_err;	// �G���[������
	std::string m_FileName;
	std::vector<cLuaCoroutine*> m_pCoroutines;		//���̃X�N���v�g����h�������R���[�`���̎Q��
	lua_CFunction m_pGetStackTraceFunc;	// debug.traceback�̎����ւ̃|�C���^
};