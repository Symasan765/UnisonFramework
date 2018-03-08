/*=================================================
//								FILE : ToonParam.h
//		�t�@�C������ :
//		�g�D�[�������_�����O�p�̃p�����[�^������舵��
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "LuaActor.h"
#include "ShaderStruct.h"
#include "ConstantBuffer.h"

class cToonParam {
public:
	cToonParam();
	~cToonParam();
	void ToonParamUpdate();
	void SetLightConstantBuffer(const int PathNo);
	void SetOutLineConstantBuffer(const int PathNo);
private:
	void ToonParamInit();
	void LightingParamUpdate();
	void DrawGuidelines();
	void OutLineUpdate();
	cConstBuf<ToonLightingStruct> m_LightParam;
	cConstBuf<OutlineParam> m_OutLineParam;
	cLuaActor* m_LuaAct;


	bool m_GuidelinesFlag;		//�p�����[�^�̃K�C�h���C����ύX���邩�ǂ����̃t���O
};