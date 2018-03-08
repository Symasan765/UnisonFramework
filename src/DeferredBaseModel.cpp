/*=================================================
//								FILE : DeferredBaseModel.cpp
//		�t�@�C������ :
//		�f�B�t�@�[�h�����_�����O�ŗ��p���邱�Ƃ�z�肵��
//		���b�V�����f������舵���N���X
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "DeferredBaseModel.h"
#include "LuaConvert.h"
using namespace LuaConv;

bool cDeferredModel::m_DefaultRenderFlag = false;

/// <summary>
/// ������
/// </summary>
cDeferredModel::cDeferredModel()
{
	//����������
	m_ShaderType = 0;
	m_Constant.SetWorldMatrix(GetWorldMatrix());
	m_pTesse = nullptr;
}

/// <summary>
/// ���b�V���f�[�^�����[�h����
/// </summary>
/// <param name="_fileName"></param>
/// <param name="_type"></param>
void cDeferredModel::LoadData(std::string _fileName, int _type)
{
	//���b�V���ƃV�F�[�_�����[�h����
	m_pMesh = MeshManager::GetInstance().LoadMeshData(_fileName);
	m_ShaderType = _type;		//�n���ꂽ�l�𐮐��l�Ƃ��ĕۑ�
}

/// <summary>
/// ���b�V���f�[�^��`�悷��
/// </summary>
/// <param name="_anmNo"></param>
/// <param name="_time"></param>
void cDeferredModel::DrawMesh(int _anmNo, const float _time)
{
	//m_pMesh->Draw();		//�W���̕`��g���ƃV�F�[�_�Z�b�g���o���Ȃ�����g��Ȃ��������ȁc�H

	SetConstant();

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g�i�o�[�e�b�N�X�o�b�t�@�[�͈�ł����j
	UINT stride = sizeof(VERTEX3D);
	UINT offset = 0;
	GetDirectX::Context()->IASetVertexBuffers(0, 1, &m_pMesh->vVertexBuf, &stride, &offset);

	//	�A�j���[�V�������������Ă�����V�F�[�_�[�ɃZ�b�g����
	if (m_pMesh->vpAnimetion != nullptr) {
		this->m_pMesh->vpAnimetion->SetBoneConst(_anmNo, _time);
		// ���̊֐����̂Ƀf�t�H���g�����ɂăA�j���[�V�����ԍ����擾�ł���悤�ɂ��Ă���
		//�^�C�}�[��ݒ�ł���悤�ɂ��Ă����������ōs���B
	}

	//�}�e���A���̐������A���ꂼ��̃}�e���A���̃C���f�b�N�X�o�b�t�@�|��`��
	for (int i = 0; i < m_pMesh->vMaterialCnt; i++)
	{
		//�}�e���A�������Z�b�g����
		m_Constant.SetMaterial(m_pMesh->vpMaterial[i].Ka, m_pMesh->vpMaterial[i].Kd, m_pMesh->vpMaterial[i].Ks);

		//�g�p����Ă��Ȃ��}�e���A���΍�
		if (m_pMesh->vpMaterial[i].dwNumFace == 0)
		{
			continue;
		}

		GetDirectX::Context()->IASetIndexBuffer(m_pMesh->vppIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

		//�e�N�X�`���[���V�F�[�_�[�ɓn��
		if (m_pMesh->vpMaterial[i].pDefaultTex != NULL)
		{
			//�ʏ탌���_�����O���̂݃e�N�X�`�����Z�b�g����B�[�x�}�b�v�쐬���͕s�v
			if (m_DefaultRenderFlag) {
				GetDirectX::Context()->PSSetShaderResources(0, 1, &m_pMesh->vpMaterial[i].pDefaultTex->vpTexture);
				GetDirectX::Context()->PSSetShaderResources(1, 1, &m_pMesh->vpMaterial[i].pShadowTex->vpTexture);
				GetDirectX::Context()->PSSetShaderResources(2, 1, &m_pMesh->vpMaterial[i].pLightTex->vpTexture);
				GetDirectX::Context()->PSSetShaderResources(3, 1, &m_pMesh->vpMaterial[i].pMaterialMaskTex->vpTexture);
			}
			else {
				GetDirectX::Context()->PSSetShaderResources(NULL, NULL, NULL);
			}
		}

		m_Constant.SetShader(true);		//�}�e���A�����萔���V�F�[�_�̃Z�b�g����

										//�e�b�Z���[�V����������ꍇ�����Z�b�g����
		if (m_pTesse) {
			m_pTesse->SetTessellation();
			//�v���~�e�B�u�E�g�|���W�[���Z�b�g
			GetDirectX::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		}
		else {
			cTessellationManager::GetInstance().SetTessellationToNull();
			//�v���~�e�B�u�E�g�|���W�[���Z�b�g
			GetDirectX::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		//Draw
		GetDirectX::Context()->DrawIndexed(m_pMesh->vpMaterial[i].dwNumFace * 3, 0, 0);
	}
	GetDirectX::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cTessellationManager::GetInstance().SetTessellationToNull();
}

/// <summary>
/// ���������Z�b�g
/// </summary>
/// <param name="_color"></param>
void cDeferredModel::SetEmission(DirectX::XMFLOAT4 _color)
{
	m_Constant.SetEmission(_color);
}

void cDeferredModel::SetShaderType(int _type)
{
	m_ShaderType = _type;		//�n���ꂽ�l�𐮐��l�Ƃ��ĕۑ�
}

/// <summary>
/// �e�b�Z���[�V�����X�e�[�W���g�p����ꍇ�ǉ��œǂݍ���
/// </summary>
/// <param name="_FileName"></param>
void cDeferredModel::LoadTessellation(std::string _FileName)
{
	m_pTesse = cTessellationManager::GetInstance().LoadTessellationData(_FileName);
}

const Mesh3D & cDeferredModel::GetMeshData() const
{
	return *m_pMesh;
}

void cDeferredModel::DefaultRenderFlag(bool flag)
{
	m_DefaultRenderFlag = flag;
}

/// <summary>
/// Lua�Ƀo�C���h����
/// </summary>
/// <param name="L"></param>
/// <param name="LuaVarName"></param>
void cDeferredModel::AddFunctionToLua(lua_State * L, std::string LuaVarName)
{
	const luaL_Reg funcs[] = {
		{ "Scaling", cMatrix::ScalingGlue },
		{ "Translation", cMatrix::TranslationGlue },
		{ "Rotation", cMatrix::RotationGlue },
		{ "SetScaling", cMatrix::SetScalingGlue },
		{ "SetRotate", cMatrix::SetRotateGlue },
		{ "SetTrans", cMatrix::SetTransGlue },
		{ "VectMove", cMatrix::VectMoveGlue },
		{ "GetPosition", cMatrix::GetPositionGlue },
		{ "Draw", cDeferredModel::DrawGlue },
		{ NULL, NULL }
	};

	SetLuaGlobalObjct<cDeferredModel>(L, LuaVarName.c_str(), funcs, this);
}

/// <summary>
/// �萔���V�F�[�_�ɃZ�b�g����
/// </summary>
void cDeferredModel::SetConstant()
{
	//���[���h�s���ۑ�
	m_Constant.SetWorldMatrix(GetWorldMatrix());
	m_Constant.SetShaderType(m_ShaderType);
	m_Constant.SetShader();
}

/// <summary>
/// Lua���g�p���邽�߂�Glue�֐�
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cDeferredModel::DrawGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cDeferredModel>(L);


	//�֐����s
	p->DrawMesh();
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	return 0;		//�߂�l
}
