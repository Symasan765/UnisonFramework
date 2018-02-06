/*=================================================
//								FILE : SkyDome.h
//		ファイル説明 :
//		スカイドームの読み込み、シェーダ切り替え、描画を行うクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "DeferredBaseModel.h"
#include "PostEffects.h"
#include "Camera.h"

class cSkyDome{
public:
	cSkyDome();
	~cSkyDome();
	void DrawSkyDome(CameraData camera);
private:
	cDeferredModel* m_SkyDome;		//スカイドームモデルを読み込む
	std::shared_ptr<ShaderData> m_Shader;									//ディファードレンダリングシェーダを保持
	ID3D11DepthStencilState* m_pDepthStencilState;

	float m_RotAngle;
};