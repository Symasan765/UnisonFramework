#include "FontDraw.h"
#include "ToonParam.h"
#include "Input.h"

/// <summary>
/// ������
/// </summary>
cToonParam::cToonParam()
{
	m_LuaAct = new cLuaActor("ShaderParam/ToonParam.lua");
	m_LuaAct->ScriptCommit();

	ToonParamInit();	

	m_GuidelinesFlag = false;
}

/// <summary>
/// ���
/// </summary>
cToonParam::~cToonParam()
{
	delete m_LuaAct;
}

/// <summary>
/// �L�[���͂���g�D�[�������_�����O�̃p�����[�^��ύX����
/// </summary>
void cToonParam::ToonParamUpdate()
{
	if (GetKey->Trigger(DIK_SPACE))
		m_GuidelinesFlag = !m_GuidelinesFlag;

	if (m_GuidelinesFlag) {
		LightingParamUpdate();
		OutLineUpdate();
	}

	//���݂̂��ꂼ��̃p�����[�^�l��`��w�肵�Ă���
	DrawGuidelines();
}

/// <summary>
/// ���C�g�����V�F�[�_�ɃZ�b�g����
/// </summary>
/// <param name="PathNo"></param>
void cToonParam::SetLightConstantBuffer(const int PathNo)
{
	m_LightParam.Set(PathNo);
}

/// <summary>
/// �֊s���̃f�[�^���V�F�[�_�ɃZ�b�g����
/// </summary>
/// <param name="PathNo"></param>
void cToonParam::SetOutLineConstantBuffer(const int PathNo)
{
	m_OutLineParam.Set(PathNo);
}

/// <summary>
/// �g�D�[���̃p�����[�^��������������
/// </summary>
void cToonParam::ToonParamInit()
{
	m_LuaAct->CallFunc("LightingParam", 2);
	m_LightParam.data.LightBorder = m_LuaAct->m_Ret.GetNumber(0);
	m_LightParam.data.ShadowBorder = m_LuaAct->m_Ret.GetNumber(1);

	m_LuaAct->CallFunc("CookTorranceParam", 2);
	m_LightParam.data.HighlightSub = m_LuaAct->m_Ret.GetNumber(0);
	m_LightParam.data.HighlightSum = m_LuaAct->m_Ret.GetNumber(1);

	m_LuaAct->CallFunc("OutlineParam", 4);
	m_OutLineParam.data.offsetScall = m_LuaAct->m_Ret.GetNumber(0);
	m_OutLineParam.data.DepthRange = m_LuaAct->m_Ret.GetNumber(1);
	m_OutLineParam.data.NormalRange = m_LuaAct->m_Ret.GetNumber(2);
	m_OutLineParam.data.OutlineSum = m_LuaAct->m_Ret.GetNumber(3);
}

/// <summary>
/// ���C�e�B���O�p�f�[�^�̃p�����[�^���X�V����
/// </summary>
void cToonParam::LightingParamUpdate()
{
	//���l����
	if (GetKey->Press(DIK_T)) {
		m_LightParam.data.LightBorder += 0.01f;
		if (m_LightParam.data.LightBorder > 1.0f)
			m_LightParam.data.LightBorder = 1.0f;
	}
	else if (GetKey->Press(DIK_F)) {
		m_LightParam.data.LightBorder -= 0.01f;
		//�V���h�E�̈��菬�����Ȃ����璲������
		if (m_LightParam.data.LightBorder < m_LightParam.data.ShadowBorder)
			m_LightParam.data.LightBorder = m_LightParam.data.ShadowBorder;
	}

	//�Òl����
	if (GetKey->Press(DIK_Y)) {
		m_LightParam.data.ShadowBorder += 0.01f;
		if (m_LightParam.data.ShadowBorder > m_LightParam.data.LightBorder)
			m_LightParam.data.ShadowBorder = m_LightParam.data.LightBorder;
	}
	else if (GetKey->Press(DIK_G)) {
		m_LightParam.data.ShadowBorder -= 0.01f;
		//�V���h�E�̈��菬�����Ȃ����璲������
		if (m_LightParam.data.ShadowBorder < 0.0f)
			m_LightParam.data.ShadowBorder = 0.0f;
	}
}

/// <summary>
/// �ύX�p�̃K�C�h��\������
/// </summary>
void cToonParam::DrawGuidelines()
{
	//�K�C�h���C��������
	if (m_GuidelinesFlag) {
		//���݂̃p�����[�^��`�悷��
		std::string param[6];
		param[0] = "���� (T or F): " + std::to_string(m_LightParam.data.LightBorder);
		param[1] = "�Õ� (Y or G): " + std::to_string(m_LightParam.data.ShadowBorder);
		param[2] = "���� (U or H): " + std::to_string(m_OutLineParam.data.offsetScall);
		param[3] = "�[�x�͈� (I or J): " + std::to_string(m_OutLineParam.data.DepthRange);
		param[4] = "�@���͈� (O or K): " + std::to_string(m_OutLineParam.data.NormalRange);
		param[5] = "�F�̓���� (P or L): " + std::to_string(m_OutLineParam.data.OutlineSum);

		cFontDraw::getInstance()->TextDraw("�p�����[�^�������[�h", 10, 20, FontName::MSGothic, 1.0f, { (255.0f / 255.0f),(255.0f / 255.0f),(255.0f / 255.0f) });
		for (int i = 0; i < 6; i++) {
			int offsetY = 25;
			cFontDraw::getInstance()->TextDraw(param[i], 20, 20 + offsetY + (i * offsetY), FontName::MSGothic, 0.8f, { (255.0f / 255.0f),(255.0f / 255.0f),(255.0f / 255.0f) });
		}
	}
	//�K�C�h���C����\����
	else {
		cFontDraw::getInstance()->TextDraw("�X�y�[�X�L�[�Ńg�D�[���p�����[�^������", 10, 20, FontName::MSGothic, 1.0f, { (255.0f / 255.0f),(255.0f / 255.0f),(255.0f / 255.0f) });
	}
}

/// <summary>
/// �֊s���̍X�V���s��
/// </summary>
void cToonParam::OutLineUpdate()
{
	//�I�t�Z�b�g�X�P�[���l(�֊s���̕�)
	if (GetKey->Press(DIK_U)) {
		m_OutLineParam.data.offsetScall += 0.005f;
	}
	else if (GetKey->Press(DIK_H)) {
		m_OutLineParam.data.offsetScall -= 0.005f;
		if(m_OutLineParam.data.offsetScall < 0.0f)
			m_OutLineParam.data.offsetScall = 0.005f;
	}

	//�֊s���̈�(�[�x)
	if (GetKey->Press(DIK_I)) {
		m_OutLineParam.data.DepthRange += 0.01f;
	}
	else if (GetKey->Press(DIK_J)) {
		m_OutLineParam.data.DepthRange -= 0.01f;
		if (m_OutLineParam.data.DepthRange < 0.01f)
			m_OutLineParam.data.DepthRange = 0.01f;
	}

	//�֊s���̈�(�@��)
	if (GetKey->Press(DIK_O)) {
		m_OutLineParam.data.NormalRange += 0.01f;
	}
	else if (GetKey->Press(DIK_K)) {
		m_OutLineParam.data.NormalRange -= 0.01f;
		if (m_OutLineParam.data.NormalRange < 0.01f)
			m_OutLineParam.data.NormalRange = 0.01f;
	}

	//�֊s�F�̓���݋
	if (GetKey->Press(DIK_P)) {
		m_OutLineParam.data.OutlineSum += 0.005f;
	}
	else if (GetKey->Press(DIK_L)) {
		m_OutLineParam.data.OutlineSum -= 0.005f;
		if (m_OutLineParam.data.OutlineSum < 0.0f)
			m_OutLineParam.data.OutlineSum = 0.0f;
	}
}
