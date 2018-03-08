/*=================================================
//								FILE : FontDraw.cpp
//		�t�@�C������ :
//		�����t�H���g�̕\������������
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "FontDraw.h"
#include "WinMain.h"
#include "DirectX11.h"
#include "LuaConvert.h"
using namespace LuaConv;

/// <summary>
/// �t�H���g�`��N���X�̏�����
/// </summary>
void cFontDraw::Draw()
{
	GetDirectX::Context()->IASetInputLayout(NULL);
	GetDirectX::Context()->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	GetDirectX::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	GetDirectX::Context()->VSSetShader(NULL, NULL, 0);
	GetDirectX::Context()->GSSetShader(NULL, NULL, 0);
	GetDirectX::Context()->PSSetShader(NULL, NULL, 0);
	
	m_batch->Begin();

	//�i�[���������f�[�^��`��
	for (auto itr = m_drawData.begin(); itr != m_drawData.end(); ++itr) {
		//�t�H���g���ǂݍ��܂�Ă��邩�`�F�b�N
		auto itrt = m_fontData.find(itr->vFont);

		if (itrt != m_fontData.end()) {
			//�ǂݍ��܂�Ă���I
			m_fontData[itr->vFont]->DrawString(m_batch.get(), itr->vText.c_str(), DirectX::XMFLOAT2(itr->vPosX, itr->vPosY), { itr->vColor.x,itr->vColor.y,itr->vColor.z,1.0f}, 0.0f, DirectX::XMFLOAT2{ 0.0f,0.0f }, itr->vScale);
		}
		else {
			//�ǂݍ��܂�Ă��Ȃ�
			m_fontData[itr->vFont] = std::make_unique<DirectX::SpriteFont>(GetDirectX::Device(), GetFileName(itr->vFont));
			m_fontData[itr->vFont]->DrawString(m_batch.get(), itr->vText.c_str(), DirectX::XMFLOAT2(itr->vPosX, itr->vPosY), { itr->vColor.x,itr->vColor.y,itr->vColor.z,1.0f }, 0.0f, DirectX::XMFLOAT2{ 0.0f,0.0f }, itr->vScale);
		}
	}
	m_batch->DirectX::SpriteBatch::End();

	m_drawData.erase(m_drawData.begin(), m_drawData.end());
}

/// <summary>
/// �������
/// </summary>
cFontDraw::~cFontDraw() {

}

/// <summary>
/// �e�L�X�g�f�[�^��`��p�o�b�t�@�Ɋi�[���Ă����B����̓��[�v�̍Ō�ɂ܂Ƃ߂ĕ`�悳���
/// </summary>
/// <param name="_drawText">�`��p�e�L�X�g</param>
/// <param name="_x">�X�N���[���ʒuX</param>
/// <param name="_y">�X�N���[���ʒuY</param>
/// <param name="_fontName">�t�H���g���񋓑�</param>
/// <param name="scale">�{��</param>
/// <param name="Color">�J���[</param>
void cFontDraw::TextDraw(std::string _drawText, const int _x, const int _y, FontName _fontName, float scale, DirectX::XMFLOAT3 Color)
{
	std::wstring text = StringToWString(_drawText);
	//�`�悷��f�[�^��ǉ�
	m_drawData.push_back({ text, _x, _y, _fontName,scale ,Color });
}

const wchar_t* cFontDraw::GetFileName(FontName _Enum)
{
	//�w��̕������ԋp����
	return m_fontMap[_Enum].c_str();
}

void cFontDraw::AddFunctionToLua(lua_State * L, std::string LuaVarName)
{
	const luaL_Reg funcs[] = {
		{ "Draw", cFontDraw::TextDrawGlue },
		{ NULL, NULL }
	};

	SetLuaGlobalObjct<cFontDraw>(L, LuaVarName.c_str(), funcs, this);
}

cFontDraw::cFontDraw() {
	m_batch = std::make_unique<DirectX::SpriteBatch>(GetDirectX::Context());
	//=================�܂��e�t�H���g����enum�ɓo�^���Ă���========================
	m_fontMap[FontName::Mplus] = L"font/Mplus.spritefont";
	m_fontMap[FontName::MSGothic] = L"font/MGothic.spritefont";
	m_fontMap[FontName::KokuGothic] = L"font/KokuGothic.spritefont";
	m_fontMap[FontName::TeraMin] = L"font/GenEiLateMinT.spritefont";
	//=====================================================================

	//�Ƃ肠�������߂�M+�����ǂݍ���
	m_fontData[FontName::Mplus] = std::make_unique<DirectX::SpriteFont>(GetDirectX::Device(), GetFileName(FontName::Mplus));
	m_fontData[FontName::MSGothic] = std::make_unique<DirectX::SpriteFont>(GetDirectX::Device(), GetFileName(FontName::MSGothic));

	m_batch->SetViewport(cDirectX11::GetInstance()->GetViewPort());
	m_state = std::make_unique<DirectX::CommonStates>(GetDirectX::Device());
}

std::wstring cFontDraw::StringToWString(std::string oString){
	// SJIS �� wstring
	int iBufferSize = MultiByteToWideChar(CP_ACP, 0, oString.c_str()
		, -1, (wchar_t*)NULL, 0);

	// �o�b�t�@�̎擾
	wchar_t* cpUCS2 = new wchar_t[iBufferSize];

	// SJIS �� wstring
	MultiByteToWideChar(CP_ACP, 0, oString.c_str(), -1, cpUCS2
		, iBufferSize);

	// string�̐���
	std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);

	// �o�b�t�@�̔j��
	delete[] cpUCS2;

	// �ϊ����ʂ�Ԃ�
	return(oRet);
}

int cFontDraw::TextDrawGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cFontDraw>(L);

	//�������擾���Ă���
	std::string text = lua_tostring(L, 2);
	float x = (float)lua_tonumber(L, 3);
	float y = (float)lua_tonumber(L, 4);

	//�֐����s
	p->TextDraw(text,x,y);
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	return 0;		//�߂�l
}
