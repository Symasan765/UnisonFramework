#include "DepthOfField.h"

cDepthOfField::cDepthOfField() : cPostEffects("DepthOfField.hlsl")
{
	m_GauseBlue = new cGaussBlur;
	m_pBufRenderTarget = new cRenderTargetTex;
	m_pBufRenderTarget->RTCreate((float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y, { 0,0,0 });

	m_LuaAct = new cLuaActor("ShaderParam/DepthOfField.lua");
	m_LuaAct->ScriptCommit();
}

cDepthOfField::~cDepthOfField()
{
	delete m_pBufRenderTarget;
	delete m_GauseBlue;
}


void cDepthOfField::DrawDOF(ID3D11ShaderResourceView * _color,ID3D11ShaderResourceView * _normal, const DoFData& data)
{
	m_GauseBlue->DrawGaussBlur(_color);
	
	m_LuaAct->CallFunc("DepthOfFieldParam",2);
	m_ShaderConst.data = data;
	m_ShaderConst.data.vBlur = m_LuaAct->m_Ret.GetNumber(0);
	m_ShaderConst.data.vDoFRange = m_LuaAct->m_Ret.GetNumber(1);
	ID3D11ShaderResourceView* Tex[] = {
		_color,m_GauseBlue->GetResourceView(),_normal
	};
	this->SetTexSamplerState(0, LINEAR);

	m_pBufRenderTarget->SetRenderTargetTex();
	m_ShaderConst.Set(1);
	this->Draw(Tex, 3);
}

ID3D11ShaderResourceView * cDepthOfField::GetResourceView()
{
	return m_pBufRenderTarget->GetTextureResourceView();
}
