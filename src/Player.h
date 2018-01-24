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
private:
	void Move();
	void PosClamp();
	void RotationCalculation(DirectX::XMFLOAT2 ProgressDir);
	DirectX::XMFLOAT3 MoveCorrection(DirectX::XMFLOAT2 ProgressDir);
	cLuaActor* m_LuaScript;
	ViewProj m_CameraData;
	int m_anmCnt;
};