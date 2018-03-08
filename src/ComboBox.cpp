/*=================================================
//								FILE : ComboBox.cpp
//		�t�@�C������ :
//		�v���_�E�����j���[����舵���N���X���`����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "ComboBox.h"

int cComboBox::m_WindowNo = 1;

/// <summary>
/// �R���{�{�b�N�X�̏��������s��
/// </summary>
/// <param name="hwnd">�E�B���h�E�n���h��</param>
/// <param name="lp"></param>
/// <param name="posX">���WX</param>
/// <param name="posY">���WY</param>
/// <param name="width">����</param>
/// <param name="height">�c��</param>
cComboBox::cComboBox(HWND hwnd, LPARAM lp, int posX, int posY, int width, int height)
{
	m_Combo = CreateWindow(
		TEXT("COMBOBOX"), NULL,
		WS_CHILD | WS_VISIBLE | CBS_SORT | CBS_DROPDOWNLIST,
		posX, posY, width, height, hwnd, (HMENU)m_WindowNo,
		((LPCREATESTRUCT)(lp))->hInstance, NULL
	);
	pszBuf = new char[MAX_COMBOBOX_CHARA_LEN + 1];
	m_WindowNo++;	//�E�B���h�E�ԍ����Z
	m_DispFlag = true;		//����Hide�֐���L���ɂ��邽�߂�true����Hide()��false�ɕς�����
	Hide();
}

/// <summary>
/// �������
/// </summary>
cComboBox::~cComboBox()
{
	delete[] pszBuf;
	pszBuf = nullptr;
	DeleteAllItem();
	Hide();		//����E�B���h�E���j�������Ζ��Ȃ��̂łƂ肠������\���ɂ������Ă���
}

/// <summary>
/// �\��
/// </summary>
void cComboBox::display()
{
		ShowWindow(m_Combo, SW_SHOWNORMAL);
		m_DispFlag = true;
}

/// <summary>
/// ������
/// </summary>
void cComboBox::Hide()
{
	if (m_DispFlag == true) {
		ShowWindow(m_Combo, SW_HIDE);
		m_DispFlag = false;
	}
}

/// <summary>
/// �I�����ꂽ�A�C�e�������擾����
/// </summary>
/// <param name="hwnd"></param>
/// <returns>�I������N���b�N���ꂽ���ڂ̖��O��ԋp</returns>
std::string cComboBox::GetSelectItem(HWND hwnd)
{
	//�I�𒆂̃C���f�b�N�X�擾
	int index = SendMessage(m_Combo, CB_GETCURSEL, 0, 0);

	int intTxtLen = SendMessage(m_Combo, CB_GETLBTEXTLEN, index, 0);

	if (intTxtLen > MAX_COMBOBOX_CHARA_LEN)	MessageBox(hwnd, "�R���{�{�b�N�X�Ɋi�[�ł��镶����𒴂��Ă��܂�", NULL, MB_OK);

	if (intTxtLen != CB_ERR)
	{
		if (SendMessage(m_Combo, CB_GETLBTEXT, index, (LPARAM)pszBuf) != CB_ERR)
		{
			return std::string(pszBuf);
		}
	}
	//�����܂ŗ�����G���[
	MessageBox(hwnd, "�R���{�{�b�N�X�ɂăG���[���m�F����܂���", NULL, MB_OK);
	return std::string("err");
}

/// <summary>
/// ���ׂẴA�C�e�����폜����
/// </summary>
void cComboBox::DeleteAllItem()
{
	//���ڂ��Ȃ��Ȃ�܂ō폜���߂𔭍s����
	while (SendMessage(m_Combo, CB_GETCOUNT, 0, 0) != 0)
	{
		SendMessage(m_Combo, CB_DELETESTRING, 0, 0);
	}
}

/// <summary>
/// �V�����A�C�e�����R���{�{�b�N�X�ɒǉ�����
/// </summary>
/// <param name="_ItemName"></param>
void cComboBox::AddItem(std::string _ItemName)
{
	SendMessage(m_Combo, CB_ADDSTRING, 0, (LPARAM)TEXT(_ItemName.c_str()));
}

void cComboBox::SetForeground()
{
	//�@TODO �{�^���Ńv���_�E�����j���[�𗘗p�ł���悤�ɂ��������ȉ��ł͊��������Ă��Ȃ�
	SetForegroundWindow(m_Combo);
	SetActiveWindow(m_Combo);
}