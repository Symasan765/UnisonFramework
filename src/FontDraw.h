/*=================================================
//								FILE : FontDraw.h
//		�t�@�C������ :
//		DirectXTK��p�����t�H���g�\���@�\�������N���X
//		�������̃t�@�C�����C���N���[�h���ăG���[���o��ꍇ�A
//		�C���N���[�h��擪�s�ōs�����ƂŎ���ꍇ������
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once

//DirectXTK
#include <SpriteFont.h>
#include <CommonStates.h>
#include <memory>
#include <map>
#include <string>
#include <vector>
#include "LuaActor.h"

//�V���O���g���Ŏ���������
//�@�����������������Ђ�����n���Ă���(�f�[�^���X�g�b�N���Ă���)
//�A�n���ꂽ�t���[���̎��̍ł��Ō�ɂ܂Ƃ߂ĕ�����`��(�X�g�b�N����Ă���������DrawString�����[�v������)

/// <summary>
/// �t�H���g�̖��O��ێ�����񋓑�
/// </summary>
enum class FontName {
	Mplus = 0,					//L"font/Mplus.spritefont"
	MSGothic,					//L"font/MGothic.spritefont"
	KokuGothic,				//L"font/KokuGothic.spritefont"
	TeraMin,					//L"GenEiLateMinT.spritefont"
};

struct FontDrawData {
	std::wstring vText;	//�`�悷��e�L�X�g
	int vPosX, vPosY;		//�`��ʒu
	FontName vFont;		//�`��t�H���g
	float vScale;			//�k����
	DirectX::XMFLOAT3 vColor;	//�F
};

/// <summary>
/// �t�H���g�\���N���X(2017/8/20���� ���R�x���Ȃ����߃f�o�b�O�ŗ��p���邱�Ƃ�O��)
/// </summary>
class cFontDraw {
public:
	//�X�V���������������s�����߂�cDirectX�N���X���t�����h�����Ă���
	friend class cSceneManager;
	cFontDraw(const cFontDraw&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cFontDraw& operator=(const cFontDraw&) = delete;	//�R�s�[������Z�q�� delete �w��
	cFontDraw(cFontDraw&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cFontDraw& operator=(cFontDraw&&) = delete;		//���[�u������Z�q�� delete �w��

	void TextDraw(std::string _drawText, const int _x, const int _y, FontName _fontName = FontName::Mplus, float scale = 1.0f, DirectX::XMFLOAT3 Color = {1.0f,1.0f,1.0f});	//�e������`�悷��֐�

	static cFontDraw* getInstance() {
		static cFontDraw inst;			// private�ȃR���X�g���N�^���Ăяo��
		return &inst;
	}

	void AddFunctionToLua(lua_State* L, std::string LuaVarName = "Text");
private:
	cFontDraw();					//�R���X�g���N�^�� private �ɒu��
	~cFontDraw();					//�f�X�g���N�^�� private �ɒu��
	void Draw();
	const wchar_t* GetFileName(FontName);
	std::wstring StringToWString(std::string oString);
	static int TextDrawGlue(lua_State* L);

	std::unique_ptr<DirectX::SpriteBatch> m_batch;

	std::map<FontName, std::wstring> m_fontMap;				//�t�H���g������t�H���g���Ǘ�����
	std::map<FontName, std::unique_ptr<DirectX::SpriteFont>> m_fontData;				//�t�H���g������t�H���g���Ǘ�����
	std::vector<FontDrawData> m_drawData;	//�`�悷��f�[�^��ێ�����

	std::unique_ptr<DirectX::CommonStates> m_state;
};