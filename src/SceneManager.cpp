/*=================================================
//								FILE : SceneManager.cpp
//		�t�@�C������ :
//		�Q�[�����ŃV�[����؂�ւ��邽�߂̃Q�[���V�[���}�l�[�W����
//		�������e���L�q����B
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "FontDraw.h"
#include "SceneManager.h"
#include "DeferredRendering.h"
#include "DeferredConst.h"
#include "ShadowMap.h"
#include "SpriteDraw.h"
#include "MemoryMonitor.h"
#include "DeferredBaseModel.h"

cSceneManager::cSceneManager()
{
	//�ϐ��̏���������
	m_pOldScene = nullptr;
	m_pSwapChain = nullptr;
	m_pImmediateContext = nullptr;
	m_pRenderTargetView = nullptr;
	m_pDepthStencilView = nullptr;
	m_ScenePopFlag = false;
	m_pDefaultRnedTex = new cRenderTargetTex();
	m_pDefaultRnedTex->RTCreate((float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y, { 0,0,0 });
	m_SSSSS = new cScreenSpaceSSS;
	m_CrossFilter = new cCrossFilter;
	m_FXAA = new cFXAA;
	m_OutlineEmphasis = new cOutlineEmphasis;

#if defined(DEBUG) || defined(_DEBUG)
	if (FAILED(D3DX11CreateShaderResourceViewFromFileA(GetDirectX::Device(), "asset/Texture/Debug/CPURateBar.png", NULL, NULL, &m_pCPURateBar, NULL))) {
		MessageBox(0, "CPU���p���摜�擾���s", NULL, MB_OK);
	}
	if (FAILED(D3DX11CreateShaderResourceViewFromFileA(GetDirectX::Device(), "asset/Texture/Debug/MemoryRateBar.png", NULL, NULL, &m_pMemoryRateBar, NULL))) {
		MessageBox(0, "�������[���p���摜�擾���s", NULL, MB_OK);
	}
	if (FAILED(D3DX11CreateShaderResourceViewFromFileA(GetDirectX::Device(), "asset/Texture/Debug/BarBase.png", NULL, NULL, &m_pBaseBar, NULL))) {
		MessageBox(0, "���o�[�摜�擾���s", NULL, MB_OK);
	}
#endif
}

cSceneManager::~cSceneManager()
{
	SAFE_RELEASE(m_pCPURateBar);
	SAFE_RELEASE(m_pMemoryRateBar);
	SAFE_RELEASE(m_pBaseBar);
	delete m_pDefaultRnedTex;
	m_pDefaultRnedTex = nullptr;
	delete m_SSSSS;
	delete m_CrossFilter;
	delete m_FXAA;
	delete m_OutlineEmphasis;
}

/// <summary>
/// �V�[���̃��C���X�V�������s��
/// �V�[���͍X�V�r���ŕʂ̃V�[���֍s�����Ƃ�����̂�
/// �ς�����ꍇ�͎��̕`����΂����ƂɂȂ�B
/// </summary>
void cSceneManager::Update() {
#if defined(DEBUG) || defined(_DEBUG)
	m_TimeUpdate.TimerStart();		//�f�o�b�O���[�h���͏������Ԃ��v�����s��
#endif

	//�܂��͍X�V����
	m_pOldScene = m_pSceneData[0].get();		//�X�V�������ɃV�[���؂�ւ�������ꍇ������̂Ō��݂̃A�h���X��ێ�����
	m_pSceneData[0]->Update();						//�X�V���鎞�͂����Ƃ��擪�̂��̂������X�V����

#if defined(DEBUG) || defined(_DEBUG)
	m_TimeUpdate.TimerEnd();			//���Ԍv���I��
#endif
}

/// <summary>
/// �V�[���̕`����s���B
/// ��{�I�Ƀf�B�t�@�[�h�����_�����O�x�[�X�ō쐬����\��Ȃ̂�
/// �`��̃t�F�[�Y��G-Buffer�쐬�Ƃ��̌�̃|�X�g�G�t�F�N�g�Ƃŕ����Ă���
/// </summary>
void cSceneManager::Draw(bool _DebugFlag, bool _FreeCameraFlag, bool _GBufferDrawFlag) {
	//Update�̒��ŃV�[���̍X�V���������ꍇ�͕`����΂�
	if (m_pSceneData[0].get() != m_pOldScene) return;

#if defined(DEBUG) || defined(_DEBUG)
	m_TimeDraw.TimerStart();		//�f�o�b�O���[�h���͏������Ԃ��v�����s��
#endif

	DrawStart();		//�`��X�^�[�g�B�����ŉ�ʃN���A�����=============================

	RenderingStandby(_FreeCameraFlag);

	RenderingShadowMap();

	DeferredRenderingPass();

	GBuffer GraphicBuffer = cDeferredRendering::GetInstance().GetGraphicBuffer();

	ID3D11ShaderResourceView * LastData = PostEffectPath(GraphicBuffer);

	BackBufferRendering(LastData);

	//=======================�f�o�b�O���̕`��=================================
#if defined(DEBUG) || defined(_DEBUG)
	m_pSceneData[0]->SetRendBuffer();		//�W���I�ȃ����_�����O�ݒ��ON�ɂ���
	cFontDraw::getInstance()->Draw();		//�Q�[���V�[�����ɐݒ肳�ꂽ�������܂Ƃ߂ĕ`�悷��(2017/)
	if (_DebugFlag)
		ProcessRateDraw();
	if (_GBufferDrawFlag)
		GBufferDraw(cDeferredRendering::GetInstance().GetGraphicBuffer());	//G-Buffer�\��

	m_TimeDraw.TimerEnd();			//���������O�Ɏ��Ԍv���I��
#endif

	DrawEnd();			//�`��I���B�����ŉ�ʂ��؂�ւ��================================
}

void cSceneManager::RenderingStandby(bool _FreeCameraFlag)
{
	//�܂��̓J�����̃v���W�F�N�V�������s���B�v���W�F�N�V�����̃f�[�^��������Ă���
	const ViewProj& ViewPorjData = m_pSceneData[0]->m_CameraData.Projection(_FreeCameraFlag);
	cDeferredConst::SetCameraData(m_pSceneData[0]->m_CameraData.GetCameraData(_FreeCameraFlag), ViewPorjData);
	cDeferredConst::SetLightData(m_pSceneData[0]->m_DirectionalLight.GetLightPos());
	m_pSceneData[0]->SetDefaultRender();		//�W���I�ȃ����_�����O�ݒ��ON�ɂ���
	m_pSceneData[0]->m_DirectionalLight.SetConstantBuffer(3);
	m_pSceneData[0]->m_CameraData.SetConstBuffer(4, _FreeCameraFlag);
}

void cSceneManager::RenderingShadowMap()
{
	//�[�x�}�b�v���쐬����
	cDeferredModel::DefaultRenderFlag(false);
	cShadowMap::GetInstance().SetRender();
	m_pSceneData[0]->MeshDraw();			//�V���h�E�p�̐[�x�}�b�v�������
}

void cSceneManager::DeferredRenderingPass()
{
	//G-Buffer���쐬����
	cDeferredModel::DefaultRenderFlag(true);
	cDeferredRendering::GetInstance().SetDeferredRendering(cShadowMap::GetInstance().GetDepthResourceView());		//�V���h�E�}�b�v�쐬�̂��ߐ[�x�}�b�v��n��
	m_pSceneData[0]->MeshDraw();		//������G-Buffer����������
}

ID3D11ShaderResourceView * cSceneManager::PostEffectPath(GBuffer& GraphicBuffer)
{
	//��������SSSSS��K�p����
	m_SSSSS->DrawSSS(GraphicBuffer.vSSSSS, GraphicBuffer.vNormal, GraphicBuffer.vDiffuse);

	//�֊s������������
	m_OutlineEmphasis->DrawOutLine(m_SSSSS->GetResourceView(), GraphicBuffer.vNormal);

	//�A���`�G�C���A�X�������s��
	m_FXAA->DrawFXAA(m_OutlineEmphasis->GetResourceView());

	//����|�X�g�G�t�F�N�g�̍ŏI�p�X���ς��΂����ŕԋp����e�N�X�`����ύX����
	return m_FXAA->GetResourceView();
}

void cSceneManager::BackBufferRendering(ID3D11ShaderResourceView * LastData)
{
	m_pSceneData[0]->SetRendBuffer();
	m_pSceneData[0]->SetBlendState(BrendStateNo::NONE);

	//��ʂփ����_�����O����
	cSprite2DDraw::GetInstance().Draw(LastData, { 0.0f,0.0f }, { (float)WINDOW_SIZE_X,(float)WINDOW_SIZE_Y });
}

void cSceneManager::DrawStart() {
	// �`��^�[�Q�b�g�̃N���A
	//const float ClearColor[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
	const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(
		m_pRenderTargetView, // �N���A����`��^�[�Q�b�g
		ClearColor);         // �N���A����l

							 // �[�x/�X�e���V���̃N���A
	m_pImmediateContext->ClearDepthStencilView(
		m_pDepthStencilView, // �N���A����[�x/�X�e���V���E�r���[
		D3D11_CLEAR_DEPTH,   // �[�x�l�������N���A����
		1.0f,                // �[�x�o�b�t�@���N���A����l
		0);                  // �X�e���V���E�o�b�t�@���N���A����l(���̏ꍇ�A���֌W)

	m_pDefaultRnedTex->SetRenderTargetTex();
}

void cSceneManager::DrawEnd() {
	m_pSwapChain->Present(1,	// ���������ōX�V����
		0);	// ��ʂ����ۂɍX�V����
}

void cSceneManager::PopCheck()
{
	//�Ō��pop����Ă��邩���m�F���A����Ă����POP
	if (m_ScenePopFlag) {
		m_pSceneData.pop_front();	//������POP�I
		m_ScenePopFlag = false;		//POP����������΂��Ƃɖ߂�
	}
}

/// <summary>
/// �t���[�J�����̃A�b�v�f�[�g���s��
/// </summary>
void cSceneManager::FreeCameraUpdate()
{
	m_pSceneData[0]->m_CameraData.FreeCameraOperation();
}

/// <summary>
/// G-Buffer��\��������֐�
/// </summary>
/// <param name="_GBuffer"></param>
void cSceneManager::GBufferDraw(GBuffer & _GBuffer)
{
	//�摜�ꖇ���̃T�C�Y
	const float offsetX = (float)WINDOW_SIZE_X / 8.0f;
	const float offsetY = (float)WINDOW_SIZE_Y / 8.0f;

	const float startX = 0.0f;												//��ԍ�����`��
	const float startY = WINDOW_SIZE_Y - offsetY * 3.0f;	//��ԉ��̃s�N�Z������摜�O���ڂ̈ʒu

	//2017/12/22���_��G-Buffer��5���ō\�������
	cSprite2DDraw::GetInstance().Draw(_GBuffer.vDiffuse, { startX,startY + offsetY * 0 }, { offsetX,offsetY });
	cSprite2DDraw::GetInstance().Draw(_GBuffer.vPosition, { startX,startY + offsetY * 1 }, { offsetX,offsetY });
	cSprite2DDraw::GetInstance().Draw(_GBuffer.vNormal, { startX,startY + offsetY * 2 }, { offsetX,offsetY });

	cSprite2DDraw::GetInstance().Draw(_GBuffer.vHighBrightness, { offsetX,startY + offsetY * 0 }, { offsetX,offsetY });
	cSprite2DDraw::GetInstance().Draw(_GBuffer.vSSSSS, { offsetX,startY + offsetY * 1 }, { offsetX,offsetY });

	cSprite2DDraw::GetInstance().Draw(cShadowMap::GetInstance().GetDepthResourceView(), { offsetX * 2,startY + offsetY * 0 }, { offsetY * 3,offsetY * 3 });
}

void cSceneManager::SetProcessRate(int _No, int _Rate)
{
	m_ProcessRate[_No] = _Rate;
}

void cSceneManager::SetProcessNum(int _Num)
{
	m_ProcessNum = _Num;
}

void cSceneManager::ProcessRateDraw()
{
	for (int i = 0; i < m_ProcessNum; i++) {
		float MaxLen = 50.0f;
		MaxLen *= (m_ProcessRate[i] / 100.0f);
		cSprite2DDraw::GetInstance().Draw(m_pCPURateBar, { 80.0f,238.0f + (i * 15.0f) }, { MaxLen,10.0f });
	}

	//�������[���
	MemoryData Memory = cMemoryMonitor::GetInstance().GetMemoryData();
	float Rate = ((float)Memory.vUsed / (float)Memory.vSize);	//�������[�̎g�p����\��
	float MaxLen = 200.0f;	//�ő咷
	float Len = MaxLen * Rate;
	float pos = 265 + (m_ProcessNum * 15) + 8;
	cSprite2DDraw::GetInstance().Draw(m_pBaseBar, { 15.0f,pos }, { MaxLen,20.0f });
	cSprite2DDraw::GetInstance().Draw(m_pMemoryRateBar, { 15.0f,pos }, { Len,20.0f });
}

//�V�[�����폜����
void cSceneManager::Destroy()
{
	m_pSceneData.erase(m_pSceneData.begin(), m_pSceneData.end());		//�V�[�������ׂč폜
}

void Scene::Pop()
{
	cSceneManager::GetInstance().PopScene();
}
