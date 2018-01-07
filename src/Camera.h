/*=================================================
//								FILE : Camera.h
//		�t�@�C������ :
//		�Q�[���V�[�����Ŏ�舵�����C���J��������������N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"
#include "ConstantBuffer.h"
#include <lua.hpp>
#include <string>

/// <summary>
/// �J�����̃v���W�F�N�V�����ɕK�v�ȃf�[�^���܂Ƃ߂��\����
/// </summary>
struct CameraData {
	DirectX::XMFLOAT3 vPos;			//�J�������W
	DirectX::XMFLOAT3 vLook;		//�����_
	DirectX::XMFLOAT3 vUp;			//������x�N�g��
	float vNear;
	float vFar;
};

/// <summary>
/// �J�����̃r���[�ƃv���W�F�N�V�������Ǘ�����
/// </summary>
struct ViewProj {
	DirectX::XMFLOAT4X4 vView;
	DirectX::XMFLOAT4X4 vProj;
	DirectX::XMFLOAT4X4 vVP;		//��L��̍���
};

/// <summary>
/// �Q�[���V�[�����ŊǗ������J�������`����B
/// </summary>
class cCamera {
public:
	friend class cSceneManager;		//�v���W�F�N�V�������V�[���}�l�[�W�����Ɍ��肷�邽��
	cCamera();
	~cCamera();
	void SetPosition(const DirectX::XMFLOAT3& _pos);
	void SetUpVect(const DirectX::XMFLOAT3& _Up);
	void SetLookPoint(const DirectX::XMFLOAT3& _look);
	void SetCameraAllData(const CameraData& _data);
	void SetNear(const float Near,const float Far);

	void SetConstBuffer(const int pass,bool debugFlag = false);
	CameraData GetCameraData(bool _FreeCameraFlag);

	void VectMove(const float rate);
	void LookPointRotate(const float rotX, const float rotY, const float rotZ);
	const DirectX::XMFLOAT3 GetAxisZ()const;

	void AddFunctionToLua(lua_State* L, std::string LuaVarName = "Camera");
private:
	const ViewProj& Projection(bool _FreeCameraFlag = false);		//�v���W�F�N�V�����͂ǂ��ł��o����悤�ɂ��Ȃ�
	void FreeCameraOperation();			//�t���[�J�����̑�����s���B�V�[������ĂԂ��Ƃ������Ă͂����Ȃ��̂Ńv���C�x�[�g��
	CameraData m_CameraData;			//�ʏ�̃����_�����O�ɗ��p
	CameraData m_DebugCameraData;	//�f�o�b�O���̃t���[�J�����@�\�ŗ��p
	ViewProj m_CameraVP;					//�ʏ�̃����_�����O�Ŏg�p����VP��ێ�
	ViewProj m_DebugVP;						//�t���[�J�����ŗ��p����VP��ێ�
	cConstBuf<DirectX::XMFLOAT4> m_ShaderConst;
	// TODO �����I�ɂ͎�����\���p��static�Ȓ��_�o�b�t�@�Ȃǂ����

	//======================== Lua�pGlue�֐� ===============================
	static int VectMoveGlue(lua_State* L);
	static int LookRotGlue(lua_State* L);
	static int SetPosGlue(lua_State* L);
	static int SetLookGlue(lua_State* L);
	static int SetUpGlue(lua_State* L);
};