/*=================================================
//								FILE : ComboBox.h
//		ファイル説明 :
//		プルダウンメニューを取り扱うクラスを定義する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include <Windows.h>
#include <string>

#define MAX_COMBOBOX_CHARA_LEN 30		//リストの１項目の最大文字数

class cComboBox {
public:
	cComboBox(HWND hwnd, LPARAM lp,int posX = 975,int posY = 0,int width = 300,int height = 300);
	~cComboBox();
	void display();		//表示
	void Hide();			//非表示
	std::string GetSelectItem(HWND hwnd);	//選ばれたアイテムを返す
	void DeleteAllItem();	//保持する項目を破棄する
	void AddItem(std::string _ItemName);
	void SetForeground();
private:
	HWND m_Combo;
	// TODO もし他のウィンドウを作る場合、コンボボックスだけでカウントしている今のままではダメ。ウィンドウ番号を管理するマネージャが必要になる
	static int m_WindowNo;
	char* pszBuf;		//コンボリストに入る文字列を一時的に入れておくバッファ
	bool m_DispFlag;	//表示中かどうかのフラグ
};