#include "ToonParam.h"

cToonParam::cToonParam()
{
	m_LuaAct = new cLuaActor("ShaderParam/ToonParam.lua");
	m_LuaAct->ScriptCommit();

	ToonParamUpdate();	
}

cToonParam::~cToonParam()
{
	delete m_LuaAct;
}

void cToonParam::SetLightConstantBuffer(const int PathNo)
{
	m_LightParam.Set(PathNo);
}

void cToonParam::SetOutLineConstantBuffer(const int PathNo)
{
	m_OutLineParam.Set(PathNo);
}

void cToonParam::ToonParamUpdate()
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
