/*=================================================
//								FILE : WindProc.h
//		ファイル説明 :
//		ウィンドウプロシージャの内容を記述する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include <Windows.h>
#include "DirectX11.h"
#include "ComboBox.h"
#include "LuaManager.h"

/*-------------------------------------------
ウィンドウ処理
--------------------------------------------*/
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	HRESULT hr = S_OK;

	switch (msg)
	{
	case WM_CREATE:
		//ウィンドウ生成時のみコンボボックスの作成
#if defined(DEBUG) || defined(_DEBUG)
		if (cDirectX11::GetInstance()->m_pComboBox == nullptr && lParam != 0) {
			cDirectX11::GetInstance()->m_pComboBox = new cComboBox(hWnd, lParam);
		}
#endif
		return 0;
	case WM_DESTROY:
		// Direct3Dの終了処理
		cDirectX11::m_pDX11->CleanupDirect3D();
		// ウインドウを閉じる
		PostQuitMessage(0);
		cDirectX11::m_pDX11->m_hWindow = nullptr;
		return 0;
#if defined(DEBUG) || defined(_DEBUG)
	case WM_COMMAND: {
		// スクリプトのリロードを担当
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			std::string SelectItemName = cDirectX11::GetInstance()->m_pComboBox->GetSelectItem(hWnd);
			cLuaManager::GetInstance().ReloadTheScript(SelectItemName);
			//一度非表示にしてから表示することで選択を解除する
			cDirectX11::GetInstance()->m_pComboBox->Hide();
			cDirectX11::GetInstance()->m_pComboBox->display();
			SetForegroundWindow(hWnd);
		}
		//キャンセル処理
		if(HIWORD(wParam) == CBN_SELENDCANCEL)
			SetForegroundWindow(hWnd);
		return 0;
	}
#endif
	case WM_KEYDOWN:
		// キー入力の処理
		switch (wParam)
		{
		case VK_ESCAPE:	// [ESC]キーでウインドウを閉じる
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;


		case VK_F5:			//ゲーム中のデバッグ機能を有効にする======================
#if defined(DEBUG) || defined(_DEBUG)
			//デバッグモードの時にF5が押された場合のみデバッグ機能を有効にする
			cDirectX11::GetInstance()->m_DebugFlag = !cDirectX11::GetInstance()->m_DebugFlag;		//フラグを反転

			//切り替えの瞬間は必ずどのモードも初期からスタートさせる
			cDirectX11::GetInstance()->m_PauseFlag = false;
			cDirectX11::GetInstance()->m_FreeCameraFlag = false;
			cDirectX11::GetInstance()->m_GBufferDrawFlag = false;
			if (cDirectX11::GetInstance()->m_DebugFlag) {
				cLuaManager::GetInstance().SetTheNameInTheComboBox(cDirectX11::GetInstance()->m_pComboBox);
			}
#endif
			break;


		case VK_F6:
#if defined(DEBUG) || defined(_DEBUG)
			//デバッグモードの時にF6が押された場合デバッグ中ならG-Bufferを表示させる
			if (cDirectX11::GetInstance()->m_DebugFlag)
				cDirectX11::GetInstance()->m_GBufferDrawFlag = !cDirectX11::GetInstance()->m_GBufferDrawFlag;		//フラグを反転
#endif
			break;

		case VK_F4:			//一時停止機能を有効にする==============================
#if defined(DEBUG) || defined(_DEBUG)
			//デバッグモード中でF4が押されたら一時停止にする。再度押されたら解除する
			if (cDirectX11::GetInstance()->m_DebugFlag) {
				cDirectX11::GetInstance()->m_PauseFlag = !cDirectX11::GetInstance()->m_PauseFlag;		//フラグ反転
				cDirectX11::GetInstance()->m_FreeCameraFlag = cDirectX11::GetInstance()->m_PauseFlag;	//フリーカメラも共通で
			}
#endif
			break;
		case VK_F1:
#if defined(DEBUG) || defined(_DEBUG)
			//コンボボックスをキー操作出来るようにしたかったがまだ有効ではない↓
			if (cDirectX11::GetInstance()->m_DebugFlag) {
				cDirectX11::GetInstance()->m_pComboBox->SetForeground();
			}
#endif
			break;	//ここまで2017/11/17 現在 無効↑
		}
		break;
	}

	// デフォルト処理
	return DefWindowProc(hWnd, msg, wParam, lParam);
}