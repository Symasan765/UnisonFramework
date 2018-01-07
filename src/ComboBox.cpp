/*=================================================
//								FILE : ComboBox.cpp
//		ファイル説明 :
//		プルダウンメニューを取り扱うクラスを定義する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "ComboBox.h"

int cComboBox::m_WindowNo = 1;

cComboBox::cComboBox(HWND hwnd, LPARAM lp, int posX, int posY, int width, int height)
{
	m_Combo = CreateWindow(
		TEXT("COMBOBOX"), NULL,
		WS_CHILD | WS_VISIBLE | CBS_SORT | CBS_DROPDOWNLIST,
		posX, posY, width, height, hwnd, (HMENU)m_WindowNo,
		((LPCREATESTRUCT)(lp))->hInstance, NULL
	);
	pszBuf = new char[MAX_COMBOBOX_CHARA_LEN + 1];
	m_WindowNo++;	//ウィンドウ番号加算
	m_DispFlag = true;		//次のHide関数を有効にするためにtrueだがHide()でfalseに変えられる
	Hide();
}

cComboBox::~cComboBox()
{
	delete[] pszBuf;
	pszBuf = nullptr;
	DeleteAllItem();
	Hide();		//おやウィンドウが破棄されれば問題ないのでとりあえず非表示にだけしておく
}

void cComboBox::display()
{
		ShowWindow(m_Combo, SW_SHOWNORMAL);
		m_DispFlag = true;
}

void cComboBox::Hide()
{
	if (m_DispFlag == true) {
		ShowWindow(m_Combo, SW_HIDE);
		m_DispFlag = false;
	}
}

std::string cComboBox::GetSelectItem(HWND hwnd)
{
	//選択中のインデックス取得
	int index = SendMessage(m_Combo, CB_GETCURSEL, 0, 0);

	int intTxtLen = SendMessage(m_Combo, CB_GETLBTEXTLEN, index, 0);

	if (intTxtLen > MAX_COMBOBOX_CHARA_LEN)	MessageBox(hwnd, "コンボボックスに格納できる文字列を超えています", NULL, MB_OK);

	if (intTxtLen != CB_ERR)
	{
		if (SendMessage(m_Combo, CB_GETLBTEXT, index, (LPARAM)pszBuf) != CB_ERR)
		{
			return std::string(pszBuf);
		}
	}
	//ここまで来たらエラー
	MessageBox(hwnd, "コンボボックスにてエラーが確認されました", NULL, MB_OK);
	return std::string("err");
}

void cComboBox::DeleteAllItem()
{
	//項目がなくなるまで削除命令を発行する
	while (SendMessage(m_Combo, CB_GETCOUNT, 0, 0) != 0)
	{
		SendMessage(m_Combo, CB_DELETESTRING, 0, 0);
	}
}

void cComboBox::AddItem(std::string _ItemName)
{
	SendMessage(m_Combo, CB_ADDSTRING, 0, (LPARAM)TEXT(_ItemName.c_str()));
}

void cComboBox::SetForeground()
{
	//　TODO ボタンでプルダウンメニューを利用できるようにしたいが以下では活性化していない
	SetForegroundWindow(m_Combo);
	SetActiveWindow(m_Combo);
}