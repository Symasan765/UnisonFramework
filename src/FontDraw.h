/*=================================================
//								FILE : FontDraw.h
//		ファイル説明 :
//		DirectXTKを用いたフォント表示機能を扱うクラス
//		もしこのファイルをインクルードしてエラーが出る場合、
//		インクルードを先頭行で行うことで治る場合がある
//							HAL大阪 : 松本 雄之介
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

//シングルトンで実装させる
//①代入したい文字列をひたすら渡していく(データをストックしていく)
//②渡されたフレームの時の最も最後にまとめて文字列描画(ストックされていた分だけDrawStringをループさせる)

/// <summary>
/// フォントの名前を保持する列挙対
/// </summary>
enum class FontName {
	Mplus = 0,					//L"font/Mplus.spritefont"
	MSGothic,					//L"font/MGothic.spritefont"
	KokuGothic,				//L"font/KokuGothic.spritefont"
	TeraMin,					//L"GenEiLateMinT.spritefont"
};

struct FontDrawData {
	std::wstring vText;	//描画するテキスト
	int vPosX, vPosY;		//描画位置
	FontName vFont;		//描画フォント
	float vScale;			//縮小率
	DirectX::XMFLOAT3 vColor;	//色
};

/// <summary>
/// フォント表示クラス(2017/8/20現在 自由度がないためデバッグで利用することを前提)
/// </summary>
class cFontDraw {
public:
	//更新処理や解放処理を行うためにcDirectXクラスをフレンド化しておく
	friend class cSceneManager;
	cFontDraw(const cFontDraw&) = delete;			//コピーコンストラクタを delete 指定
	cFontDraw& operator=(const cFontDraw&) = delete;	//コピー代入演算子も delete 指定
	cFontDraw(cFontDraw&&) = delete;			//ムーブコンストラクタを delete 指定
	cFontDraw& operator=(cFontDraw&&) = delete;		//ムーブ代入演算子も delete 指定

	void TextDraw(std::string _drawText, const int _x, const int _y, FontName _fontName = FontName::Mplus, float scale = 1.0f, DirectX::XMFLOAT3 Color = {1.0f,1.0f,1.0f});	//各文字を描画する関数

	static cFontDraw* getInstance() {
		static cFontDraw inst;			// privateなコンストラクタを呼び出す
		return &inst;
	}

	void AddFunctionToLua(lua_State* L, std::string LuaVarName = "Text");
private:
	cFontDraw();					//コンストラクタを private に置く
	~cFontDraw();					//デストラクタを private に置く
	void Draw();
	const wchar_t* GetFileName(FontName);
	std::wstring StringToWString(std::string oString);
	static int TextDrawGlue(lua_State* L);

	std::unique_ptr<DirectX::SpriteBatch> m_batch;

	std::map<FontName, std::wstring> m_fontMap;				//フォント名からフォントを管理する
	std::map<FontName, std::unique_ptr<DirectX::SpriteFont>> m_fontData;				//フォント名からフォントを管理する
	std::vector<FontDrawData> m_drawData;	//描画するデータを保持する

	std::unique_ptr<DirectX::CommonStates> m_state;
};