/*=================================================
//								FILE : SpriteDraw.h
//		ファイル説明 :
//		2Dスプライトを描画するクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"
#include "ConstantBuffer.h"

//基本的にstatic関数で構成しておく
//必要なシェーダと頂点情報は自ら保持する
class cSprite2DDraw {
public:
	//更新処理や解放処理を行うためにcDirectXクラスをフレンド化しておく
	friend class cDirectX11;
	cSprite2DDraw(const cSprite2DDraw&) = delete;			//コピーコンストラクタを delete 指定
	cSprite2DDraw& operator=(const cSprite2DDraw&) = delete;	//コピー代入演算子も delete 指定
	cSprite2DDraw(cSprite2DDraw&&) = delete;			//ムーブコンストラクタを delete 指定
	cSprite2DDraw& operator=(cSprite2DDraw&&) = delete;		//ムーブ代入演算子も delete 指定

	static cSprite2DDraw& GetInstance() {
		static cSprite2DDraw inst;			// privateなコンストラクタを呼び出す
		return inst;
	}
	void Draw(ID3D11ShaderResourceView* _tex,DirectX::XMFLOAT2 _LeftTop, DirectX::XMFLOAT2 _RightDown);
private:
	cSprite2DDraw();					//コンストラクタを private に置く
	~cSprite2DDraw();					//デストラクタを private に置く
	void Create();			//頂点シェーダやシェーダを読み込む

	HRESULT CreateVertex();
	HRESULT CreateShader();

	//2Dスプライトは特殊なので他のものとは併用しない
	ID3D11VertexShader* vpVertexShader;
	ID3D11PixelShader* vpPixelShader;
	ID3D11InputLayout* vpInputLayout;
	ID3D11Buffer* vVertexBuf;			//頂点バッファ
	ID3D11SamplerState* m_pSampleLinear;

	cConstBuf<DirectX::XMFLOAT4X4> m_ShaderConst;		//シェーダに渡す定数バッファ
	float m_NowDepth;			//1フレーム毎に値を引き、後から描画されたもの程後ろに描画するようにする
};