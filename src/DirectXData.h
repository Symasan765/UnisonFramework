/*=================================================
//								FILE : DirectXData.h
//		ファイル説明 :
//		DirectXが作成したデータを管理するための構造体を定義する。
//		主にゲームシーン内で保管する目的で使用する
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"

/// <summary>
/// DirectXが作成したデータを保持し、管理する
/// </summary>
struct DirectXData {
	ID3D11Device* v_pD3DDevice;									//デバイスハンドル
	ID3D11DepthStencilState* v_pDepthStencilState;
	ID3D11DeviceContext* v_pImmediateContext;			// デバイス・コンテキスト
	IDXGISwapChain* v_pSwapChain;								 // スワップ・チェイン
	ID3D11RenderTargetView* v_pRenderTargetView;	//ターゲットビュー
	ID3D11Texture2D* v_pDepthStencil;						//深度・ステンシル
	ID3D11DepthStencilView* v_pDepthStencilView;		//深度ステンシルビュー
	D3D11_VIEWPORT v_ViewPort;									//ビューポート
	ID3D11RasterizerState* v_pCullNoneRasterizerState;		//ラスタライザ
	ID3D11RasterizerState* v_pCullLeftRasterizerState;
	ID3D11RasterizerState* v_pCullRightRasterizerState;
	ID3D11RasterizerState* v_pWIREFRAMERasterizerState;
	ID3D11BlendState* v_pBlendState;							//デフォルトブレンドステート
	ID3D11BlendState* v_pAlphaState;							//アルファブレンドステート
	ID3D11BlendState* v_pAddState;							//加算合成
};

/// <summary>
/// よく利用するデバイスとコンテキストをまとめた構造体
/// </summary>
struct DirectXDevice {
	ID3D11Device* v_pD3DDevice;									//デバイスハンドル
	ID3D11DeviceContext* v_pImmediateContext;			// デバイス・コンテキスト
};

/// <summary>
/// 描画によく利用するレンダーターゲットなども入れた構造体
/// </summary>
struct DirectXDrawData {
	D3D11_VIEWPORT v_ViewPort;									//ビューポート
	ID3D11ShaderResourceView* v_ShaderResourceView;
	ID3D11RenderTargetView* v_pRenderTargetView;	//ターゲットビュー
	ID3D11DepthStencilView* v_pDepthStencilView;		//深度ステンシルビュー	ID3D11DepthStencilState* v_pDepthStencilState;
};