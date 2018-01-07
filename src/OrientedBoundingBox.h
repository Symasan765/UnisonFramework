/*=================================================
//								FILE : OrientedBoundingBox.h
//		�t�@�C������ :
//		
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"
#include "Mesh3D.h"

struct OBBBOX {
	DirectX::XMFLOAT3 vAxisX;		//X��
	DirectX::XMFLOAT3 vAxisY;		//Y��
	DirectX::XMFLOAT3 vAxisZ;		//Z��
	DirectX::XMFLOAT3 vCenter;	//���S���W
	DirectX::XMFLOAT3 vPos;		//���݂̈ʒu
	float vLenX;				//Box��X�T�C�Y�̔���
	float vLenY;				//Box��Y�T�C�Y�̔���
	float vLenZ;				//Box��Z�T�C�Y�̔���
};

/// <summary>
/// OBB�ɂ�铖���蔻����s���N���X
/// </summary>
class cOBB {
public:
	cOBB();
	~cOBB();
	void Init(const Mesh3D & _mesh);
	void Exit();
	void Update(DirectX::XMFLOAT4X4);
	void Draw();
	bool Collision(cOBB _obb);
	DirectX::XMFLOAT3 GetPos() { return m_Info.vPos; };
	OBBBOX GetOBB() { return m_Info; };
	DirectX::XMFLOAT4X4 GetWorldMat();
private:
	/// <summary>
	/// ��Փ� = false, �Փ� = true
	/// </summary>
	/// <param name="_boxA">BoxA</param>
	/// <param name="_boxB">BoxB</param>
	/// <param name="Separate">������</param>
	/// <param name="_Distance">��̃I�u�W�F�N�g�����ԃx�N�g��</param>
	/// <returns>�Փ˂��ۂ�</returns>
	bool CompareLength(const OBBBOX& _boxA, const OBBBOX& _boxB, const DirectX::XMFLOAT3& Separate, const DirectX::XMFLOAT3& _Distance);
	void CalculateBoundingBox(const Mesh3D & _mesh);
	OBBBOX m_Info;			//OBB���
	DirectX::XMFLOAT4X4 m_WorldMat;		//���[���h�ϊ��s��
};