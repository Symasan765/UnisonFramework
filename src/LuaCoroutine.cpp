/*=================================================
//								FILE : LuaCoroutine
//		�t�@�C������ :
//		Lua��̃R���[�`������舵���N���X���`����
//		LuaActor�N���X����LuaState���擾���Ďg�p����
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "LuaCoroutine.h"
#include <Windows.h>

cLuaCoroutine::cLuaCoroutine(cLuaActor* pActor, const std::string _FuncName, int result_count)
{
	pActor->PushCoroutine(this);	//�Q�Ƃ��A�N�^�[�ɊǗ����Ă��炤
	m_ParentL = pActor->GetLuaState();
	m_FuncName = _FuncName;
	m_RetNum = result_count;
	
	SetLuaNewCoroutine(m_ParentL);
}

cLuaCoroutine::~cLuaCoroutine()
{
	// �R���[�`���̍폜
	//LUA_REGISTRYINDEX[m_RegistryIndex] = nil�Ƃ����o�^���s��
	/*lua_pushnil(m_ParentL);
	lua_pushlightuserdata(m_ParentL, (void*)m_RegistryIndex.c_str());
	lua_settable(m_ParentL, LUA_REGISTRYINDEX);*/		// TODO ���󂱂��Ń����^�C���G���[����������B�����s��
}

void cLuaCoroutine::SetLuaNewCoroutine(lua_State * L)
{
	int top = lua_gettop(L);
	static int TortalNum = 0;
	//�K�[�x�W�R���N�^�΍�ŃR���[�`����e�̃��W�X�g���ɕێ����Ă���
	//LUA_REGISTRYINDEX��Lua���W���ŗp�ӂ��郌�W�X�g���e�[�u��
	m_RegistryIndex = m_FuncName + "_CorName" + std::to_string(TortalNum);
	TortalNum++;
	lua_pushlightuserdata(L, (void*)m_RegistryIndex.c_str());
	m_Thread = lua_newthread(L);
	// �ȉ��̖��߂�LUA_REGISTRYINDEX[m_RegistryIndex] = m_Thread�Ƃ����o�^�ɂȂ�B����ŃX�^�b�N����X���b�h�������Ă�GC�̑Ώۂ���O���
	lua_settable(L, LUA_REGISTRYINDEX);

	//�R���[�`�����擾���ăX�^�b�N�ɐς�
	lua_getglobal(m_Thread, m_FuncName.c_str());

	lua_settop(L, top);
}

void cLuaCoroutine::Reload()
{
	SetLuaNewCoroutine(m_ParentL);
}

/// <summary>
/// 
/// </summary>
/// <param name="funcname"></param>
/// <param name="result_count"></param>
/// <returns>true : ���s�ł���</returns>
bool cLuaCoroutine::Execution()
{
	static int s = 0;
	if (m_Thread == NULL)
		int a = 10;
	s++;
	int old_top = lua_gettop(m_Thread);
	// �Ԃ�l�̓N���A���Ă���
	m_Ret.Clear();

	int params_count = m_Param.PushToStack(m_Thread);
	// resume����уX�^�b�N����
	int res_call = lua_resume(m_Thread, nullptr, params_count);
	while (res_call != 0) {
		if (res_call == LUA_YIELD) {
			// �Ԃ�l���
			m_Ret.GetFromStack(m_Thread, m_RetNum);
			lua_settop(m_Thread, old_top); // �X�^�b�N�����ɖ߂�
			return true;		//���f���������Ȃ琳��
		}
		// �G���[���b�Z�[�W����
		std::string location = "calling function<" + m_FuncName + ">";
		AnalyzeError(res_call, location);
		int res = MessageBox(0, "�ēx�R���[�`���̎��s�������܂����H", NULL, MB_OKCANCEL);
		//�L�����Z�����ꂽ�烋�[�v�𔲂���
		if (res == IDCANCEL) {
			break;
		}
		res_call = lua_resume(m_Thread, nullptr, params_count);
	}
	// �Ԃ�l���
	m_Ret.GetFromStack(m_Thread, m_RetNum);
	lua_settop(m_Thread, old_top); // �X�^�b�N�����ɖ߂�
	return (res_call == 0);	// true : �֐�����
}


void cLuaCoroutine::AnalyzeError(int res_call, std::string location)
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
	std::string err_mes = reason + lua_tostring(m_Thread, -1);
	//�f�o�b�O���[�h���̂݃G���[���o�����Ƃ�\��������
#if defined(DEBUG) || defined(_DEBUG)
	MessageBox(0, (location + err_mes).c_str(), NULL, MB_OK);
#endif
}