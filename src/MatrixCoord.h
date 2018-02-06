/*=================================================
//								FILE : MatrixCoord.h
//		ファイル説明 :
//		行列計算を行うクラスを定義する。
//		計算を行う際にはDirectXMathを利用する
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once

#include "WinMain.h"
#include <lua.hpp>
#include <string>

//変換用行列
typedef struct {
	DirectX::XMFLOAT4X4 mtxWorld;	//ワールド行列
}MATRIX3D;


class cMatrix {
public:
	cMatrix();
	virtual ~cMatrix() = default;
	/*===拡大縮小・移動・回転===*/
	virtual void Scaling(const float);
	virtual void Scaling(const float, const float, const float);
	virtual void Translation(const DirectX::XMFLOAT3);
	virtual void Rotation(const float, const float, const float);
	//	/*===拡大縮小・移動・回転の初期化===*/
	virtual void SetScaling(const float);
	virtual void SetRotate(const float, const float, const float);
	void SetTrans(const DirectX::XMFLOAT3);
	void SetMatrix(const DirectX::XMFLOAT4X4);
	//	/*===ワールド行列へ計算を行う(これ残すだろうか…)===*/
	//	void MatrixCalculation();
	void SetRotateMatrix(const DirectX::XMFLOAT4X4 rotMat);
	//	/*===向いている方向への移動===*/
	virtual void VectMove(const float);
	void WorldRotation(const float, const float, const float);		//ワールド軸を基準に回転させる。原点移動しない
//	/*===各行列のポインタ取得===*/
	virtual DirectX::XMFLOAT4X4 GetWorldMatrix()const;
	//	DirectX::XMMATRIX GetScaleMatrix();
	//	DirectX::XMMATRIX GetRotatMatrix();
	//	DirectX::XMMATRIX GetTransMatrix();
	DirectX::XMFLOAT3 GetPosition()const;
	DirectX::XMFLOAT3 GetAxisX()const;
	DirectX::XMFLOAT3 GetAxisY()const;
	DirectX::XMFLOAT3 GetAxisZ()const;

	virtual void AddFunctionToLua(lua_State* L, std::string LuaVarName = "transform");
protected:
	DirectX::XMFLOAT3 m_ScaleRate;		//現在の拡縮率を保持
	MATRIX3D mtxDate;	//構造体群

	//======================== Lua用Glue関数 ===============================
	static int ScalingGlue(lua_State* L);
	static int TranslationGlue(lua_State* L);
	static int RotationGlue(lua_State* L);
	static int SetScalingGlue(lua_State* L);
	static int SetRotateGlue(lua_State* L);
	static int SetTransGlue(lua_State* L);
	static int VectMoveGlue(lua_State* L);
	static int GetPositionGlue(lua_State* L);
};