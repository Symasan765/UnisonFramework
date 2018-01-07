/*=================================================
//								FILE :
//		ファイル説明 : WinMain.h
//		Windows,DirectXを利用する際にまとめて使用するヘッダ
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once

#include <windows.h>
#include <mmsystem.h>
#include <crtdbg.h>
#include <d3dx11.h>
#include <DirectXMath.h>
#include <dxerr.h>

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }	// 解放マクロ

//画面解像度
#define WINDOW_SIZE_X (1280)
#define WINDOW_SIZE_Y (720)

//フレームレート
#define FRAME_RATE (60)

namespace GetDirectX{
	ID3D11Device* Device();					//デバイスを取得する
	ID3D11DeviceContext* Context();		//コンテキストを取得する
	HWND Hwnd();
	HINSTANCE	HInstance();
	void InitOMSetRenderTargets();
}