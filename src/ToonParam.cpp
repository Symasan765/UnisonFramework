#include "FontDraw.h"
#include "ToonParam.h"
#include "Input.h"

/// <summary>
/// 初期化
/// </summary>
cToonParam::cToonParam()
{
	m_LuaAct = new cLuaActor("ShaderParam/ToonParam.lua");
	m_LuaAct->ScriptCommit();

	ToonParamInit();	

	m_GuidelinesFlag = false;
}

/// <summary>
/// 解放
/// </summary>
cToonParam::~cToonParam()
{
	delete m_LuaAct;
}

/// <summary>
/// キー入力からトゥーンレンダリングのパラメータを変更する
/// </summary>
void cToonParam::ToonParamUpdate()
{
	if (GetKey->Trigger(DIK_SPACE))
		m_GuidelinesFlag = !m_GuidelinesFlag;

	if (m_GuidelinesFlag) {
		LightingParamUpdate();
		OutLineUpdate();
	}

	//現在のそれぞれのパラメータ値を描画指定しておく
	DrawGuidelines();
}

/// <summary>
/// ライト情報をシェーダにセットする
/// </summary>
/// <param name="PathNo"></param>
void cToonParam::SetLightConstantBuffer(const int PathNo)
{
	m_LightParam.Set(PathNo);
}

/// <summary>
/// 輪郭線のデータをシェーダにセットする
/// </summary>
/// <param name="PathNo"></param>
void cToonParam::SetOutLineConstantBuffer(const int PathNo)
{
	m_OutLineParam.Set(PathNo);
}

/// <summary>
/// トゥーンのパラメータ情報を初期化する
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
/// ライティング用データのパラメータを更新する
/// </summary>
void cToonParam::LightingParamUpdate()
{
	//明値調整
	if (GetKey->Press(DIK_T)) {
		m_LightParam.data.LightBorder += 0.01f;
		if (m_LightParam.data.LightBorder > 1.0f)
			m_LightParam.data.LightBorder = 1.0f;
	}
	else if (GetKey->Press(DIK_F)) {
		m_LightParam.data.LightBorder -= 0.01f;
		//シャドウ領域より小さくなったら調整する
		if (m_LightParam.data.LightBorder < m_LightParam.data.ShadowBorder)
			m_LightParam.data.LightBorder = m_LightParam.data.ShadowBorder;
	}

	//暗値調整
	if (GetKey->Press(DIK_Y)) {
		m_LightParam.data.ShadowBorder += 0.01f;
		if (m_LightParam.data.ShadowBorder > m_LightParam.data.LightBorder)
			m_LightParam.data.ShadowBorder = m_LightParam.data.LightBorder;
	}
	else if (GetKey->Press(DIK_G)) {
		m_LightParam.data.ShadowBorder -= 0.01f;
		//シャドウ領域より小さくなったら調整する
		if (m_LightParam.data.ShadowBorder < 0.0f)
			m_LightParam.data.ShadowBorder = 0.0f;
	}
}

/// <summary>
/// 変更用のガイドを表示する
/// </summary>
void cToonParam::DrawGuidelines()
{
	//ガイドライン調整時
	if (m_GuidelinesFlag) {
		//現在のパラメータを描画する
		std::string param[6];
		param[0] = "明部 (T or F): " + std::to_string(m_LightParam.data.LightBorder);
		param[1] = "暗部 (Y or G): " + std::to_string(m_LightParam.data.ShadowBorder);
		param[2] = "線幅 (U or H): " + std::to_string(m_OutLineParam.data.offsetScall);
		param[3] = "深度範囲 (I or J): " + std::to_string(m_OutLineParam.data.DepthRange);
		param[4] = "法線範囲 (O or K): " + std::to_string(m_OutLineParam.data.NormalRange);
		param[5] = "色の馴染み (P or L): " + std::to_string(m_OutLineParam.data.OutlineSum);

		cFontDraw::getInstance()->TextDraw("パラメータ調整モード", 10, 20, FontName::MSGothic, 1.0f, { (255.0f / 255.0f),(255.0f / 255.0f),(255.0f / 255.0f) });
		for (int i = 0; i < 6; i++) {
			int offsetY = 25;
			cFontDraw::getInstance()->TextDraw(param[i], 20, 20 + offsetY + (i * offsetY), FontName::MSGothic, 0.8f, { (255.0f / 255.0f),(255.0f / 255.0f),(255.0f / 255.0f) });
		}
	}
	//ガイドライン非表示時
	else {
		cFontDraw::getInstance()->TextDraw("スペースキーでトゥーンパラメータ調整へ", 10, 20, FontName::MSGothic, 1.0f, { (255.0f / 255.0f),(255.0f / 255.0f),(255.0f / 255.0f) });
	}
}

/// <summary>
/// 輪郭線の更新を行う
/// </summary>
void cToonParam::OutLineUpdate()
{
	//オフセットスケール値(輪郭線の幅)
	if (GetKey->Press(DIK_U)) {
		m_OutLineParam.data.offsetScall += 0.005f;
	}
	else if (GetKey->Press(DIK_H)) {
		m_OutLineParam.data.offsetScall -= 0.005f;
		if(m_OutLineParam.data.offsetScall < 0.0f)
			m_OutLineParam.data.offsetScall = 0.005f;
	}

	//輪郭線領域(深度)
	if (GetKey->Press(DIK_I)) {
		m_OutLineParam.data.DepthRange += 0.01f;
	}
	else if (GetKey->Press(DIK_J)) {
		m_OutLineParam.data.DepthRange -= 0.01f;
		if (m_OutLineParam.data.DepthRange < 0.01f)
			m_OutLineParam.data.DepthRange = 0.01f;
	}

	//輪郭線領域(法線)
	if (GetKey->Press(DIK_O)) {
		m_OutLineParam.data.NormalRange += 0.01f;
	}
	else if (GetKey->Press(DIK_K)) {
		m_OutLineParam.data.NormalRange -= 0.01f;
		if (m_OutLineParam.data.NormalRange < 0.01f)
			m_OutLineParam.data.NormalRange = 0.01f;
	}

	//輪郭色の馴染み具合
	if (GetKey->Press(DIK_P)) {
		m_OutLineParam.data.OutlineSum += 0.005f;
	}
	else if (GetKey->Press(DIK_L)) {
		m_OutLineParam.data.OutlineSum -= 0.005f;
		if (m_OutLineParam.data.OutlineSum < 0.0f)
			m_OutLineParam.data.OutlineSum = 0.0f;
	}
}
