/*=================================================
//								FILE : GBuffer.h
//		ファイル説明 :
//		ディファードレンダリングで利用するG-Bufferを構造体として
//		定義する。
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"

struct GBuffer {
	ID3D11ShaderResourceView* vDiffuse;					//カラー情報
	ID3D11ShaderResourceView* vPosition;					//座標情報
	ID3D11ShaderResourceView* vNormal;						//法線(w成分に深度値が格納)
	ID3D11ShaderResourceView* vHighBrightness;		//高輝度情報
	ID3D11ShaderResourceView* vSSSSS;						//SSS

	/// <summary>
	/// G-Bufferすべてをシェーダステージにセットする
	/// </summary>
	void SetAllGBuffer() {
		GetDirectX::Context()->PSSetShaderResources(0, sizeof(GBuffer) / sizeof(ID3D11ShaderResourceView*), &vDiffuse);
		GetDirectX::Context()->VSSetShaderResources(0, sizeof(GBuffer) / sizeof(ID3D11ShaderResourceView*), &vDiffuse);
	}

	/// <summary>
	/// 指定したバッファを指定パスにセットする
	/// </summary>
	/// <param name="_no">0 = カラー,1 = 座標,2 = 法線,3 = アンビエント,4 = スペキュラ,5 = 自発光,6 = シャドウマップ</param>
	/// <param name="_pass"></param>
	void SetSelectView(const int _no,const int _pass) {
		GetDirectX::Context()->PSSetShaderResources(_no, 1, (&vDiffuse + _no));
		GetDirectX::Context()->VSSetShaderResources(_no, 1, (&vDiffuse + _no));
	}
};