/*=================================================
//								FILE :
//		ファイル説明 : WinMain.cpp
//		WinMainを記述し、処理のスタートプログラムや
//		初期化処理を行う
//							HAL大阪 : 松本 雄之介
=================================================*/
// 必要なライブラリをリンクする
#include "WinMain.h"
#include "DirectX11.h"

#pragma comment( lib, "d3d11.lib" )
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx11d.lib" )
#else
#pragma comment( lib, "d3dx11.lib" )
#endif
#pragma comment( lib, "dxerr.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment(lib,"xinput.lib")
#pragma comment(lib,"libfbxsdk-mt.lib")
#pragma comment(lib,"DirectXTK.lib")
#pragma comment( lib, "legacy_stdio_definitions.lib" )
#pragma comment(lib, "pdh.lib")
#pragma comment(lib,"lua53.dll")
#pragma comment(lib,"liblua53.a")

/// <summary>
/// WinMainは内部構造を1.初期化 2.システムチェック 3.アップデート 4.解放の流れで構成する
/// </summary>
/// <param name="hInst"></param>
/// <param name="hPrevInstance"></param>
/// <param name="lpCmdLine"></param>
/// <param name="nCmdShow"></param>
/// <returns></returns>
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//まずはシステムのもととなるcDirectX11を作成する
	cDirectX11::Init();
	if(FAILED(cDirectX11::GetInstance()->Create(hInst))) return 0;
	if(!cDirectX11::GetInstance()->InitApp()) return 0;

	//利用するシステムにハードウェアが対応しているかチェックする
	if (!cDirectX11::GetInstance()->CheckSystem()) return 0;

	//メインループ処理
	cDirectX11::GetInstance()->Update();

	//以降解放処理。システムで不要なものは必ず解放すること！
	cDirectX11::GetInstance()->DestroyApp();
	return cDirectX11::GetInstance()->Destroy();		//根幹のシステムを最後に解放
}