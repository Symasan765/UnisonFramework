/*=================================================
//								FILE : DirectionalLight.h
//		ファイル説明 :
//		ゲームシーンが管理するディクショナルライトを定義する。
//		
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"
#include "ShaderStruct.h"
#include "ConstantBuffer.h"

/// <summary>
/// ディクショナルライトクラス。シーン中の環境光を保持する。
/// </summary>
class cDirectionalLight {
public:
	cDirectionalLight();
	~cDirectionalLight();
	// TODO 以前作成した緯度と経度から太陽位置を算出するプログラムを組み込みたい
	void SetDiffuseColor(DirectX::XMFLOAT4 _diffuse);
	void SetDiffuseColor(int _r,int _g,int _b);
	void SetAmbientColor(DirectX::XMFLOAT4 _ambient);
	void SetAmbientColor(int _r, int _g, int _b);
	void SetLightPos(DirectX::XMFLOAT4 _pos);
	void SetConstantBuffer(const int _passNo);			//情報をシェーダにセットする
	DirectX::XMFLOAT3 GetLightPos()const;
private:
	cConstBuf<DirectionalLightData> m_ShaderConst;
};