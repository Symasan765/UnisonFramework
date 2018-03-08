/*=================================================
//								FILE : Player.h
//		�t�@�C������ :
//		�v���C���[�̏������L�q����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "DeferredBaseModel.h"
#include "LuaActor.h"

class cPlayer : public cDeferredModel{
public:
	cPlayer();
	~cPlayer();
	void Draw();
	void Update();	//������O�ɃJ�����̃A�b�v�f�[�g��������
	void SetCameraData(ViewProj data);

	enum MoveState {
		WALK = 0,
		DASH,
		WAIT
	};
private:
	void PlayerParamUpdate();
	void Move();
	void PosClamp();
	void InputAngleCorrection(DirectX::XMFLOAT2 inp);
	void RotationCalculation(DirectX::XMFLOAT2 ProgressDir);
	void PlayerVectChange();
	DirectX::XMFLOAT2 MoveCorrection(DirectX::XMFLOAT2 ProgressDir,const float speed);
	void MovingSpeedClamp();
	cLuaActor* m_LuaScript;
	ViewProj m_CameraData;

	//���
	float m_ConstScal;	//���f���̃T�C�Y��ς���K�v�����邽�ߖ{�Ԋ��ł͕s�v�ɂȂ�Ǝv����
	float m_anmCnt;
	float m_NowSpeed;		//�ړ������x�N�g�����Ǘ�����
	DirectX::XMFLOAT2 m_NowVect;			//�L�����N�^�[�̕������Ǘ�����

	//�p�����[�^
	float m_DashAddSpeed;
	float m_WalkAddSpeed;
	float m_MaxDashSpeed;
	float m_MaxWalkSpeed;
	float m_Switching;
	float m_Accele;
	float m_Decele;
	float m_MaxRotatAngle;

	MoveState m_MoveState;

};