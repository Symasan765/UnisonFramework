/*=================================================
//								FILE : DeferredConst.h
//		ファイル説明 :
//		ディファードレンダリングで利用するための定数を取り扱うクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once

#include "DeferredConst.h"
#include "ConstantBuffer.h"
#include "ShaderStruct.h"
#include "WinMain.h"
#include "Camera.h"

/// <summary>
/// ディファードレンダリングに利用する定数バッファを管理するクラス
/// 基本的にアプリから使う場合、設定する項目は
/// まず描画シーンの最初にSetCameraData、SetLightDataを呼ぶ。後はBaseModelの中で行ってくれる
/// </summary>
class cDeferredConst{
public:
	cDeferredConst();
	~cDeferredConst();
	void SetShader(bool _mtrOnly = false);
	static void SetCameraData(const CameraData& _cameraData, ViewProj _vp);
	static void SetLightData(const DirectX::XMFLOAT3& _light);
	void SetWorldMatrix(const DirectX::XMFLOAT4X4& _w);
	void SetEmission(DirectX::XMFLOAT4 _color);
	void SetMaterial(DirectX::XMFLOAT4 _a, DirectX::XMFLOAT4 _d, DirectX::XMFLOAT4 _s);
	void SetShaderType(const int);
	const CameraData GetCameraData()const;
private:
	cConstBuf<DefeMatrix> m_MatrixConst;		//シェーダに渡す定数バッファ
	cConstBuf<DefeMaterial> m_MaterialConst;

	//設定時に必要な項目
	static DirectX::XMFLOAT3 m_LightPos;			//ライト位置
	static DirectX::XMFLOAT3 m_Lookpt;			//カメラ注視点
	static DirectX::XMFLOAT3 m_CameraPos;		//カメラ位置
	static DirectX::XMFLOAT3 m_CameraUp;
	static DirectX::XMFLOAT4X4 m_View;			//ビュー行列
	static DirectX::XMFLOAT4X4 m_DepthProj;			//プロジェクション行列
	static DirectX::XMFLOAT4X4 m_Proj;			//プロジェクション行列
	DirectX::XMFLOAT4X4 m_World;						//個別のワールド行列
	static float m_Near;
	static float m_Far;
};