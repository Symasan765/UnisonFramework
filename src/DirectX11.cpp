/*=================================================
//								FILE : DirectX11.cpp
//		�t�@�C������ :
//		�V�X�e���̏�������ݒ�A�J���̋L�q���s���B
//		WinMain�̉B��������ȖړI�Ƃ���B
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "FontDraw.h"
#include "DirectX11.h"
#include "WindProc.h"
#include "SceneManager.h"
#include "MainScene.h"
#include "TextureManager.h"
#include "Input.h"
#include "Mesh3D.h"
#include "SpriteDraw.h"
#include "MemoryMonitor.h"
#include "ComboBox.h"
#include "Tessellation.h"
#include "ShaderMonitoring.h"

#include "PostEffects.h"

cDirectX11* cDirectX11::m_pDX11 = nullptr;

//���̃t�@�C���ł͎g�p���Ȃ�
namespace {
	MSG msg;				//���b�Z�[�W�\����
#if defined(DEBUG) || defined(_DEBUG)
	CHAR		g_szAppTitle[] = "UnisonFramework(F5:Debug)";
#else
	CHAR		g_szAppTitle[] = "UnisonFramework";
#endif
	CHAR		g_szWndClass[] = "UnisonFramework";

	D3D_FEATURE_LEVEL g_pFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
	UINT              g_FeatureLevels = 3;   // �z��̗v�f��
	D3D_FEATURE_LEVEL g_FeatureLevelsSupported; // �f�o�C�X�쐬���ɕԂ����@�\���x��
}

/// <summary>
/// �����ŃV�[���}�l�[�W���ɂ��Q�[���̍X�V���s��
/// </summary>
void cDirectX11::MainGameUpdate()
{
	//==========================�V�X�e���̍X�V����====================================

	//���\�[�X��������ꂽ���̃`�F�b�N�����ĕK�v�Ȃ���Ή���������s���B
	cTexManager::GetInstance().CheckDelete();
	MeshManager::GetInstance().CheckDelete();
	SHADER::GetInstance()->CheckDelete();
	cTessellationManager::GetInstance().CheckDelete();

	//���͂̍X�V��������
	GetKey->UpdateKeyboard();
	GetGamePad->UpdateGamePad();

	cSprite2DDraw::GetInstance().m_NowDepth = 0.99999f;		//�X�v���C�g�`����Ŕw�ʂɐݒ肷��
#if defined(DEBUG) || defined(_DEBUG)
	cShaderMonitoring::GetInstance().MonitorChanges();		//�V�F�[�_�t�@�C���̍X�V�����ꂽ���ǂ����𔻒肷��
	cMemoryMonitor::GetInstance().Update();						//�f�o�b�O���̓������[���p�󋵂��X�V����
#endif

	//============================================================================
	//==========================�Q�[���̍X�V����======================================

#if defined(DEBUG) || defined(_DEBUG)
	m_TimeAllProcess.TimerStart();			//�f�o�b�O���[�h���͏������Ԃ��v������
#endif

	//�A�b�v�f�[�g�o���邩�`�F�b�N����
	if (GameUpdateFlag())
		cSceneManager::GetInstance().Update();
	//�t���[�J��������t���O�������Ă���΃t���[�J�����̃A�b�v�f�[�g���s��
	if (m_FreeCameraFlag)
		cSceneManager::GetInstance().FreeCameraUpdate();
	cSceneManager::GetInstance().Draw(m_DebugFlag, m_FreeCameraFlag,m_GBufferDrawFlag);
	cSceneManager::GetInstance().PopCheck();							//�Ō�Ƀ��[�v���ɑJ�ڂ�POP�������������m�F���Ď��s����

#if defined(DEBUG) || defined(_DEBUG)
	m_TimeAllProcess.TimerEnd();			//���Ԍv���I��
	GameProcessingTime();						//�v���������Ԃ��܂Ƃ߂�
#endif

	//=============================================================================
}

/// <summary>
/// �Q�[���̍X�V�������s���Ă��������`�F�b�N����֐�
/// </summary>
/// <returns></returns>
bool cDirectX11::GameUpdateFlag() const
{
#if defined(DEBUG) || defined(_DEBUG)
	//�f�o�b�O��
	if (!m_DebugFlag) return true;		//�f�o�b�O���ł͂Ȃ��Ȃ炻�̂܂܎��s���Ă���
	if (!m_PauseFlag) return true;		//�|�[�Y���ĂȂ��Ȃ炻�̂܂܎��s
	if (GetKey->Trigger(DIK_F3)) return true;		//�|�[�Y���ł�F3�������ꂽ�u�Ԃ͎��s���Ă���

	return false;
#else
	//�����[�X��
	return true;		//�����[�X���[�h�ł̓Q�[�����~�߂邱�Ƃ��Ȃ�
#endif
}

/// <summary>
/// �Q�[�����[�v�ő��肵���v�����Ԃ��܂Ƃ߂�
/// </summary>
void cDirectX11::GameProcessingTime()
{
	//�f�o�b�O���[�h���������p���Ȃ�
#if defined(DEBUG) || defined(_DEBUG)
	//�ϐ��̐錾
	static unsigned TimeCnt;	//�t���[���̃J�E���g
	static float AllTime;		//1�t���[���ɂ�����������
	static float UpdateTime;	//�X�V�ɂ�����������
	static float DrawTime;			//�`��ɂ�����������
	static float Estimated;		//1�t���[���ɂ�����z�莞��
	static int UpdateRate;		//�X�V�ɂ��������䗦(�p�[�Z���g)
	static int DrawRate;				//�`��ɂ��������䗦(�p�[�Z���g)
	static int AllRate;				//�z�肷��t���[�����[�g�ɑ΂��Ă����������ԗ�(�p�[�Z���g)
	static int ProcTime;	//�P�ʂ��~���ɕϊ�
	static CameraData Camera;	//�J�������
	static int ProcessRate[MAX_PROCESSORS_COUNT];
	static float AllMemory;
	static float UseMemory;
	//�܂��͌��݃f�o�b�O���[�h�ɂȂ��Ă��邩�`�F�b�N����
	if (m_DebugFlag) {
		m_pComboBox->display();
		//1�t���[������4�񏈗�����
		if (TimeCnt % (FRAME_RATE / 4) == 0) {
			AllTime = m_TimeAllProcess.GetProcessingTime() * 1000.0f;		//1�t���[���ɂ�����������
			UpdateTime = cSceneManager::GetInstance().m_TimeUpdate.GetProcessingTime() * 1000.0f;	//�X�V�ɂ�����������
			DrawTime = cSceneManager::GetInstance().m_TimeDraw.GetProcessingTime() * 1000.0f;			//�`��ɂ�����������
			Estimated = 1.0f / (float)FRAME_RATE;		//1�t���[���ɂ�����z�莞��

			UpdateRate = (int)(UpdateTime / AllTime * 100.0f);		//�X�V�ɂ��������䗦(�p�[�Z���g)
			DrawRate = (int)(DrawTime / AllTime * 100.0f);				//�`��ɂ��������䗦(�p�[�Z���g)
			AllRate = (int)(AllTime / Estimated * 100.0f);				//�z�肷��t���[�����[�g�ɑ΂��Ă����������ԗ�(�p�[�Z���g)

			ProcTime = 1000.0f / (AllTime);	//�P�ʂ��t���[�����[�g�ɕϊ�

			Camera = cSceneManager::GetInstance().m_pSceneData[0]->m_CameraData.GetCameraData(m_PauseFlag);		//�J�����̏����擾����
			int ProcessNum = m_ProcessMonitor.GetProcessorsNum();	//�v���Z�b�T��
			for (int i = 0; i < ProcessNum + 1; i++) {
				ProcessRate[i] = m_ProcessMonitor.GetProcessRate(i);	//CPU�g�p�����擾
				cSceneManager::GetInstance().SetProcessRate(i, ProcessRate[i]);
			}
			//�������̎g�p�ʂ��擾����
			MemoryData Memory = cMemoryMonitor::GetInstance().GetMemoryData();
			int _AllMemory = Memory.vSize / 1024.0f / 1024.0f;	//�e�ʂ�MB�ɕϊ�
			int _UseMemory = Memory.vUsed / 1024.0f / 1024.0f;
			AllMemory = (float)_AllMemory / 1024.0f;				//�P�ʂ�GB�ɕϊ�
			UseMemory = (float)_UseMemory / 1024.0f;
		}

			//�f�o�b�O���[�h�����ʂŕ\������G���A
		cFontDraw::getInstance()->TextDraw("�f�o�b�O���[�h��", 10, 10, FontName::MSGothic, 1.5f);
		std::string ProcTimeChara = "�S�������� : " + std::to_string(AllTime) + "ms(" + std::to_string(ProcTime) + "fps)";
		std::string ProcUpdateChara = "�E�X�V���� : " + std::to_string(UpdateTime) + "ms(" + std::to_string(UpdateRate) + "%)";
		std::string ProcDrawChara = "�E�`�掞�� : " + std::to_string(DrawTime) + "ms(" + std::to_string(DrawRate) + "%)";
		//��L�̏������Ԃ�\��������
		cFontDraw::getInstance()->TextDraw(ProcTimeChara, 15, 55,FontName::Mplus,0.6f);
		cFontDraw::getInstance()->TextDraw(ProcUpdateChara, 15, 75, FontName::Mplus, 0.6f);
		cFontDraw::getInstance()->TextDraw(ProcDrawChara, 15, 95, FontName::Mplus, 0.6f);

		//�J���������܂Ƃ߂�
		std::string CameraPos = "�E���W : X" + std::to_string(Camera.vPos.x) + " Y" + std::to_string(Camera.vPos.y) + " Z" + std::to_string(Camera.vPos.z);
		std::string CameravLook = "�E���� : X" + std::to_string(Camera.vLook.x) + " Y" + std::to_string(Camera.vLook.y) + " Z" + std::to_string(Camera.vLook.z);
		std::string CameraNearFar = "�E�͈� : " + std::to_string(Camera.vNear) + " �` " + std::to_string(Camera.vFar);
		//�J�����̏���\������
		cFontDraw::getInstance()->TextDraw("�J�������", 15, 125, FontName::Mplus, 0.6f);
		cFontDraw::getInstance()->TextDraw(CameraPos, 15, 145, FontName::Mplus, 0.4f);
		cFontDraw::getInstance()->TextDraw(CameravLook, 15, 160, FontName::Mplus, 0.4f);
		cFontDraw::getInstance()->TextDraw(CameraNearFar, 15, 175, FontName::Mplus, 0.4f);
		//CPU�g�p����\������
		int ProcessNum = m_ProcessMonitor.GetProcessorsNum();	//�v���Z�b�T��
		std::string ProcessChara = "�E�v���Z�b�T�� : " + std::to_string(ProcessNum);
		cFontDraw::getInstance()->TextDraw("CPU�g�p��", 15, 205, FontName::Mplus, 0.6f);
		cFontDraw::getInstance()->TextDraw(ProcessChara, 15, 225, FontName::Mplus, 0.4f);
		std::string ProcessRateChara;
		for (int i = 0; i < ProcessNum + 1; i++) {
			if (i == ProcessNum) {
				ProcessRateChara = "AVL : ";
			}
			else {
				ProcessRateChara = "No" + std::to_string(i) + " : ";
			}
			ProcessRateChara += std::to_string(ProcessRate[i]) + "%";
			cFontDraw::getInstance()->TextDraw(ProcessRateChara, 15, 245 + (i * 15), FontName::Mplus, 0.4f);
		}
		//�������g�p��
		int InitPos = (ProcessNum) * 15;
		std::string AllMemoryChara =std::to_string(UseMemory) + "GB/" + std::to_string(AllMemory) + "GB";
		cFontDraw::getInstance()->TextDraw("�������g�p��", 15, 245 + (InitPos) + 20, FontName::Mplus, 0.6f);
		cFontDraw::getInstance()->TextDraw(AllMemoryChara, 25, 265 + (InitPos)+20, FontName::Mplus, 0.4f);

		//�I���\�����o��
		cFontDraw::getInstance()->TextDraw("F5:�f�o�b�O���[�hOFF", 940, 700);
		cFontDraw::getInstance()->TextDraw("F4:�|�[�Y�@�\ON�EOFF", 940, 660);

		//G-Buffer�\������UI
		if (!m_GBufferDrawFlag) {
			cFontDraw::getInstance()->TextDraw("F6:G-Buffer�\��", 10, 710, FontName::MSGothic, 0.7f);
		}

		//�|�[�Y���[�h���݂̂ɕ\������G���A
		if (m_PauseFlag) {
			cFontDraw::getInstance()->TextDraw("(�|�[�Y���[�h)", 400, 10, FontName::MSGothic, 1.5f);
			cFontDraw::getInstance()->TextDraw("�t���[�J��������", 1000, 10);
			cFontDraw::getInstance()->TextDraw("WASD : �ړ�", 1115, 40, FontName::MSGothic, 0.8f);
			cFontDraw::getInstance()->TextDraw("�\�� : ��]", 1115, 65, FontName::MSGothic, 0.8f);
			cFontDraw::getInstance()->TextDraw("QZEC : �͈�", 1115, 90, FontName::MSGothic, 0.8f);
			cFontDraw::getInstance()->TextDraw("ENTER: �߂�", 1115, 115, FontName::MSGothic, 0.8f);
			cFontDraw::getInstance()->TextDraw("F3:�R�}����", 1115, 140, FontName::MSGothic, 0.8f);
		}
		TimeCnt++;
		//�J�E���^�[������������
		if (TimeCnt == FRAME_RATE)
			TimeCnt = 0;
	}
	else {
		m_pComboBox->Hide();	//�X�N���v�g���R���{�{�b�N�X��\��
	}
#endif
}

/// <summary>
/// �����������s��
/// </summary>
cDirectX11::cDirectX11() {
	m_sizeWindow = { WINDOW_SIZE_X, WINDOW_SIZE_Y };		//�E�B���h�E�T�C�Y
	m_hWindow = nullptr;
	m_hInstance = nullptr;
	m_pD3DDevice = nullptr;
	m_pDepthStencilState = nullptr;
	m_pImmediateContext = nullptr;
	m_pSwapChain = nullptr;
	m_pRenderTargetView = nullptr;
	m_pDepthStencilView = nullptr;
	m_pCullNoneRasterizerState = nullptr;
	m_pCullLeftRasterizerState = nullptr;
	m_pCullRightRasterizerState = nullptr;
	m_pBlendState = nullptr;
	v_pAlphaState = nullptr;

	//�f�o�b�O�t���O����
	m_DebugFlag = false;
	m_PauseFlag = false;
	m_FreeCameraFlag = false;
	m_GBufferDrawFlag = false;
	m_pComboBox = nullptr;
}

/// <summary>
/// �V���O���g���̃|�C���^�Ɏ��Ԃ��쐬���A�V�X�e���̊e�평�������s���Ă���
/// </summary>
void cDirectX11::Init()
{
	if (!m_pDX11) {
		// �f�o�b�O �q�[�v �}�l�[�W���ɂ�郁�������蓖�Ă̒ǐՕ��@��ݒ�
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		m_pDX11 = new cDirectX11;
	}
}

/// <summary>
/// �������Q�[�����[�v���i�[����Ă���֐�
/// </summary>
void cDirectX11::Update()
{
	do
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//�����Ƀ��C������
			MainGameUpdate();
		}
	} while (msg.message != WM_QUIT);		//�Ȃɂ�����΂��̂܂܏����𔲂���
}

/// <summary>
/// �E�B���h�E�̍쐬�ADirectX�̏��������s��
/// </summary>
/// <param name="_hInst">WinMain�ŗ��p���ꂽHINSTANCE�����̂܂ܓn��</param>
/// <returns></returns>
HRESULT cDirectX11::Create(HINSTANCE _hInst)
{
	//�܂��̓E�B���h�E�̐�������s��
	if (FAILED(InitWindow(_hInst))) {
		_ASSERT(true, "WinMain InitWindow() err");
		return E_FAIL;
	}

	//����DirectX������
	if (FAILED(InitDirect3D())) {
		_ASSERT(true, "WinMain InitDirect3D() err");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT cDirectX11::InitWindow(HINSTANCE _hInst)
{
	// �A�v���P�[�V�����̃C���X�^���X �n���h����ۑ�
	m_hInstance = _hInst;

	// �E�C���h�E �N���X�̓o�^
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hInst;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szWndClass;

	if (!RegisterClass(&wc))
		return DXTRACE_ERR("InitApp", GetLastError());

	// ���C�� �E�C���h�E�쐬
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = m_sizeWindow.cx;
	rect.bottom = m_sizeWindow.cy;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

	m_hWindow = CreateWindow(g_szWndClass, g_szAppTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, _hInst, NULL);
	if (m_hWindow == NULL)
		return DXTRACE_ERR("InitApp", GetLastError());

	// �E�C���h�E�\��
	ShowWindow(m_hWindow, SW_SHOWNORMAL);
	UpdateWindow(m_hWindow);

	return S_OK;
}

/// <summary>
/// �Q�[���ŗ��p����@�\�̏��������s��
/// </summary>
/// <returns></returns>
bool cDirectX11::InitApp()
{
	//���p����@�\��������΋L�q����

	//�V�[���}�l�[�W���̏�����
	InitSceneManager();

	//�V�[���Ƀf�o�C�X����n���Ă���
	DirectXData data{
	m_pD3DDevice,									//�f�o�C�X�n���h��
	m_pDepthStencilState,
	m_pImmediateContext,			// �f�o�C�X�E�R���e�L�X�g
	m_pSwapChain,								 // �X���b�v�E�`�F�C��
	m_pRenderTargetView,	//�^�[�Q�b�g�r���[
	m_pDepthStencil,						//�[�x�E�X�e���V��
	m_pDepthStencilView,		//�[�x�X�e���V���r���[
	m_ViewPort,									//�r���[�|�[�g
	m_pCullNoneRasterizerState,		//���X�^���C�U
	m_pCullLeftRasterizerState,
	m_pCullRightRasterizerState,
	m_pWIREFRAMERasterizerState,
	m_pBlendState,							//�f�t�H���g�u�����h�X�e�[�g
	v_pAlphaState,
	v_pAddState
	};
	cGameScene::SetDirectXData(data);
	cGameScene::m_TexDrawData = cSceneManager::GetInstance().m_pDefaultRnedTex->GetDrawData();

#if defined(DEBUG) || defined(_DEBUG)
	cSceneManager::GetInstance().SetProcessNum(m_ProcessMonitor.GetProcessorsNum() + 1);
	for (int i = 0; i < m_ProcessMonitor.GetProcessorsNum() + 1; i++) {
		cSceneManager::GetInstance().SetProcessRate(i, m_ProcessMonitor.GetProcessRate(i));
	}
	cMemoryMonitor::GetInstance();	//�������̏����l���擾����
#endif

	return true;
}

void cDirectX11::InitSceneManager()
{
	cSceneManager::GetInstance().m_pDepthStencilView = m_pDepthStencilView;
	cSceneManager::GetInstance().m_pImmediateContext = m_pImmediateContext;
	cSceneManager::GetInstance().m_pRenderTargetView = m_pRenderTargetView;
	cSceneManager::GetInstance().m_pSwapChain = m_pSwapChain;
	Scene::Push<cMainScene>();
}

/// <summary>
/// DirectX�̏�������S������֐�
/// </summary>
/// <returns></returns>
HRESULT cDirectX11::InitDirect3D()
{
	// �E�C���h�E�̃N���C�A���g �G���A
	RECT rc;
	GetClientRect(m_hWindow, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// �f�o�C�X�ƃX���b�v �`�F�C���̍쐬
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));	// �\���̂̒l��������
	sd.BufferCount = 3;		// �o�b�N �o�b�t�@��
	sd.BufferDesc.Width = width;	// �o�b�N �o�b�t�@�̕�
	sd.BufferDesc.Height = height;	// �o�b�N �o�b�t�@�̍���
									//sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // �t�H�[�}�b�g
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // �t�H�[�}�b�g
	sd.BufferDesc.RefreshRate.Numerator = 60;  // ���t���b�V�� ���[�g(���q)
	sd.BufferDesc.RefreshRate.Denominator = 1; // ���t���b�V�� ���[�g(����)
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;	// �X�L�������C��
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;			// �X�P�[�����O
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �o�b�N �o�b�t�@�̎g�p�@
	sd.OutputWindow = m_hWindow;	// �֘A�t����E�C���h�E
	sd.SampleDesc.Count = 1;		// �}���` �T���v���̐�
	sd.SampleDesc.Quality = 0;		// �}���` �T���v���̃N�I���e�B
	sd.Windowed = TRUE;				// �E�C���h�E ���[�h
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // ���[�h�����؂�ւ�

#if defined(DEBUG) || defined(_DEBUG)
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT createDeviceFlags = 0;
#endif

	// �n�[�h�E�F�A�E�f�o�C�X���쐬
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
		g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
		&m_pSwapChain, &m_pD3DDevice, &g_FeatureLevelsSupported,
		&m_pImmediateContext);
	if (FAILED(hr)) {
		// WARP�f�o�C�X���쐬
		hr = D3D11CreateDeviceAndSwapChain(
			NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags,
			g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
			&m_pSwapChain, &m_pD3DDevice, &g_FeatureLevelsSupported,
			&m_pImmediateContext);
		if (FAILED(hr)) {
			// ���t�@�����X�E�f�o�C�X���쐬
			hr = D3D11CreateDeviceAndSwapChain(
				NULL, D3D_DRIVER_TYPE_REFERENCE, NULL, createDeviceFlags,
				g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
				&m_pSwapChain, &m_pD3DDevice, &g_FeatureLevelsSupported,
				&m_pImmediateContext);
			if (FAILED(hr)) {
				return DXTRACE_ERR("InitDirect3D D3D11CreateDeviceAndSwapChain", hr);
			}
		}
	}

	// **********************************************************
	// �u�����h�E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.AlphaToCoverageEnable = TRUE;
	BlendState.IndependentBlendEnable = FALSE;
	BlendState.RenderTarget[0].BlendEnable = FALSE;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_pD3DDevice->CreateBlendState(&BlendState, &m_pBlendState);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateBlendState", hr);

	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));
	BlendDesc.AlphaToCoverageEnable = TRUE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;			//ONE
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;		//ONE
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;		//ONE
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	GetDirectX::Device()->CreateBlendState(&BlendDesc, &v_pAlphaState);

	D3D11_BLEND_DESC AddDesc;
	ZeroMemory(&AddDesc, sizeof(AddDesc));
	AddDesc.AlphaToCoverageEnable = TRUE;
	AddDesc.IndependentBlendEnable = FALSE;
	AddDesc.RenderTarget[0].BlendEnable = TRUE;
	AddDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;			//ONE
	AddDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;		//ONE
	AddDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	AddDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	AddDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;		//ONE
	AddDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	AddDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	GetDirectX::Device()->CreateBlendState(&AddDesc, &v_pAddState);

	// ���X�^���C�U�E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D11_FILL_SOLID;   // ���ʂɕ`�悷��
	RSDesc.CullMode = D3D11_CULL_NONE;    // ���ʂ�`�悷��
	RSDesc.FrontCounterClockwise = FALSE; // ���v��肪�\��
	RSDesc.DepthBias = 0;
	RSDesc.DepthBiasClamp = 0;
	RSDesc.SlopeScaledDepthBias = 0;
	RSDesc.DepthClipEnable = TRUE;
	RSDesc.ScissorEnable = FALSE;
	RSDesc.MultisampleEnable = FALSE;
	RSDesc.AntialiasedLineEnable = FALSE;
	hr = m_pD3DDevice->CreateRasterizerState(&RSDesc, &m_pCullNoneRasterizerState);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateRasterizerState", hr);

	RSDesc.CullMode = D3D11_CULL_FRONT;    // ������W��̕\��`�悷��
	hr = m_pD3DDevice->CreateRasterizerState(&RSDesc, &m_pCullLeftRasterizerState);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateRasterizerState", hr);

	RSDesc.CullMode = D3D11_CULL_BACK;    // ������W��̗���`�悷��
	hr = m_pD3DDevice->CreateRasterizerState(&RSDesc, &m_pCullRightRasterizerState);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateRasterizerState", hr);

	D3D11_RASTERIZER_DESC wRSDesc;
	wRSDesc.CullMode = D3D11_CULL_NONE;    // ���ʂ�`�悷��
	wRSDesc.FillMode = D3D11_FILL_WIREFRAME;		//���C���[�t���[��
	wRSDesc.FrontCounterClockwise = FALSE; // ���v��肪�\��
	wRSDesc.DepthBias = 0;
	wRSDesc.DepthBiasClamp = 0;
	wRSDesc.SlopeScaledDepthBias = 0;
	wRSDesc.DepthClipEnable = TRUE;
	wRSDesc.ScissorEnable = FALSE;
	wRSDesc.MultisampleEnable = FALSE;
	wRSDesc.AntialiasedLineEnable = FALSE;
	hr = m_pD3DDevice->CreateRasterizerState(&wRSDesc, &m_pWIREFRAMERasterizerState);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateRasterizerState", hr);

	// **********************************************************
	// �[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_DEPTH_STENCIL_DESC DepthStencil;
	DepthStencil.DepthEnable = TRUE; // �[�x�e�X�g����
	DepthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // ��������
	DepthStencil.DepthFunc = D3D11_COMPARISON_LESS; // ��O�̕��̂�`��
	DepthStencil.StencilEnable = FALSE; // �X�e���V���E�e�X�g�Ȃ�
	DepthStencil.StencilReadMask = 0;     // �X�e���V���ǂݍ��݃}�X�N�B
	DepthStencil.StencilWriteMask = 0;     // �X�e���V���������݃}�X�N�B
										   // �ʂ��\�������Ă���ꍇ�̃X�e���V���E�e�X�g�̐ݒ�
	DepthStencil.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilFunc = D3D11_COMPARISON_NEVER; // ��Ɏ��s
																 // �ʂ����������Ă���ꍇ�̃X�e���V���E�e�X�g�̐ݒ�
	DepthStencil.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // ��ɐ���
	hr = m_pD3DDevice->CreateDepthStencilState(&DepthStencil,
		&m_pDepthStencilState);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateDepthStencilState", hr);

	// **********************************************************
	// �o�b�N �o�b�t�@�̏�����
	hr = InitBackBuffer();
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D InitBackBuffer", hr);

	return hr;
}

/// <summary>
/// �`��p�̃o�b�N�o�b�t�@�̏��������s��
/// </summary>
/// <returns></returns>
HRESULT cDirectX11::InitBackBuffer()
{
	HRESULT hr;

	// �X���b�v�E�`�F�C������ŏ��̃o�b�N�E�o�b�t�@���擾����
	ID3D11Texture2D *pBackBuffer;  // �o�b�t�@�̃A�N�Z�X�Ɏg���C���^�[�t�F�C�X
	hr = m_pSwapChain->GetBuffer(
		0,                         // �o�b�N�E�o�b�t�@�̔ԍ�
		__uuidof(ID3D11Texture2D), // �o�b�t�@�ɃA�N�Z�X����C���^�[�t�F�C�X
		(LPVOID*)&pBackBuffer);    // �o�b�t�@���󂯎��ϐ�
	if (FAILED(hr))
		return DXTRACE_ERR("InitBackBuffer g_pSwapChain->GetBuffer", hr);  // ���s

																		   // �o�b�N�E�o�b�t�@�̏��
	D3D11_TEXTURE2D_DESC descBackBuffer;
	pBackBuffer->GetDesc(&descBackBuffer);

	// �o�b�N�E�o�b�t�@�̕`��^�[�Q�b�g�E�r���[�����
	hr = m_pD3DDevice->CreateRenderTargetView(
		pBackBuffer,           // �r���[�ŃA�N�Z�X���郊�\�[�X
		NULL,                  // �`��^�[�Q�b�g�E�r���[�̒�`
		&m_pRenderTargetView); // �`��^�[�Q�b�g�E�r���[���󂯎��ϐ�
	SAFE_RELEASE(pBackBuffer);  // �ȍ~�A�o�b�N�E�o�b�t�@�͒��ڎg��Ȃ��̂ŉ��
	if (FAILED(hr))
		return DXTRACE_ERR("InitBackBuffer g_pD3DDevice->CreateRenderTargetView", hr);  // ���s

																						// �[�x/�X�e���V���E�e�N�X�`���̍쐬
	D3D11_TEXTURE2D_DESC descDepth = descBackBuffer;
	//	descDepth.Width     = descBackBuffer.Width;   // ��
	//	descDepth.Height    = descBackBuffer.Height;  // ����
	descDepth.MipLevels = 1;       // �~�b�v�}�b�v�E���x����
	descDepth.ArraySize = 1;       // �z��T�C�Y
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;  // �t�H�[�}�b�g(�[�x�̂�)
											   //	descDepth.SampleDesc.Count   = 1;  // �}���`�T���v�����O�̐ݒ�
											   //	descDepth.SampleDesc.Quality = 0;  // �}���`�T���v�����O�̕i��
	descDepth.Usage = D3D11_USAGE_DEFAULT;      // �f�t�H���g�g�p�@
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL; // �[�x/�X�e���V���Ƃ��Ďg�p
	descDepth.CPUAccessFlags = 0;   // CPU����̓A�N�Z�X���Ȃ�
	descDepth.MiscFlags = 0;   // ���̑��̐ݒ�Ȃ�
	hr = m_pD3DDevice->CreateTexture2D(
		&descDepth,         // �쐬����2D�e�N�X�`���̐ݒ�
		NULL,               // 
		&m_pDepthStencil);  // �쐬�����e�N�X�`�����󂯎��ϐ�
	if (FAILED(hr))
		return DXTRACE_ERR("InitBackBuffer g_pD3DDevice->CreateTexture2D", hr);  // ���s

																				 // �[�x/�X�e���V�� �r���[�̍쐬
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;            // �r���[�̃t�H�[�}�b�g
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = 0;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pD3DDevice->CreateDepthStencilView(
		m_pDepthStencil,       // �[�x/�X�e���V���E�r���[�����e�N�X�`��
		&descDSV,              // �[�x/�X�e���V���E�r���[�̐ݒ�
		&m_pDepthStencilView); // �쐬�����r���[���󂯎��ϐ�
	if (FAILED(hr))
		return DXTRACE_ERR("InitBackBuffer g_pD3DDevice->CreateDepthStencilView", hr);  // ���s

																						// �r���[�|�[�g�̐ݒ�
	m_ViewPort.TopLeftX = 0.0f;    // �r���[�|�[�g�̈�̍���X���W�B
	m_ViewPort.TopLeftY = 0.0f;    // �r���[�|�[�g�̈�̍���Y���W�B
	m_ViewPort.Width = (FLOAT)descBackBuffer.Width;   // �r���[�|�[�g�̈�̕�
	m_ViewPort.Height = (FLOAT)descBackBuffer.Height;  // �r���[�|�[�g�̈�̍���
	m_ViewPort.MinDepth = 0.0f; // �r���[�|�[�g�̈�̐[�x�l�̍ŏ��l
	m_ViewPort.MaxDepth = 1.0f; // �r���[�|�[�g�̈�̐[�x�l�̍ő�l

								//�T�C�Y��ۑ�
	m_sizeWindow.cx = descBackBuffer.Width;
	m_sizeWindow.cy = descBackBuffer.Height;

	return S_OK;
}

/// <summary>
/// DirectX�̏I���������s��
/// </summary>
/// <returns></returns>
bool cDirectX11::CleanupDirect3D()
{
	// �f�o�C�X�E�X�e�[�g�̃N���A
	if (m_pImmediateContext)
		m_pImmediateContext->ClearState();

	// �X���b�v �`�F�C�����E�C���h�E ���[�h�ɂ���
	if (m_pSwapChain)
		m_pSwapChain->SetFullscreenState(FALSE, NULL);

	// �擾�����C���^�[�t�F�C�X�̊J��
	SAFE_RELEASE(m_pDepthStencilState);

	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pDepthStencil);

	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pSwapChain);

	SAFE_RELEASE(m_pImmediateContext);
	SAFE_RELEASE(m_pD3DDevice);

	return true;
}

/// <summary>
/// �A�v���P�[�V�������I��������
/// </summary>
void cDirectX11::CleanupApp()
{
	// �E�C���h�E �N���X�̓o�^����
	UnregisterClass(g_szWndClass, m_hInstance);
	delete m_pComboBox;
	m_pComboBox = nullptr;
	delete m_pDX11;
	m_pDX11 = nullptr;
}

/// <summary>
/// �g�p����@�\���n�[�h�E�F�A���Ή����Ă��邩�m�F����B����@�\���ǉ������΂����ɋL�q����
/// </summary>
/// <returns></returns>
bool cDirectX11::CheckSystem()
{
	// DirectX Math���C�u�����̃T�|�[�g �`�F�b�N
	if (DirectX::XMVerifyCPUSupport() != TRUE)
	{
		DXTRACE_MSG("WinMain XMVerifyCPUSupport");
		return false;
	}

	//�������Ԍv���Ɏg�p����p�t�H�[�}���X�J�E���^�̑Ή����m�F����
	LARGE_INTEGER freq;
	if (!QueryPerformanceFrequency(&freq)) {
#if defined(DEBUG) || defined(_DEBUG)
		MessageBox(0, "���̏����n�ł̓n�C�p�t�H�[�}���X�J�E���^���T�|�[�g���Ă��Ȃ����ߏ������Ԍv�����s���܂���", NULL, MB_OK);
#endif
	}

	return true;		//���ׂ�OK
}

/// <summary>
/// �Q�[�����ŗ��p�����@�\�ŉ���������s�����̂�����΋L�q����
/// </summary>
void cDirectX11::DestroyApp()
{
	//�V�[���}�l�[�W���̉������
	cSceneManager::GetInstance().Destroy();
	XInputEnable(false);	//�R���g���[������
	cPostEffects::Destroy();
}

/// <summary>
/// �I���������s���AWinMain���V�X�e���ɕԋp����l��Ԃ��B
/// WinMain�͂��̒l�����̂܂�return����
/// </summary>
/// <returns></returns>
int cDirectX11::Destroy()
{
	CleanupDirect3D();
	CleanupApp();
	//�A�Z�b�g�̉���m�F
	cTexManager::GetInstance().CheckDelete();
	MeshManager::GetInstance().CheckDelete();
	return (int)msg.wParam;
}

ID3D11Device* GetDirectX::Device() {
	return cDirectX11::GetInstance()->Device();
}

ID3D11DeviceContext* GetDirectX::Context() {
	return cDirectX11::GetInstance()->Context();
}

HWND GetDirectX::Hwnd() {
	return cDirectX11::GetInstance()->GetHwnd();
}

HINSTANCE	GetDirectX::HInstance() {
	return cDirectX11::GetInstance()->HInstance();
}

void GetDirectX::InitOMSetRenderTargets()
{
	ID3D11RenderTargetView* pRTV[8] = { NULL };
	GetDirectX::Context()->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, pRTV, NULL);

	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { NULL };
	GetDirectX::Context()->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);
	GetDirectX::Context()->GSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);
	GetDirectX::Context()->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);
}