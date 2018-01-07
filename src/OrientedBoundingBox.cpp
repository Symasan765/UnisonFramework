/*=================================================
//								FILE : OrientedBoundingBox.cpp
//		�t�@�C������ :
//		OBB�̓����蔻�菈�����s��
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "OrientedBoundingBox.h"

using namespace DirectX;

cOBB::cOBB()
{
	//�P�ʍs����Z�b�g���Ă���
	XMStoreFloat4x4(&m_WorldMat, XMMatrixIdentity());
}

cOBB::~cOBB()
{
	Exit();
}

void cOBB::Init(const Mesh3D & _mesh)
{
	CalculateBoundingBox(_mesh);
}

void cOBB::Exit()
{
}

void cOBB::Update(DirectX::XMFLOAT4X4 _Mat)
{
	m_Info.vAxisX = { _Mat._11,_Mat._12,_Mat._13 };
	m_Info.vAxisY = { _Mat._21,_Mat._22,_Mat._23 };
	m_Info.vAxisZ = { _Mat._31,_Mat._32,_Mat._33 };

	//�`��p�̍s����X�V
	m_WorldMat = _Mat;
	XMVECTOR Current = XMLoadFloat3(&m_Info.vCenter);
	XMVECTOR NewPos;
	NewPos = XMVector3TransformCoord(Current, XMLoadFloat4x4(&_Mat));

	XMStoreFloat3(&m_Info.vPos, NewPos);

	m_WorldMat._41 = m_Info.vPos.x;
	m_WorldMat._42 = m_Info.vPos.y;
	m_WorldMat._43 = m_Info.vPos.z;
}

bool cOBB::Collision(cOBB _obb)
{
	XMVECTOR Separate;		//������

	//��̃I�u�W�F�N�g�����񂾃x�N�g��
	XMVECTOR ObjPos = XMLoadFloat3(&_obb.GetPos());
	XMVECTOR MyPos = XMLoadFloat3(&m_Info.vPos);
	XMVECTOR Distance = MyPos - ObjPos;		//��̃I�u�W�F�N�g�����񂾃x�N�g��
	XMFLOAT3 _Dis;
	XMStoreFloat3(&_Dis, Distance);

	OBBBOX boxA, boxB;
	boxA = GetOBB();
	boxB = _obb.GetOBB();

	bool sts = true;
	
	//OBB-A�����X�g
	XMFLOAT3* OBB_A_Axis[3] = {
		&boxA.vAxisX,
		&boxA.vAxisY,
		&boxA.vAxisZ
	};

	//OBB-B�����X�g
	XMFLOAT3* OBB_B_Axis[3] = {
		&boxB.vAxisX,
		&boxB.vAxisY,
		&boxB.vAxisZ
	};

	//OBB-A�̂R���𕪗����ɂ��ă`�F�b�N
	for (int i = 0; i < 3; i++) {
		sts = CompareLength(boxA, boxB, *OBB_A_Axis[i], _Dis);

		//��ł��������ĂȂ����_�ŋA����I�I
		if (sts == false)
			return false;
	}

	//OBB-B�̂R���𕪗����ɂ��ă`�F�b�N
	for (int i = 0; i < 3; i++) {
		sts = CompareLength(boxA, boxB, *OBB_B_Axis[i], _Dis);

		//��ł��������ĂȂ����_�ŋA����I�I
		if (sts == false)
			return false;
	}

	//�O�ς𕪗����Ƃ��ĊO�ς𕪗����Ƃ��Čv�Z
	for (int p1 = 0; p1 < 3; p1++) {
		for (int p2 = 0; p2 < 3; p2++) {
			XMVECTOR cross;
			XMFLOAT3 crossAxis;
			cross = XMVector3Cross(XMLoadFloat3(OBB_A_Axis[p1]), XMLoadFloat3(OBB_B_Axis[p2]));
			XMStoreFloat3(&crossAxis, cross);

			sts = CompareLength(boxA, boxB, crossAxis, _Dis);

			//��ł��������ĂȂ����_�ŋA����I�I
			if (sts == false)
				return false;
		}
	}

	//���ׂĒʂ����瓖����I
	return true;
}

DirectX::XMFLOAT4X4 cOBB::GetWorldMat()
{
	XMMATRIX Ret = XMMatrixScaling(m_Info.vLenX / 2.0f, m_Info.vLenY / 2.0f, m_Info.vLenZ / 2.0f);
	Ret *= XMLoadFloat4x4(&m_WorldMat);

	XMFLOAT4X4 Mat;
	XMStoreFloat4x4(&Mat, Ret);

	return Mat;
}

bool cOBB::CompareLength(const OBBBOX & _boxA, const OBBBOX & _boxB, const DirectX::XMFLOAT3 & _Separate, const DirectX::XMFLOAT3 & _Distance)
{
	XMVECTOR Separate = XMLoadFloat3(&_Separate);		//������
	XMVECTOR Distance = XMLoadFloat3(&_Distance);		//��̃I�u�W�F�N�g�����ԃx�N�g��
	XMVECTOR dotVec = XMVector3Dot(Distance, Separate);
	XMFLOAT3 dotVecF;
	XMStoreFloat3(&dotVecF, dotVec);
	float fDistance = fabsf(XMVector3Dot(Distance, Separate).m128_f32[0]);

	//��������Ƀ{�b�N�XA���ˉe��������
	float ShadowA = 0;
	//��������Ƀ{�b�N�XB�Ɏˉe��������
	float ShadowB = 0;

	//���ꂼ��̃{�b�N�X�̉e���Z�o
	
	ShadowA = fabsf(XMVector3Dot(Separate, XMLoadFloat3(&_boxA.vAxisX)).m128_f32[0] * (_boxA.vLenX) / 2) +
		fabsf(XMVector3Dot(Separate, XMLoadFloat3(&_boxA.vAxisY)).m128_f32[0] * (_boxA.vLenY) / 2) +
		fabsf(XMVector3Dot(Separate, XMLoadFloat3(&_boxA.vAxisZ)).m128_f32[0] * (_boxA.vLenZ) / 2);
		
	ShadowB = fabsf(XMVector3Dot(Separate, XMLoadFloat3(&_boxB.vAxisX)).m128_f32[0] * (_boxB.vLenX) / 2) +
		fabsf(XMVector3Dot(Separate, XMLoadFloat3(&_boxB.vAxisY)).m128_f32[0] * (_boxB.vLenY) / 2) +
		fabsf(XMVector3Dot(Separate, XMLoadFloat3(&_boxB.vAxisZ)).m128_f32[0] * (_boxB.vLenZ) / 2);

	if (fDistance > ShadowA + ShadowB)
		return false;
	return true;
}

void cOBB::CalculateBoundingBox(const Mesh3D & _mesh)
{
	XMFLOAT3 m_BoxMin = {0,0,0};		//�ŏ��l
	XMFLOAT3 m_BoxMax = {0,0,0};		//�ő�l

	HRESULT hr;		//�G���[����
	//�e���_�𒲂ׂ�AABB���쐬����
	int VertexNum = -1;		//���_��
	//���_�����m�F����
	for (int i = 0; i < _mesh.vMaterialCnt; i++) {
		for (int j = 0; j < _mesh.vpMaterial[i].dwNumFace * 3; j++) {
			//���傫�Ȓ��_��������
			if (VertexNum < _mesh.ppiIndex[i][j]) {
				VertexNum = _mesh.ppiIndex[i][j];
			}
		}
	}

	for (int i = 0; i < VertexNum; i++) {
		//BOX�쐬�̍���
		if (_mesh.vpVertexData[i].vPos.x < m_BoxMin.x) m_BoxMin.x = _mesh.vpVertexData[i].vPos.x;
		if (_mesh.vpVertexData[i].vPos.x > m_BoxMax.x) m_BoxMax.x = _mesh.vpVertexData[i].vPos.x;

		if (_mesh.vpVertexData[i].vPos.y < m_BoxMin.y) m_BoxMin.y = _mesh.vpVertexData[i].vPos.y;
		if (_mesh.vpVertexData[i].vPos.y > m_BoxMax.y) m_BoxMax.y = _mesh.vpVertexData[i].vPos.y;

		if (_mesh.vpVertexData[i].vPos.z < m_BoxMin.z) m_BoxMin.z = _mesh.vpVertexData[i].vPos.z;
		if (_mesh.vpVertexData[i].vPos.z > m_BoxMax.z) m_BoxMax.z = _mesh.vpVertexData[i].vPos.z;
	}

	//���S���W
	m_Info.vCenter = { (m_BoxMax.x + m_BoxMin.x) / 2.0f, (m_BoxMax.y + m_BoxMin.y) / 2.0f ,(m_BoxMax.z + m_BoxMin.z) / 2.0f };
	m_Info.vPos = m_Info.vCenter;

	//�傫�����Z�b�g����
	m_Info.vLenX = fabsf(m_BoxMax.x - m_BoxMin.x);
	m_Info.vLenY = fabsf(m_BoxMax.y - m_BoxMin.y);
	m_Info.vLenZ = fabsf(m_BoxMax.z - m_BoxMin.z);

	//�O�����Z�b�g
	m_Info.vAxisX = { 1,0,0 };
	m_Info.vAxisY = { 0,1,0 };
	m_Info.vAxisZ = { 0,0,1 };
}
