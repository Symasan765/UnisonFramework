/*=================================================
//								FILE : OrientedBoundingBox.h
//		ファイル説明 :
//		
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"
#include "Mesh3D.h"

struct OBBBOX {
	DirectX::XMFLOAT3 vAxisX;		//X軸
	DirectX::XMFLOAT3 vAxisY;		//Y軸
	DirectX::XMFLOAT3 vAxisZ;		//Z軸
	DirectX::XMFLOAT3 vCenter;	//中心座標
	DirectX::XMFLOAT3 vPos;		//現在の位置
	float vLenX;				//BoxのXサイズの半分
	float vLenY;				//BoxのYサイズの半分
	float vLenZ;				//BoxのZサイズの半分
};

/// <summary>
/// OBBによる当たり判定を行うクラス
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
	/// 非衝突 = false, 衝突 = true
	/// </summary>
	/// <param name="_boxA">BoxA</param>
	/// <param name="_boxB">BoxB</param>
	/// <param name="Separate">分離軸</param>
	/// <param name="_Distance">二つのオブジェクトを結ぶベクトル</param>
	/// <returns>衝突か否か</returns>
	bool CompareLength(const OBBBOX& _boxA, const OBBBOX& _boxB, const DirectX::XMFLOAT3& Separate, const DirectX::XMFLOAT3& _Distance);
	void CalculateBoundingBox(const Mesh3D & _mesh);
	OBBBOX m_Info;			//OBB情報
	DirectX::XMFLOAT4X4 m_WorldMat;		//ワールド変換行列
};