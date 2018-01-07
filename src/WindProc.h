/*=================================================
//								FILE : WindProc.h
//		�t�@�C������ :
//		�E�B���h�E�v���V�[�W���̓��e���L�q����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include <Windows.h>
#include "DirectX11.h"
#include "ComboBox.h"
#include "LuaManager.h"

/*-------------------------------------------
�E�B���h�E����
--------------------------------------------*/
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	HRESULT hr = S_OK;

	switch (msg)
	{
	case WM_CREATE:
		//�E�B���h�E�������̂݃R���{�{�b�N�X�̍쐬
#if defined(DEBUG) || defined(_DEBUG)
		if (cDirectX11::GetInstance()->m_pComboBox == nullptr && lParam != 0) {
			cDirectX11::GetInstance()->m_pComboBox = new cComboBox(hWnd, lParam);
		}
#endif
		return 0;
	case WM_DESTROY:
		// Direct3D�̏I������
		cDirectX11::m_pDX11->CleanupDirect3D();
		// �E�C���h�E�����
		PostQuitMessage(0);
		cDirectX11::m_pDX11->m_hWindow = nullptr;
		return 0;
#if defined(DEBUG) || defined(_DEBUG)
	case WM_COMMAND: {
		// �X�N���v�g�̃����[�h��S��
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			std::string SelectItemName = cDirectX11::GetInstance()->m_pComboBox->GetSelectItem(hWnd);
			cLuaManager::GetInstance().ReloadTheScript(SelectItemName);
			//��x��\���ɂ��Ă���\�����邱�ƂőI������������
			cDirectX11::GetInstance()->m_pComboBox->Hide();
			cDirectX11::GetInstance()->m_pComboBox->display();
			SetForegroundWindow(hWnd);
		}
		//�L�����Z������
		if(HIWORD(wParam) == CBN_SELENDCANCEL)
			SetForegroundWindow(hWnd);
		return 0;
	}
#endif
	case WM_KEYDOWN:
		// �L�[���͂̏���
		switch (wParam)
		{
		case VK_ESCAPE:	// [ESC]�L�[�ŃE�C���h�E�����
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;


		case VK_F5:			//�Q�[�����̃f�o�b�O�@�\��L���ɂ���======================
#if defined(DEBUG) || defined(_DEBUG)
			//�f�o�b�O���[�h�̎���F5�������ꂽ�ꍇ�̂݃f�o�b�O�@�\��L���ɂ���
			cDirectX11::GetInstance()->m_DebugFlag = !cDirectX11::GetInstance()->m_DebugFlag;		//�t���O�𔽓]

			//�؂�ւ��̏u�Ԃ͕K���ǂ̃��[�h����������X�^�[�g������
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
			//�f�o�b�O���[�h�̎���F6�������ꂽ�ꍇ�f�o�b�O���Ȃ�G-Buffer��\��������
			if (cDirectX11::GetInstance()->m_DebugFlag)
				cDirectX11::GetInstance()->m_GBufferDrawFlag = !cDirectX11::GetInstance()->m_GBufferDrawFlag;		//�t���O�𔽓]
#endif
			break;

		case VK_F4:			//�ꎞ��~�@�\��L���ɂ���==============================
#if defined(DEBUG) || defined(_DEBUG)
			//�f�o�b�O���[�h����F4�������ꂽ��ꎞ��~�ɂ���B�ēx�����ꂽ���������
			if (cDirectX11::GetInstance()->m_DebugFlag) {
				cDirectX11::GetInstance()->m_PauseFlag = !cDirectX11::GetInstance()->m_PauseFlag;		//�t���O���]
				cDirectX11::GetInstance()->m_FreeCameraFlag = cDirectX11::GetInstance()->m_PauseFlag;	//�t���[�J���������ʂ�
			}
#endif
			break;
		case VK_F1:
#if defined(DEBUG) || defined(_DEBUG)
			//�R���{�{�b�N�X���L�[����o����悤�ɂ������������܂��L���ł͂Ȃ���
			if (cDirectX11::GetInstance()->m_DebugFlag) {
				cDirectX11::GetInstance()->m_pComboBox->SetForeground();
			}
#endif
			break;	//�����܂�2017/11/17 ���� ������
		}
		break;
	}

	// �f�t�H���g����
	return DefWindowProc(hWnd, msg, wParam, lParam);
}