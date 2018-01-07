/*=================================================
//								FILE : LuaManager.h
//		�t�@�C������ :
//		Lua�X�N���v�g���Ǘ�����}�l�[�W���N���X
//		�����[�h�����Ȃǂ�S�����邱�ƂɂȂ�
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include <map>
#include <vector>
#include "LuaActor.h"

class cComboBox;

/// <summary>
/// Lua�X�N���v�g���Ǘ�����N���X
/// </summary>
class cLuaManager {
public:
	cLuaManager(const cLuaManager&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cLuaManager& operator=(const cLuaManager&) = delete;	//�R�s�[������Z�q�� delete �w��
	cLuaManager(cLuaManager&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cLuaManager& operator=(cLuaManager&&) = delete;		//���[�u������Z�q�� delete �w��

	static cLuaManager& GetInstance() {
		static cLuaManager inst;			// private�ȃR���X�g���N�^���Ăяo��
		return inst;
	}

	void AddLuaScript(cLuaActor* pLuaAct,std::string FileName);
	void EraseLuaScript(cLuaActor* pLuaAct, std::string FileName);
	void SetTheNameInTheComboBox(cComboBox* pComboBox);
	void ReloadTheScript(std::string FileName);
private:
	cLuaManager() = default;					//�R���X�g���N�^�� private �ɒu��
	~cLuaManager() = default;					//�f�X�g���N�^�� private �ɒu��

	std::map<std::string, std::vector<cLuaActor*>> m_ScriptMap;		//�X�N���v�g�̗��p�󋵂��Ǘ�����
};