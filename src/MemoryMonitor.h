/*=================================================
//								FILE : MemoryMonitor.h
//		�t�@�C������ :
//		�������[�g�p�󋵂��擾���Ǘ�����N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"

struct MemoryData {
	DWORDLONG vUsed;		//�g�p��������
	DWORDLONG vFree;		//�󂫗e��
	DWORDLONG vSize;		//������������
	DWORDLONG vInit;		//�v���O�����J�n���̃�������
};

/// <summary>
/// �������̊Ǘ��N���X
/// </summary>
class cMemoryMonitor {
public:
	//�X�V���������������s�����߂�cDirectX�N���X���t�����h�����Ă���
	friend class cDirectX11;
	cMemoryMonitor(const cMemoryMonitor&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cMemoryMonitor& operator=(const cMemoryMonitor&) = delete;	//�R�s�[������Z�q�� delete �w��
	cMemoryMonitor(cMemoryMonitor&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cMemoryMonitor& operator=(cMemoryMonitor&&) = delete;		//���[�u������Z�q�� delete �w��

	static cMemoryMonitor& GetInstance() {
		static cMemoryMonitor inst;			// private�ȃR���X�g���N�^���Ăяo��
		return inst;
	}
	MemoryData GetMemoryData()const;

private:
	void Update();
	cMemoryMonitor();						//�R���X�g���N�^�� private �ɒu��
	~cMemoryMonitor() = default;	//�f�X�g���N�^�� private �ɒu��
	MemoryData m_MemoryData;		//����������ێ�����
};