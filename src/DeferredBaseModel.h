/*=================================================
//								FILE : DeferredBaseModel.h
//		�t�@�C������ :
//		�f�B�t�@�[�h�����_�����O�ŗ��p���郂�f���f�[�^���I�Ɏ�舵��
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include <string>
#include "Mesh3D.h"
#include "MatrixCoord.h"
#include "DeferredConst.h"
#include "DeferredRendering.h"
#include "Tessellation.h"

/// <summary>
/// ��{�I�ɏ�������LoadData�A�X�V�������s���Ă���
/// DrawMesh���Ăяo����OK�B
/// ���f���ɂ���Ď�������ݒ�
/// </summary>
class cDeferredModel : public cMatrix {
public:
	cDeferredModel();
	~cDeferredModel() = default;
	void LoadData(std::string _fileName, int _type);	//type��ShaderType���r�b�g���Z����
	void DrawMesh(int _anmNo = 0, const float _time = 0);
	void SetEmission(DirectX::XMFLOAT4 _color);	//�������F��ݒ肷��
	void SetShaderType(int _type);	//type��ShaderType���r�b�g���Z����Ƃ���
	void LoadTessellation(std::string _FileName);
	const Mesh3D& GetMeshData()const;
	static void DefaultRenderFlag(bool flag);	//true : �ʏ탌���_�����O false : �V���h�E�����_�����O
	void AddFunctionToLua(lua_State* L, std::string LuaVarName);
private:
	void SetConstant();		//�ۑ����ꂽ�f�[�^��萔�o�b�t�@�ɐݒ肷��

	static int DrawGlue(lua_State* L);
	std::shared_ptr<Mesh3D> m_pMesh;
	std::shared_ptr<TessellationStruct> m_pTesse;
	cDeferredConst m_Constant;		//�萔�o�b�t�@
	int m_ShaderType;						//�r�b�g���Z�����V�F�[�_�ԍ����i�[����

	static bool m_DefaultRenderFlag;		//�ʏ탌���_�����O�t���O�B�V���h�E�}�b�v�쐬���Ƀe�N�X�`�����Z�b�g���Ȃ��Ǘ��Ɏg�p
};