/*=================================================
//								FILE :
//		�t�@�C������ : WinMain.cpp
//		WinMain���L�q���A�����̃X�^�[�g�v���O������
//		�������������s��
//							HAL��� : ���{ �Y�V��
=================================================*/
// �K�v�ȃ��C�u�����������N����
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
/// WinMain�͓����\����1.������ 2.�V�X�e���`�F�b�N 3.�A�b�v�f�[�g 4.����̗���ō\������
/// </summary>
/// <param name="hInst"></param>
/// <param name="hPrevInstance"></param>
/// <param name="lpCmdLine"></param>
/// <param name="nCmdShow"></param>
/// <returns></returns>
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//�܂��̓V�X�e���̂��ƂƂȂ�cDirectX11���쐬����
	cDirectX11::Init();
	if(FAILED(cDirectX11::GetInstance()->Create(hInst))) return 0;
	if(!cDirectX11::GetInstance()->InitApp()) return 0;

	//���p����V�X�e���Ƀn�[�h�E�F�A���Ή����Ă��邩�`�F�b�N����
	if (!cDirectX11::GetInstance()->CheckSystem()) return 0;

	//���C�����[�v����
	cDirectX11::GetInstance()->Update();

	//�ȍ~��������B�V�X�e���ŕs�v�Ȃ��͕̂K��������邱�ƁI
	cDirectX11::GetInstance()->DestroyApp();
	return cDirectX11::GetInstance()->Destroy();		//�����̃V�X�e�����Ō�ɉ��
}