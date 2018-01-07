/*=================================================
//								FILE :
//		�t�@�C������ : WinMain.h
//		Windows,DirectX�𗘗p����ۂɂ܂Ƃ߂Ďg�p����w�b�_
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once

#include <windows.h>
#include <mmsystem.h>
#include <crtdbg.h>
#include <d3dx11.h>
#include <DirectXMath.h>
#include <dxerr.h>

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }	// ����}�N��

//��ʉ𑜓x
#define WINDOW_SIZE_X (1280)
#define WINDOW_SIZE_Y (720)

//�t���[�����[�g
#define FRAME_RATE (60)

namespace GetDirectX{
	ID3D11Device* Device();					//�f�o�C�X���擾����
	ID3D11DeviceContext* Context();		//�R���e�L�X�g���擾����
	HWND Hwnd();
	HINSTANCE	HInstance();
	void InitOMSetRenderTargets();
}