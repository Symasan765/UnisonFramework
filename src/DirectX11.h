/*=================================================
//								FILE : DirectX11.h
//		�t�@�C������ :
//		DIrectX11���s����������f�o�C�X�̕ێ��Ȃǂ�
//		�s���N���X���`���A�L�q����B
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once

#include "WinMain.h"
#include "TimeCheck.h"
#include "ProcessMonitor.h"

// TODO 2017/11/17���� DirectX11�I�u�W�F�N�g�̃����[�X���������Ȃ��B�K���_�����邱��

class cComboBox;

/// <summary>
/// DirectX11�̋@�\������B
/// ���̂ŋ����͔������Ȃ��Ǝv���̂ł킩��₷�����̂܂ܖ����B
/// �V���O���g���ō쐬���邪��قǂ̂��Ƃ��Ȃ���΃A�N�Z�X���Ȃ��悤�Ȑ݌v�ɂ���\��
/// </summary>
class cDirectX11 {
public:
	//WinMain�ƃE�B���h�E�v���V�[�W���ł̂݃v���C�x�[�g�ɃA�N�Z�X���A�������Ȃǂ��s�����Ƃő�����Ăяo����Ă���肪�Ȃ��悤�ɂ���
	friend int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	friend LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);
	cDirectX11(const cDirectX11&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cDirectX11& operator=(const cDirectX11&) = delete;	//�R�s�[������Z�q�� delete �w��
	cDirectX11(cDirectX11&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cDirectX11& operator=(cDirectX11&&) = delete;		//���[�u������Z�q�� delete �w��

	static cDirectX11* GetInstance() {
		return m_pDX11;
	}

	ID3D11Device* Device() { return m_pD3DDevice; };
	ID3D11DeviceContext* Context() { return m_pImmediateContext; };
	HWND GetHwnd() { return m_hWindow; };
	HINSTANCE	HInstance() {return m_hInstance;};
	D3D11_VIEWPORT GetViewPort() { return m_ViewPort; };
private:
	cDirectX11();									//�R���X�g���N�^�� private �ɒu��
	~cDirectX11() = default;								//�f�X�g���N�^�� private �ɒu��
	static void Init();											//����ō쐬����B�t�����h�֐���WinMain���炵���ĂׂȂ�
	void Update();												//�Q�[�����[�v�̍X�V���J�n����
	HRESULT Create(HINSTANCE _hInst);				//���ۂɃV�X�e���̍쐬���s��
	HRESULT InitWindow(HINSTANCE _hInst);			//�E�B���h�E������
	bool InitApp();												//�Q�[�����ŗ��p����A�v���P�[�V�����̏��������s��
	void InitSceneManager();
	HRESULT InitDirect3D();									//Direct3D�̏��������s��
	HRESULT InitBackBuffer();								//�o�b�N�o�b�t�@�̏�����
	bool CleanupDirect3D();
	void CleanupApp();
	bool CheckSystem();
	void DestroyApp();
	int Destroy();

	//�Q�[���̍X�V�ɕK�v�Ȋ֐��Q
	void MainGameUpdate();
	bool GameUpdateFlag()const;
	void GameProcessingTime();		//�v�������������Ԃ��܂Ƃ߂�

	//==========================�Ǘ�����I�u�W�F�N�g===============================
	static cDirectX11* m_pDX11;									//���Ԃ��i�[
	ID3D11Device* m_pD3DDevice;									//�f�o�C�X�n���h��
	HWND m_hWindow;														//�E�B���h�E�n���h��
	HINSTANCE	m_hInstance;
	ID3D11DepthStencilState* m_pDepthStencilState;
	ID3D11DeviceContext* m_pImmediateContext;			// �f�o�C�X�E�R���e�L�X�g
	IDXGISwapChain* m_pSwapChain;								 // �X���b�v�E�`�F�C��
	ID3D11RenderTargetView* m_pRenderTargetView;	//�^�[�Q�b�g�r���[
	ID3D11Texture2D* m_pDepthStencil;						//�[�x�E�X�e���V��
	ID3D11DepthStencilView* m_pDepthStencilView;		//�[�x�X�e���V���r���[
	D3D11_VIEWPORT m_ViewPort;									//�r���[�|�[�g
	ID3D11RasterizerState* m_pCullNoneRasterizerState;		//���X�^���C�U
	ID3D11RasterizerState* m_pCullLeftRasterizerState;		//����n���W�p
	ID3D11RasterizerState* m_pCullRightRasterizerState;		//�E��n���W�p
	ID3D11RasterizerState* m_pWIREFRAMERasterizerState;
	ID3D11BlendState* m_pBlendState;
	ID3D11BlendState* v_pAlphaState;							//�A���t�@�u�����h�X�e�[�g
	ID3D11BlendState* v_pAddState;							//���Z�����X�e�[�g
	SIZE		m_sizeWindow;											// �E�B���h�E�T�C�Y

	//====�������Ԍv��====
	cTimeCheck m_TimeAllProcess;		//�t���[���S�̂̏������Ԃ��v��
	cProcessMonitor m_ProcessMonitor;	//CPU�g�p������
	//=================

	//===============�f�o�b�O�@�\�ŗ��p����t���O����================

	bool m_DebugFlag;					//�f�o�b�O�����̑匳�̃t���O
	bool m_PauseFlag;					//�|�[�Y�@�\��L���ɂ��邩�ǂ����̃t���O
	bool m_FreeCameraFlag;		//�J���������R�ɓ��������ǂ����̃t���O
	bool m_GBufferDrawFlag;		//�f�o�b�O����G-Buffer��\�����邩�ǂ����H
	cComboBox* m_pComboBox;

	//=======================================================
};