/*=================================================
//								FILE : LuaManager.cpp
//		�t�@�C������ :
//		Lua�X�N���v�g���Ǘ�����}�l�[�W���N���X
//		�����[�h�����Ȃǂ�S�����邱�ƂɂȂ�
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "LuaManager.h"
#include "ComboBox.h"

/// <summary>
/// �X�N���v�g���}�l�[�W���ɓo�^����
/// </summary>
/// <param name="pL"></param>
/// <param name="FileName"></param>
/// <returns></returns>
void cLuaManager::AddLuaScript(cLuaActor* pLuaAct, std::string FileName)
{
	m_ScriptMap[FileName].push_back(pLuaAct);
}

/// <summary>
/// �X�N���v�g������map���������Ă��̗v�f��vector����폜����
/// </summary>
/// <param name="pLuaAct"></param>
/// <param name="FileName"></param>
void cLuaManager::EraseLuaScript(cLuaActor * pLuaAct, std::string FileName)
{
	//���̖��O��map�ɓo�^����Ă��邩�m�F����
	auto map_itr = m_ScriptMap.find(FileName);
	if (map_itr != m_ScriptMap.end()) {
		//�ݒ肳��Ă���ꍇ�̏���
		std::vector<cLuaActor*>& data = map_itr->second;
		for (auto itr = data.begin(); itr != data.end(); itr++) {
			if (pLuaAct == *itr) {
				map_itr->second.erase(itr);
				return;
			}
		}
	}
}

/// <summary>
/// ���ׂẴX�N���v�g�����R���{�{�b�N�X�ɒǉ�����
/// </summary>
/// <param name="pComboBox"></param>
void cLuaManager::SetTheNameInTheComboBox(cComboBox * pComboBox)
{
	pComboBox->DeleteAllItem();
	for (auto itr = m_ScriptMap.begin(); itr != m_ScriptMap.end(); ++itr) {
		pComboBox->AddItem(itr->first);
	}
}

void cLuaManager::ReloadTheScript(std::string FileName)
{
	auto map_itr = m_ScriptMap.find(FileName);
	if (map_itr == m_ScriptMap.end()) return;

	//�ݒ肳��Ă���ꍇ�̏���
	std::vector<cLuaActor*>& data = map_itr->second;
	for (auto itr = data.begin(); itr != data.end(); itr++) {
		// TODO ������cLuaActor�N���X�Œ�`���������[�h�֐����Ă�
		(*itr)->ScriptReload();
	}
}