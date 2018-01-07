/*=================================================
//								FILE : HitCheck.h
//		�t�@�C������ :
//		�����蔻����s���֐����܂Ƃ߂�
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"

//�����蔻����܂Ƃ߂����O���
namespace HitCheck {
	bool HitCheckSphere(const DirectX::XMFLOAT3& p1, const float r1, const DirectX::XMFLOAT3& p2, const float r2);
	bool AABBandLine(const DirectX::XMFLOAT3& _BoxMin, const DirectX::XMFLOAT3& _BoxMax, const DirectX::XMFLOAT3& _LineStart, const DirectX::XMFLOAT3& _LineEnd);		//AABB�Ɛ����̓����蔻��
	DirectX::XMFLOAT4 CreateTriangleToPlane(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2, const DirectX::XMFLOAT3& pos3);		//�O�p�`���畽�ʂ����
	bool LinetoPlaneCross(const DirectX::XMFLOAT4& plane, const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& vec, DirectX::XMFLOAT3* out);
	bool CheckInTriangle(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2, const DirectX::XMFLOAT3& p3, const DirectX::XMFLOAT3& CheckPoint);	//�O�p�`�̓��O����
}
