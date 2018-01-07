/*=================================================
//								FILE : LuaActor.cpp
//		�t�@�C������ :
//		Lua�̃X�N���v�g����舵���N���X���`����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "LuaActor.h"
#include "LuaManager.h"
#include "LuaCoroutine.h"
#include <Windows.h>
#include <stdlib.h>

namespace {
	//�X�N���v�g���i�[���Ă���t�H���_�܂ł̃p�X
	std::string ScriptFileDIrePathName = "Script/";
}

cLuaActor::cLuaActor(const std::string _FileName, bool LibLoadFlag)
{
	m_FileName = _FileName;
	//���g���f�[�^��ǂݍ��񂾂��Ƃ��}�l�[�W���ɊǗ�������
	cLuaManager::GetInstance().AddLuaScript(this, _FileName);

	m_L = luaL_newstate();
	if (LibLoadFlag)
		luaL_openlibs(m_L);		//���ׂẴ��C�u�������J���Ă���

		//Lua_State�Ƀf�o�b�O����ς�ł���
	lua_getglobal(m_L, "debug");	//"debug"�Ƃ�����������X�^�b�N�ɐς�
	//�X�^�b�N�̍ŏ㕔��nil���ǂ����H
	if (!lua_isnil(m_L, -1)) {
		lua_getfield(m_L, -1, "traceback");	//���̏ꍇ-1�ɐς܂ꂽ�l[traceback]�̒l���X�^�b�N�ɐςނ̂�debug.traceback��ς񂾂��ƂɂȂ�
		m_pGetStackTraceFunc = lua_tocfunction(m_L, -1);		//debug.traceback��C�̊֐��ɕϊ����Ă��̃|�C���^�������o�ŕێ�����
	}
}

cLuaActor::~cLuaActor()
{
	//�}�l�[�W���ɍ폜�������Ƃ�`����
	cLuaManager::GetInstance().EraseLuaScript(this, m_FileName);
	Exit();
}

bool cLuaActor::CallFunc(std::string funcname, int result_count)
{
	int old_top = lua_gettop(m_L);
	// �Ԃ�l�̓N���A���Ă���
	m_Ret.Clear();
	// �����^�C���G���[�����֐���ς�
	lua_pushcfunction(m_L, m_pGetStackTraceFunc);

	// �֐����݂���
	lua_getglobal(m_L, funcname.c_str());
	//�����Ɗ֐�������ꂽ�H
	if (!lua_isfunction(m_L, -1)) {
		std::string location = "calling function<" + funcname + ">";
		SetErr(location, "the function not found.");
		return false;
	}

	// pcall����уX�^�b�N����
	int res_call = CallSub(&m_Ret, result_count, &m_Param, old_top + 1);
	if (res_call != 0) {
		// �G���[���b�Z�[�W����
		std::string location = "calling function<" + funcname + ">";
		AnalyzeError(res_call, location);
	}
	lua_settop(m_L, old_top); // �X�^�b�N�����ɖ߂�
	return (res_call == 0);	// true : �֐�����
}

bool cLuaActor::DoFile(std::string path,int result_count)
{
	int old_top = lua_gettop(m_L);
	// �Ԃ�l�̓N���A���Ă���
	m_Ret.Clear();
	// �����^�C���G���[�����֐���ς�
	lua_pushcfunction(m_L, m_pGetStackTraceFunc);

	// �t�@�C�������[�h
	int res_load = luaL_loadfile(m_L, path.c_str());
	if (res_load != 0) {
		// �G���[���b�Z�[�W����
		std::string location = "loading file<" + path + ">";
		AnalyzeError(res_load, location);
		lua_settop(m_L, old_top); // �X�^�b�N�����ɖ߂�
		return false;
	}
	// pcall����уX�^�b�N����
	int res_call = CallSub(&m_Ret, result_count, &m_Param, old_top + 1);
	if (res_call != 0) {
		// �G���[���b�Z�[�W����
		std::string location = "executing file<" + path + ">";
		AnalyzeError(res_load, location);
		AnalyzeError(res_call, location);
	}
	m_Param.ParamCuntClear();	//�����̐����N���A���Ă���
	lua_settop(m_L, old_top); // �X�^�b�N�����ɖ߂�
	return (res_call == 0);
}

void cLuaActor::ScriptCommit()
{
	//�ǂݍ��݂̃G���[�������Ȃ�܂Ń��[�h�������s��
	while (luaL_dofile(m_L, (ScriptFileDIrePathName + m_FileName).c_str()) != 0) { //�����Ńt�H���_�̃p�X�����������ăt�@�C�����s
																				  //�G���[�B�ǂݍ��ݎ��s
		std::string ErrMessage = m_FileName + " �̃��[�h���s�B�����[�h���܂�";
		int res = MessageBox(0, ErrMessage.c_str(), NULL, MB_OKCANCEL);
		//�L�����Z�����ꂽ�烋�[�v�𔲂���
		if (res == IDCANCEL)
			break;
	}

	//�f�o�b�O���[�h���̂݃G�f�B�^���J��
#if defined(DEBUG) || defined(_DEBUG)
	/*char szFullPath[256] = { "\0" };
	char* szFilePath = nullptr;
	const char* path = (ScriptFileDIrePathName + m_FileName).c_str();

	_fullpath(szFullPath, (ScriptFileDIrePathName).c_str(), sizeof(szFullPath) / sizeof(szFullPath[0]));

	std::string AllPath = std::string(szFullPath) + (m_FileName);
	HINSTANCE hr = ShellExecute(NULL, "open", AllPath.c_str(), NULL, NULL, SW_HIDE);*/
#endif

	//Lua�̃X�^�b�N����ɂ��Ă���
	lua_settop(m_L, 0);
}

void cLuaActor::PushCoroutine(cLuaCoroutine * pCol)
{
	m_pCoroutines.push_back(pCol);
}

void cLuaActor::ScriptReload()
{
	//�܂��t�@�C����ǂݒ����Ă���R���[�`����ǂݒ���
	ScriptCommit();
	for (auto itr = m_pCoroutines.begin(); itr != m_pCoroutines.end(); itr++) {
		(*itr)->SetLuaNewCoroutine(m_L);
	}
}

void cLuaActor::Exit()
{
	lua_close(m_L);
	m_L = nullptr;
}

void cLuaActor::SetErr(std::string location, std::string message)
{
	m_err = location + message;
	//�f�o�b�O���[�h���̂݃G���[���o�����Ƃ�\��������
#if defined(DEBUG) || defined(_DEBUG)
	MessageBox(0, m_err.c_str(), NULL, MB_OK);
#endif
}

/// <summary>
/// �����ɗ���̂̓G���[���N�������Ƃ��m�肵�Ă���B�G���[���l����G���[���e�ɕϊ�����
/// </summary>
/// <param name="res_call"></param>
/// <param name="location"></param>
void cLuaActor::AnalyzeError(int res_call, std::string location)
{
	std::string reason = "";
	switch (res_call) {
	case LUA_ERRRUN: reason = "SCRIPT RUNTIME ERROR"; break;
	case LUA_ERRSYNTAX: reason = "SCRIPT SYNTAX ERROR"; break;
	case LUA_ERRMEM: reason = "SCRIPT MEMORY ERROR"; break;
	case LUA_ERRFILE: reason = "SCRIPT FILE ERROR"; break;
	default: break;
	}
	// �G���[���b�Z�[�W�擾
	std::string err_mes = reason + lua_tostring(m_L, -1);
	SetErr(location, err_mes);
}

/// <summary>
/// ���ۂɊ֐����Ă�
/// </summary>
/// <param name="result"></param>
/// <param name="result_count"></param>
/// <param name="params"></param>
/// <param name="errfunc_index"></param>
/// <returns></returns>
int cLuaActor::CallSub(LuaFuncParam * result, int result_count, LuaFuncParam * params, int errfunc_index)
{
	// params�ɂ���Ďw�肳�ꂽ�ό̈������X�^�b�N�ɒu��
	int params_count = 0;
	if (params) {
		params_count = params->PushToStack(m_L);
	}
	// ���[�h���ꂽ�`�����N���R�[��
	int res_call = lua_pcall(m_L, params_count, result_count, errfunc_index);
	if (res_call != 0) {
		// �G���[����
		return res_call;
	}
	// �Ԃ�l���
	if (result) result->GetFromStack(m_L, result_count);
	return res_call;
}
