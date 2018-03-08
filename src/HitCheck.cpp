/*=================================================
//								FILE : HitCheck.h
//		�t�@�C������ :
//		�����蔻����s���֐����܂Ƃ߂�
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "HitCheck.h"
#include	<math.h>

using namespace DirectX;

/// <summary>
/// �����m�̓����蔻��
/// </summary>
/// <param name="p1">��1���W</param>
/// <param name="r1">���a�P</param>
/// <param name="p2">���Q���W</param>
/// <param name="r2">���a�Q</param>
/// <returns>�������Ă��邩�H</returns>
bool HitCheck::HitCheckSphere(const DirectX::XMFLOAT3 & p1, const float r1, const DirectX::XMFLOAT3 & p2, const float r2)
{
	//�����蔻�肾���Ȃ̂Ő��m�ȋ����𑪂�sqrtf�͏��O����
	float len = (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y) + (p2.z - p1.z) * (p2.z - p1.z);

	if (len <= (r1 + r2) * (r1 + r2)) return true;

	return false;
}

/// <summary>
/// AABB�Ɛ����ł̓����蔻����s���֐�
/// </summary>
/// <param name="_BoxMin"></param>
/// <param name="_BoxMax"></param>
/// <param name="_LineStart"></param>
/// <param name="_LineEnd"></param>
/// <returns></returns>
bool HitCheck::AABBandLine(const DirectX::XMFLOAT3 & _BoxMin, const DirectX::XMFLOAT3 & _BoxMax, const DirectX::XMFLOAT3 & _LineStart, const DirectX::XMFLOAT3 & _LineEnd)
{
	//�����̃x�N�g�������߂Ă���
	XMFLOAT3 Vec = { _LineEnd.x - _LineStart.x, _LineEnd.y - _LineStart.y ,_LineEnd.z - _LineStart.z };
	XMFLOAT3 Pos = _LineStart;

	//���ʊi�[�p
	float t_min = 0, t_max = 0;


	//==============X���̈�ł̔���==================
	float tx_min = 0, tx_max = 0;
	//�x�N�g����0�ł��n�_���͈͊O�ɂ���ΐ�΂ɓ�����Ȃ�
	if (Vec.x == 0.0f) {
		if ((Pos.x < _BoxMin.x) || (Pos.x > _BoxMax.x)) return false;
		tx_min = 0.0f;
		tx_max = 1.0f;
	}
	else {
		//X���ł̗̈�����肷��
		float t0 = (_BoxMin.x - Pos.x) / Vec.x;	//Vec��if����0�ł͂Ȃ����Ƃ��ۏ؂���Ă���
		float t1 = (_BoxMax.x - Pos.x) / Vec.x;
		if (t0 < t1) {
			tx_min = t0;
			tx_max = t1;
		}
		else {
			tx_min = t1;
			tx_max = t0;
		}
		//X�����ŗ̈�O�ɏo�Ă��邩�`�F�b�N����
		if ((tx_max < 0.0f) || (tx_min > 1.0f)) return false;	//X������ؓ������Ă��Ȃ��̂Ŕ͈͊O
	}
	t_min = tx_min;
	t_max = tx_max;

	//==============Y���̈�ł̔���==================
	float ty_min = 0, ty_max = 0;
	//�x�N�g����0�ł��n�_���͈͊O�ɂ���ΐ�΂ɓ�����Ȃ�
	if (Vec.y == 0.0f) {
		if ((Pos.y < _BoxMin.y) || (Pos.y > _BoxMax.y)) return false;
		ty_min = 0.0f;
		ty_max = 1.0f;
	}
	else {
		//Y���ł̗̈�����肷��
		float t0 = (_BoxMin.y - Pos.y) / Vec.y;	//Vec��if����0�ł͂Ȃ����Ƃ��ۏ؂���Ă���
		float t1 = (_BoxMax.y - Pos.y) / Vec.y;
		if (t0 < t1) {
			ty_min = t0;
			ty_max = t1;
		}
		else {
			ty_min = t1;
			ty_max = t0;
		}
		//Y�����ŗ̈�O�ɏo�Ă��邩�`�F�b�N����
		if ((ty_max < 0.0f) || (ty_min > 1.0f)) return false;	//Y������ؓ������Ă��Ȃ��̂Ŕ͈͊O
	}
	if ((t_max < ty_min) || (t_min > ty_max)) return false;		//X����Y�������Ԃ��Ă��Ȃ�
	if (t_min < ty_min) t_min = ty_min;
	if (t_max > ty_max) t_max = ty_max;


	//==============Z���̈�ł̔���==================
	float tz_min = 0, tz_max = 0;
	//�x�N�g����0�ł��n�_���͈͊O�ɂ���ΐ�΂ɓ�����Ȃ�
	if (Vec.z == 0.0f) {
		if ((Pos.z < _BoxMin.z) || (Pos.z > _BoxMax.z)) return false;
		tz_min = 0.0f;
		tz_max = 1.0f;
	}
	else {
		//Z���ł̗̈�����肷��
		float t0 = (_BoxMin.z - Pos.z) / Vec.z;	//Vec��if����0�ł͂Ȃ����Ƃ��ۏ؂���Ă���
		float t1 = (_BoxMax.z - Pos.z) / Vec.z;
		if (t0 < t1) {
			tz_min = t0;
			tz_max = t1;
		}
		else {
			tz_min = t1;
			tz_max = t0;
		}
		//Z�����ŗ̈�O�ɏo�Ă��邩�`�F�b�N����
		if ((tz_max < 0.0f) || (tz_min > 1.0f)) return false;	//Z������ؓ������Ă��Ȃ��̂Ŕ͈͊O
	}
	if ((t_max < tz_min) || (t_min > tz_max)) return false;		//X����Y�������Ԃ��Ă��Ȃ�
	if (t_min < tz_min) t_min = tz_min;
	if (t_max > tz_max) t_max = tz_max;

	//���ʗ̈�̃`�F�b�N
	if ((t_min > 1.0f) || (t_max < 0.0f)) return false;		//�͈͊O�͓������ĂȂ�
	else return true;													//�͈͓��Ȃ瓖������

	return false;
}

/// <summary>
/// �O�p�`���畽�ʂ��쐬����
/// </summary>
/// <param name="pos1">���W1</param>
/// <param name="pos2">���W2</param>
/// <param name="pos3">���W3</param>
/// <returns>x = a,y = b, z = c, w = d�Ŋi�[�������ʏ���Ԃ�</returns>
DirectX::XMFLOAT4 HitCheck::CreateTriangleToPlane(const DirectX::XMFLOAT3 & pos1, const DirectX::XMFLOAT3 & pos2, const DirectX::XMFLOAT3 & pos3)
{
	XMVECTOR p1p2;
	XMVECTOR p2p3;

	XMVECTOR normal;
	XMFLOAT3 n;	//���ʏ����ꎞ�I�Ɋi�[����

	XMFLOAT4 plane;		//�����ɏ����i�[���ĕԂ�
	

	p1p2 = XMLoadFloat3(&pos2) - XMLoadFloat3(&pos1);
	p2p3 = XMLoadFloat3(&pos3) - XMLoadFloat3(&pos2);

	normal = XMVector3Cross(p1p2, p2p3);		//�O�ς����߂�
	normal = XMVector3Normalize(normal);		//����̌v�Z��A���K�����Ȃ���΃o�O���o��
	XMStoreFloat3(&n, normal);

	plane.x = n.x;
	plane.y = n.y;
	plane.z = n.z;

	plane.w = -(plane.x * pos1.x + plane.y * pos1.y + plane.z * pos1.z);

	return plane;
}

/// <summary>
/// �����ƕ��ʂ̓����蔻����s��
/// </summary>
/// <param name="plane">���ʂ̕�����</param>
/// <param name="origin">�����̋N�_</param>
/// <param name="vec">�����̃x�N�g��</param>
/// <param name="out">��_���i�[����</param>
/// <returns>true : ��������</returns>
bool HitCheck::LinetoPlaneCross(const DirectX::XMFLOAT4 & plane, const DirectX::XMFLOAT3 & origin, const DirectX::XMFLOAT3 & vec, DirectX::XMFLOAT3* out)
{
	float dot = 0.0f;
	dot = plane.x * vec.x + plane.y * vec.y + plane.z * vec.z;
	if (fabsf(dot) < FLT_EPSILON)
		return false;

	float t = -(plane.x * origin.x + plane.y * origin.y + plane.z * origin.z + plane.w) / dot;
	out->x = origin.x + vec.x * t;
	out->y = origin.y + vec.y * t;
	out->z = origin.z + vec.z * t;

	return true;
}

/// <summary>
/// �w����W���O�p�`�̓������ǂ����𔻒肷��
/// </summary>
/// <param name="p1">�O�p�`���W1</param>
/// <param name="p2">�O�p�`���W2</param>
/// <param name="p3">�O�p�`���W3</param>
/// <param name="CheckPoint">���ׂ���W</param>
/// <returns>true : ��</returns>
bool HitCheck::CheckInTriangle(const DirectX::XMFLOAT3 & a, const DirectX::XMFLOAT3 & b, const DirectX::XMFLOAT3 & c, const DirectX::XMFLOAT3 & p)
{
	XMVECTOR ab, bc, ca;		//�O�ӂ̃x�N�g��
	XMVECTOR ap, bp, cp;		//�����̓_�Ƃ̃x�N�g��
	XMVECTOR normal;			//�O�p�`�̖@���x�N�g��
	XMVECTOR n1, n2, n3;		//�O�ӂ̓����̓_�Ƃ̖@���x�N�g��

	ab = XMLoadFloat3(&b) - XMLoadFloat3(&a);
	bc = XMLoadFloat3(&c) - XMLoadFloat3(&b);
	ca = XMLoadFloat3(&a) - XMLoadFloat3(&c);

	ap = XMLoadFloat3(&p) - XMLoadFloat3(&a);
	bp = XMLoadFloat3(&p) - XMLoadFloat3(&b);
	cp = XMLoadFloat3(&p) - XMLoadFloat3(&c);

	normal = XMVector3Cross(ab, bc);		//�O�ς��v�Z

	n1 = XMVector3Cross(ab, ap);
	n2 = XMVector3Cross(bc, bp);
	n3 = XMVector3Cross(ca, cp);

	if (XMVector3Dot(n1, normal).m128_f32 < 0) return false;
	if (XMVector3Dot(n2, normal).m128_f32 < 0) return false;
	if (XMVector3Dot(n3, normal).m128_f32 < 0) return false;
	return true;
}
