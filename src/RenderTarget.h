/*=================================================
//								FILE : RenderTarget.h
//		ファイル説明 :
//		レンダーターゲットを扱いやすいようにクラス化する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"
#include "DirectXData.h"

struct RTTex
{
	ID3D11RenderTargetView* m_pDepthRenderTargetView;    //描画時にレンダーターゲットの設定とクリアで使用する。
	ID3D11DepthStencilView* m_pDepthStencilView;				  //描画時にレンダーターゲットの設定とクリアで使用する。
	ID3D11ShaderResourceView* m_pTextureView;        //2パス目に利用する。
	D3D11_VIEWPORT m_ViewPort;
	float ClereColor[4];
};

/// <summary>
/// レンダーターゲットビューと深度ステンシルビューを内包する。
/// 初期化カラー、幅高さ情報、フォーマットで設定可能。
/// テクスチャクラスを継承して設定することでレンダーターゲットONでテクスチャを作成する
/// 描画セット時は設定パスを指定できるようにすること
/// 使用するときはCreateを呼び出してから利用する直前にSetRenderを呼ぶ
/// </summary>
class cRenderTargetTex {
public:
	cRenderTargetTex();		//基本的に初期化だけ
	~cRenderTargetTex();		//基本的に開放だけ
	HRESULT RTCreate(const float _width,const float _height,DirectX::XMUINT3 _initColor, DXGI_FORMAT _format = DXGI_FORMAT_R32G32B32A32_FLOAT);
	/// <summary>
	/// この関数を呼ぶ場合、必要であればコンテキスト系をゲットして保持しておくこと。
	/// 基本的に1枚のターゲットでいい場合だけ利用する。
	/// 複数枚必要であればテクスチャやビューをゲットしてそれらを利用する。
	/// </summary>
	/// <param name="_pathNo">シェーダパス番号</param>
	void SetRenderTargetTex(const int _pathNo = 0);
	RTTex GetRenderTextureStruct();			//基本的にはアプリ側からは使わない。ディファードレンダリングに情報を大量に送るのでまとめてるだけ
	ID3D11ShaderResourceView* GetTextureResourceView();
	ID3D11DepthStencilView* GetDepthStencilView();
	DirectXDrawData GetDrawData();
private:
	HRESULT RenderTex(const float _width, const float _height, DXGI_FORMAT _format);
	HRESULT DepthStencilView(const float _width, const float _height, DXGI_FORMAT _format);
	HRESULT ShaderResourceView(const float _width, const float _height, DXGI_FORMAT _format);

	DirectX::XMUINT3 m_InitColor;		//画面クリアカラー

	ID3D11Texture2D* m_pDepthTexture;								 //この後の設定で必要。描画時には使わない
	ID3D11RenderTargetView* m_pDepthRenderTargetView;    //描画時にレンダーターゲットの設定とクリアで使用する。
	ID3D11Texture2D* m_pDepthStencil;								//描画時には利用しない。
	ID3D11DepthStencilView* m_pDepthStencilView;				  //描画時にレンダーターゲットの設定とクリアで使用する。
	ID3D11ShaderResourceView* m_pTextureView;        //2パス目に利用する。
	D3D11_VIEWPORT m_ViewPort;
};