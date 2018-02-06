/*=================================================
//								FILE : SceneManager.h
//		�t�@�C������ :
//		�Q�[���V�[���̐؂�ւ��Ȃǂ��Ǘ�����}�l�[�W���B
//		�Q�[�����[�v���B�����邱�Ƃ��ړI
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include <deque>
#include <memory>
#include "GameScene.h"
#include "TimeCheck.h"
#include "ProcessMonitor.h"
#include "ScreenSpaceSSS.h"
#include "CrossFilter.h"
//#include "SMAntialiasing.h"
#include "FXAntialiasing.h"
#include "OutlineEmphasis.h"
#include "SkyDome.h"
#include "DepthOfField.h"

/// <summary>
/// �V�[�����Ǘ�����B�Q�[����ʂ��Ĉ�ł����̂ŃV���O���g���Ƃ���
/// </summary>
class cSceneManager {
public:
	//�X�V���������������s�����߂�cDirectX�N���X���t�����h�����Ă���
	friend class cDirectX11;
	cSceneManager(const cSceneManager&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cSceneManager& operator=(const cSceneManager&) = delete;	//�R�s�[������Z�q�� delete �w��
	cSceneManager(cSceneManager&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cSceneManager& operator=(cSceneManager&&) = delete;		//���[�u������Z�q�� delete �w��

	static cSceneManager& GetInstance() {
		static cSceneManager inst;			// private�ȃR���X�g���N�^���Ăяo��
		return inst;
	}

	//=============================================
	//			�V�[���؂�ւ���POP,PUSH,GOTO���L�q
	// TODO �����I�ɉ�ʐ؂�ւ�����������

	void cSceneManager::PopScene() {
		//���ݎ��s���̃V�[�����폜���邪�����Œ���POP����ƃV�[�������E���邱�ƂɂȂ�̂�
		//�t���O�ŊǗ������ĕ`��I����A�X�V���I������^�C�~���O��POP����
		m_ScenePopFlag = true;
	};

	template<typename T>void PushScene() {
		//���ݎ��s���Ă�����̂̏�ɐV�����V�[�����쐬����
		m_pSceneData.push_front(std::unique_ptr<T>(new T));
	};

	template<typename T>void GotoScene() {
		//���ݎ��s���̃V�[����POP���Ă���Push����
		PopScene();
		PushScene<T>();
	};
	//=============================================

private:
	//�e��X�V����
	void Update();
	//====== �`��n�֐�=====
	void Draw(bool _DebugFlag,bool _FreeCameraFlag,bool _GBufferDrawFlag);
	void RenderingStandby(bool _FreeCameraFlag);
	void RenderingShadowMap();
	void DeferredRenderingPass(bool _FreeCameraFlag);
	ID3D11ShaderResourceView* PostEffectPath(GBuffer& GraphicBuffer);
	void BackBufferRendering(ID3D11ShaderResourceView* LastData);
	//===================
	void DrawStart();
	void DrawEnd();
	void PopCheck();
	void FreeCameraUpdate();
	void GBufferDraw(GBuffer& _GBuffer);
	void SetProcessRate(int _No, int _Rate);
	void SetProcessNum(int _Num);
	void ProcessRateDraw();

	void Destroy();
	//=============================================


	cSceneManager();					//�R���X�g���N�^�� private �ɒu��
	~cSceneManager();					//�f�X�g���N�^�� private �ɒu��
	std::deque<std::unique_ptr<cGameScene>> m_pSceneData;		//�V�[���f�[�^���i�[����deque
	const cGameScene* m_pOldScene;							//�X�V������s�ɂ��邽��const�͐�΂ɊO���Ȃ�����
	cRenderTargetTex* m_pDefaultRnedTex;				//�V�X�e���Ƃ��ĕW���I�ɕ`�悷��e�N�X�`��

	IDXGISwapChain* m_pSwapChain;								 // �X���b�v�E�`�F�C��
	ID3D11DeviceContext* m_pImmediateContext;			// �f�o�C�X�E�R���e�L�X�g
	ID3D11RenderTargetView* m_pRenderTargetView;	//�^�[�Q�b�g�r���[
	ID3D11DepthStencilView* m_pDepthStencilView;		//�[�x�X�e���V���r���[

	//=====�G�t�F�N�g========
	cSkyDome* m_SkyDome;	//�X�J�C�h�[���`��ɗ��p����
	cScreenSpaceSSS* m_SSSSS;
	cCrossFilter* m_CrossFilter;
	cFXAA* m_FXAA;
	cOutlineEmphasis* m_OutlineEmphasis;
	cDepthOfField* m_DepthOfField;


	//=====�������Ԍv��=====
	cTimeCheck m_TimeUpdate;				//�Q�[���A�b�v�f�[�g�̏������Ԍv�����s��
	cTimeCheck m_TimeDraw;					//�`��̏������Ԍv�����s��
	int m_ProcessRate[MAX_PROCESSORS_COUNT];	//CPU�g�p�����V�X�e�����������ĕێ����Ă���
	int m_ProcessNum;		//�v���Z�X��
	ID3D11ShaderResourceView* m_pCPURateBar;	//CPU�g�p���Q�[�W��\��������̂ɗ��p����
	ID3D11ShaderResourceView* m_pMemoryRateBar;	//�������̎g�p����\������
	ID3D11ShaderResourceView* m_pBaseBar;			//���̃o�[�̉��ɕ\������
	//===================
	bool m_ScenePopFlag;											//�V�[����POP���ꂽ���ǂ����̃t���O
};

//�V�[���}�l�[�W�������b�v���ČĂяo��
namespace Scene {
	void Pop();
	template<typename T>void Push() {
		//���ݎ��s���Ă�����̂̏�ɐV�����V�[�����쐬����
		cSceneManager::GetInstance().PushScene<T>();
	};
	template<typename T>void Goto() {
		//���ݎ��s���Ă�����̂̏�ɐV�����V�[�����쐬����
		cSceneManager::GetInstance().GotoScene<T>();
	};
}