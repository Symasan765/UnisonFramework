/*=================================================
//								FILE : HitCheck.h
//		ファイル説明 :
//		当たり判定を行う関数をまとめた
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"

//当たり判定をまとめた名前空間
namespace HitCheck {
	bool HitCheckSphere(const DirectX::XMFLOAT3& p1, const float r1, const DirectX::XMFLOAT3& p2, const float r2);
	bool AABBandLine(const DirectX::XMFLOAT3& _BoxMin, const DirectX::XMFLOAT3& _BoxMax, const DirectX::XMFLOAT3& _LineStart, const DirectX::XMFLOAT3& _LineEnd);		//AABBと線分の当たり判定
	DirectX::XMFLOAT4 CreateTriangleToPlane(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2, const DirectX::XMFLOAT3& pos3);		//三角形から平面を作る
	bool LinetoPlaneCross(const DirectX::XMFLOAT4& plane, const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& vec, DirectX::XMFLOAT3* out);
	bool CheckInTriangle(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2, const DirectX::XMFLOAT3& p3, const DirectX::XMFLOAT3& CheckPoint);	//三角形の内外判定
}
