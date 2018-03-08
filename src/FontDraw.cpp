/*=================================================
//								FILE : FontDraw.cpp
//		ファイル説明 :
//		文字フォントの表示を処理する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "FontDraw.h"
#include "WinMain.h"
#include "DirectX11.h"
#include "LuaConvert.h"
using namespace LuaConv;

/// <summary>
/// フォント描画クラスの初期化
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

	//格納文字数分データを描画
	for (auto itr = m_drawData.begin(); itr != m_drawData.end(); ++itr) {
		//フォントが読み込まれているかチェック
		auto itrt = m_fontData.find(itr->vFont);

		if (itrt != m_fontData.end()) {
			//読み込まれている！
			m_fontData[itr->vFont]->DrawString(m_batch.get(), itr->vText.c_str(), DirectX::XMFLOAT2(itr->vPosX, itr->vPosY), { itr->vColor.x,itr->vColor.y,itr->vColor.z,1.0f}, 0.0f, DirectX::XMFLOAT2{ 0.0f,0.0f }, itr->vScale);
		}
		else {
			//読み込まれていない
			m_fontData[itr->vFont] = std::make_unique<DirectX::SpriteFont>(GetDirectX::Device(), GetFileName(itr->vFont));
			m_fontData[itr->vFont]->DrawString(m_batch.get(), itr->vText.c_str(), DirectX::XMFLOAT2(itr->vPosX, itr->vPosY), { itr->vColor.x,itr->vColor.y,itr->vColor.z,1.0f }, 0.0f, DirectX::XMFLOAT2{ 0.0f,0.0f }, itr->vScale);
		}
	}
	m_batch->DirectX::SpriteBatch::End();

	m_drawData.erase(m_drawData.begin(), m_drawData.end());
}

/// <summary>
/// 解放処理
/// </summary>
cFontDraw::~cFontDraw() {

}

/// <summary>
/// テキストデータを描画用バッファに格納しておく。これはループの最後にまとめて描画される
/// </summary>
/// <param name="_drawText">描画用テキスト</param>
/// <param name="_x">スクリーン位置X</param>
/// <param name="_y">スクリーン位置Y</param>
/// <param name="_fontName">フォント名列挙対</param>
/// <param name="scale">倍率</param>
/// <param name="Color">カラー</param>
void cFontDraw::TextDraw(std::string _drawText, const int _x, const int _y, FontName _fontName, float scale, DirectX::XMFLOAT3 Color)
{
	std::wstring text = StringToWString(_drawText);
	//描画するデータを追加
	m_drawData.push_back({ text, _x, _y, _fontName,scale ,Color });
}

const wchar_t* cFontDraw::GetFileName(FontName _Enum)
{
	//指定の文字列を返却する
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
	//=================まず各フォント名をenumに登録しておく========================
	m_fontMap[FontName::Mplus] = L"font/Mplus.spritefont";
	m_fontMap[FontName::MSGothic] = L"font/MGothic.spritefont";
	m_fontMap[FontName::KokuGothic] = L"font/KokuGothic.spritefont";
	m_fontMap[FontName::TeraMin] = L"font/GenEiLateMinT.spritefont";
	//=====================================================================

	//とりあえず初めはM+だけ読み込み
	m_fontData[FontName::Mplus] = std::make_unique<DirectX::SpriteFont>(GetDirectX::Device(), GetFileName(FontName::Mplus));
	m_fontData[FontName::MSGothic] = std::make_unique<DirectX::SpriteFont>(GetDirectX::Device(), GetFileName(FontName::MSGothic));

	m_batch->SetViewport(cDirectX11::GetInstance()->GetViewPort());
	m_state = std::make_unique<DirectX::CommonStates>(GetDirectX::Device());
}

std::wstring cFontDraw::StringToWString(std::string oString){
	// SJIS → wstring
	int iBufferSize = MultiByteToWideChar(CP_ACP, 0, oString.c_str()
		, -1, (wchar_t*)NULL, 0);

	// バッファの取得
	wchar_t* cpUCS2 = new wchar_t[iBufferSize];

	// SJIS → wstring
	MultiByteToWideChar(CP_ACP, 0, oString.c_str(), -1, cpUCS2
		, iBufferSize);

	// stringの生成
	std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);

	// バッファの破棄
	delete[] cpUCS2;

	// 変換結果を返す
	return(oRet);
}

int cFontDraw::TextDrawGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cFontDraw>(L);

	//引数を取得していく
	std::string text = lua_tostring(L, 2);
	float x = (float)lua_tonumber(L, 3);
	float y = (float)lua_tonumber(L, 4);

	//関数実行
	p->TextDraw(text,x,y);
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	return 0;		//戻り値
}
