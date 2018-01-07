/*=================================================
//								FILE : ComboBox.h
//		�t�@�C������ :
//		�v���_�E�����j���[����舵���N���X���`����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include <Windows.h>
#include <string>

#define MAX_COMBOBOX_CHARA_LEN 30		//���X�g�̂P���ڂ̍ő啶����

class cComboBox {
public:
	cComboBox(HWND hwnd, LPARAM lp,int posX = 975,int posY = 0,int width = 300,int height = 300);
	~cComboBox();
	void display();		//�\��
	void Hide();			//��\��
	std::string GetSelectItem(HWND hwnd);	//�I�΂ꂽ�A�C�e����Ԃ�
	void DeleteAllItem();	//�ێ����鍀�ڂ�j������
	void AddItem(std::string _ItemName);
	void SetForeground();
private:
	HWND m_Combo;
	// TODO �������̃E�B���h�E�����ꍇ�A�R���{�{�b�N�X�����ŃJ�E���g���Ă��鍡�̂܂܂ł̓_���B�E�B���h�E�ԍ����Ǘ�����}�l�[�W�����K�v�ɂȂ�
	static int m_WindowNo;
	char* pszBuf;		//�R���{���X�g�ɓ��镶������ꎞ�I�ɓ���Ă����o�b�t�@
	bool m_DispFlag;	//�\�������ǂ����̃t���O
};