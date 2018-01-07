/*=================================================
//								FILE : ShaderMonitoring.h
//		�t�@�C������ :
//		�V�F�[�_�t�@�C���̍X�V���Ď����Č������
//		�����[�h�����𖽗߂���N���X
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include <vector>
#include <string>
#include "WinMain.h"
#include "TimeCheck.h"

#define SHADER_REROAD_TIME_SEC 5.0f

struct ShaderMonitStruct {
	std::string vFileName;
	DWORD vTime;				//���ԏ���ێ�
	HANDLE vFileHnd;

	ShaderMonitStruct() {
		vTime = 0;
		vFileHnd = NULL;
	}
};

class cShaderMonitoring {
public:
	cShaderMonitoring(const cShaderMonitoring&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cShaderMonitoring& operator=(const cShaderMonitoring&) = delete;	//�R�s�[������Z�q�� delete �w��
	cShaderMonitoring(cShaderMonitoring&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cShaderMonitoring& operator=(cShaderMonitoring&&) = delete;		//���[�u������Z�q�� delete �w��

	static cShaderMonitoring& GetInstance() {
		static cShaderMonitoring inst;			// private�ȃR���X�g���N�^���Ăяo��
		return inst;
	}

	void MonitorChanges();
	void AddFileDate(std::string _FileName);
private:
	cShaderMonitoring() = default;					//�R���X�g���N�^�� private �ɒu��
	~cShaderMonitoring();					//�f�X�g���N�^�� private �ɒu��
	DWORD GetFileChangeTime(HANDLE fileHnd);
	cTimeCheck m_TimeCnt;

	std::vector<ShaderMonitStruct> m_Data;
};