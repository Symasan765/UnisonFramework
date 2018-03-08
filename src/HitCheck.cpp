/*=================================================
//								FILE : HitCheck.h
//		ファイル説明 :
//		当たり判定を行う関数をまとめた
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "HitCheck.h"
#include	<math.h>

using namespace DirectX;

/// <summary>
/// 球同士の当たり判定
/// </summary>
/// <param name="p1">球1座標</param>
/// <param name="r1">半径１</param>
/// <param name="p2">球２座標</param>
/// <param name="r2">半径２</param>
/// <returns>当たっているか？</returns>
bool HitCheck::HitCheckSphere(const DirectX::XMFLOAT3 & p1, const float r1, const DirectX::XMFLOAT3 & p2, const float r2)
{
	//当たり判定だけなので正確な距離を測るsqrtfは除外する
	float len = (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y) + (p2.z - p1.z) * (p2.z - p1.z);

	if (len <= (r1 + r2) * (r1 + r2)) return true;

	return false;
}

/// <summary>
/// AABBと線分での当たり判定を行う関数
/// </summary>
/// <param name="_BoxMin"></param>
/// <param name="_BoxMax"></param>
/// <param name="_LineStart"></param>
/// <param name="_LineEnd"></param>
/// <returns></returns>
bool HitCheck::AABBandLine(const DirectX::XMFLOAT3 & _BoxMin, const DirectX::XMFLOAT3 & _BoxMax, const DirectX::XMFLOAT3 & _LineStart, const DirectX::XMFLOAT3 & _LineEnd)
{
	//線分のベクトルを求めておく
	XMFLOAT3 Vec = { _LineEnd.x - _LineStart.x, _LineEnd.y - _LineStart.y ,_LineEnd.z - _LineStart.z };
	XMFLOAT3 Pos = _LineStart;

	//結果格納用
	float t_min = 0, t_max = 0;


	//==============X軸領域での判定==================
	float tx_min = 0, tx_max = 0;
	//ベクトルが0でかつ始点が範囲外にあれば絶対に当たらない
	if (Vec.x == 0.0f) {
		if ((Pos.x < _BoxMin.x) || (Pos.x > _BoxMax.x)) return false;
		tx_min = 0.0f;
		tx_max = 1.0f;
	}
	else {
		//X軸での領域を決定する
		float t0 = (_BoxMin.x - Pos.x) / Vec.x;	//Vecはif文で0ではないことが保証されている
		float t1 = (_BoxMax.x - Pos.x) / Vec.x;
		if (t0 < t1) {
			tx_min = t0;
			tx_max = t1;
		}
		else {
			tx_min = t1;
			tx_max = t0;
		}
		//X軸内で領域外に出ているかチェックする
		if ((tx_max < 0.0f) || (tx_min > 1.0f)) return false;	//X軸が一切当たっていないので範囲外
	}
	t_min = tx_min;
	t_max = tx_max;

	//==============Y軸領域での判定==================
	float ty_min = 0, ty_max = 0;
	//ベクトルが0でかつ始点が範囲外にあれば絶対に当たらない
	if (Vec.y == 0.0f) {
		if ((Pos.y < _BoxMin.y) || (Pos.y > _BoxMax.y)) return false;
		ty_min = 0.0f;
		ty_max = 1.0f;
	}
	else {
		//Y軸での領域を決定する
		float t0 = (_BoxMin.y - Pos.y) / Vec.y;	//Vecはif文で0ではないことが保証されている
		float t1 = (_BoxMax.y - Pos.y) / Vec.y;
		if (t0 < t1) {
			ty_min = t0;
			ty_max = t1;
		}
		else {
			ty_min = t1;
			ty_max = t0;
		}
		//Y軸内で領域外に出ているかチェックする
		if ((ty_max < 0.0f) || (ty_min > 1.0f)) return false;	//Y軸が一切当たっていないので範囲外
	}
	if ((t_max < ty_min) || (t_min > ty_max)) return false;		//X軸とY軸がかぶっていない
	if (t_min < ty_min) t_min = ty_min;
	if (t_max > ty_max) t_max = ty_max;


	//==============Z軸領域での判定==================
	float tz_min = 0, tz_max = 0;
	//ベクトルが0でかつ始点が範囲外にあれば絶対に当たらない
	if (Vec.z == 0.0f) {
		if ((Pos.z < _BoxMin.z) || (Pos.z > _BoxMax.z)) return false;
		tz_min = 0.0f;
		tz_max = 1.0f;
	}
	else {
		//Z軸での領域を決定する
		float t0 = (_BoxMin.z - Pos.z) / Vec.z;	//Vecはif文で0ではないことが保証されている
		float t1 = (_BoxMax.z - Pos.z) / Vec.z;
		if (t0 < t1) {
			tz_min = t0;
			tz_max = t1;
		}
		else {
			tz_min = t1;
			tz_max = t0;
		}
		//Z軸内で領域外に出ているかチェックする
		if ((tz_max < 0.0f) || (tz_min > 1.0f)) return false;	//Z軸が一切当たっていないので範囲外
	}
	if ((t_max < tz_min) || (t_min > tz_max)) return false;		//X軸とY軸がかぶっていない
	if (t_min < tz_min) t_min = tz_min;
	if (t_max > tz_max) t_max = tz_max;

	//共通領域のチェック
	if ((t_min > 1.0f) || (t_max < 0.0f)) return false;		//範囲外は当たってない
	else return true;													//範囲内なら当たった

	return false;
}

/// <summary>
/// 三角形から平面を作成する
/// </summary>
/// <param name="pos1">座標1</param>
/// <param name="pos2">座標2</param>
/// <param name="pos3">座標3</param>
/// <returns>x = a,y = b, z = c, w = dで格納した平面情報を返す</returns>
DirectX::XMFLOAT4 HitCheck::CreateTriangleToPlane(const DirectX::XMFLOAT3 & pos1, const DirectX::XMFLOAT3 & pos2, const DirectX::XMFLOAT3 & pos3)
{
	XMVECTOR p1p2;
	XMVECTOR p2p3;

	XMVECTOR normal;
	XMFLOAT3 n;	//平面情報を一時的に格納する

	XMFLOAT4 plane;		//ここに情報を格納して返す
	

	p1p2 = XMLoadFloat3(&pos2) - XMLoadFloat3(&pos1);
	p2p3 = XMLoadFloat3(&pos3) - XMLoadFloat3(&pos2);

	normal = XMVector3Cross(p1p2, p2p3);		//外積を求める
	normal = XMVector3Normalize(normal);		//今後の計算上、正規化しなければバグが出る
	XMStoreFloat3(&n, normal);

	plane.x = n.x;
	plane.y = n.y;
	plane.z = n.z;

	plane.w = -(plane.x * pos1.x + plane.y * pos1.y + plane.z * pos1.z);

	return plane;
}

/// <summary>
/// 線分と平面の当たり判定を行う
/// </summary>
/// <param name="plane">平面の方程式</param>
/// <param name="origin">線分の起点</param>
/// <param name="vec">線分のベクトル</param>
/// <param name="out">交点を格納する</param>
/// <returns>true : 当たった</returns>
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
/// 指定座標が三角形の内部かどうかを判定する
/// </summary>
/// <param name="p1">三角形座標1</param>
/// <param name="p2">三角形座標2</param>
/// <param name="p3">三角形座標3</param>
/// <param name="CheckPoint">調べる座標</param>
/// <returns>true : 中</returns>
bool HitCheck::CheckInTriangle(const DirectX::XMFLOAT3 & a, const DirectX::XMFLOAT3 & b, const DirectX::XMFLOAT3 & c, const DirectX::XMFLOAT3 & p)
{
	XMVECTOR ab, bc, ca;		//三辺のベクトル
	XMVECTOR ap, bp, cp;		//内部の点とのベクトル
	XMVECTOR normal;			//三角形の法線ベクトル
	XMVECTOR n1, n2, n3;		//三辺の内部の点との法線ベクトル

	ab = XMLoadFloat3(&b) - XMLoadFloat3(&a);
	bc = XMLoadFloat3(&c) - XMLoadFloat3(&b);
	ca = XMLoadFloat3(&a) - XMLoadFloat3(&c);

	ap = XMLoadFloat3(&p) - XMLoadFloat3(&a);
	bp = XMLoadFloat3(&p) - XMLoadFloat3(&b);
	cp = XMLoadFloat3(&p) - XMLoadFloat3(&c);

	normal = XMVector3Cross(ab, bc);		//外積を計算

	n1 = XMVector3Cross(ab, ap);
	n2 = XMVector3Cross(bc, bp);
	n3 = XMVector3Cross(ca, cp);

	if (XMVector3Dot(n1, normal).m128_f32 < 0) return false;
	if (XMVector3Dot(n2, normal).m128_f32 < 0) return false;
	if (XMVector3Dot(n3, normal).m128_f32 < 0) return false;
	return true;
}
