/*=================================================
//								FILE : MatrixCoord.h
//		�t�@�C������ :
//		�s��v�Z���s���N���X���`����B
//		�v�Z���s���ۂɂ�DirectXMath�𗘗p����
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once

#include "WinMain.h"
#include <lua.hpp>
#include <string>

//�ϊ��p�s��
typedef struct {
	DirectX::XMFLOAT4X4 mtxWorld;	//���[���h�s��
}MATRIX3D;


class cMatrix {
public:
	cMatrix();
	virtual ~cMatrix() = default;
	/*===�g��k���E�ړ��E��]===*/
	virtual void Scaling(const float);
	virtual void Scaling(const float, const float, const float);
	virtual void Translation(const DirectX::XMFLOAT3);
	virtual void Rotation(const float, const float, const float);
	//	/*===�g��k���E�ړ��E��]�̏�����===*/
	virtual void SetScaling(const float);
	virtual void SetRotate(const float, const float, const float);
	void SetTrans(const DirectX::XMFLOAT3);
	void SetMatrix(const DirectX::XMFLOAT4X4);
	//	/*===���[���h�s��֌v�Z���s��(����c�����낤���c)===*/
	//	void MatrixCalculation();
	void SetRotateMatrix(const DirectX::XMFLOAT4X4 rotMat);
	//	/*===�����Ă�������ւ̈ړ�===*/
	virtual void VectMove(const float);
	void WorldRotation(const float, const float, const float);		//���[���h������ɉ�]������B���_�ړ����Ȃ�
//	/*===�e�s��̃|�C���^�擾===*/
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
	DirectX::XMFLOAT3 m_ScaleRate;		//���݂̊g�k����ێ�
	MATRIX3D mtxDate;	//�\���̌Q

	//======================== Lua�pGlue�֐� ===============================
	static int ScalingGlue(lua_State* L);
	static int TranslationGlue(lua_State* L);
	static int RotationGlue(lua_State* L);
	static int SetScalingGlue(lua_State* L);
	static int SetRotateGlue(lua_State* L);
	static int SetTransGlue(lua_State* L);
	static int VectMoveGlue(lua_State* L);
	static int GetPositionGlue(lua_State* L);
};