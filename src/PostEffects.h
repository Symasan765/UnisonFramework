/*=================================================
//								FILE : PostEffects.h
//		ファイル説明 :
//		ポストエフェクト処理を扱いやすくするようなクラスを作成
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "Shader.h"
#include "ConstantBuffer.h"
#include <string>

/// <summary>
/// ポストエフェクトを取り扱うクラス
/// </summary>
class cPostEffects {
public:
	friend class cDirectX11;	//解放処理を行う

	//テクスチャに適用するサンプラ。ビット演算に対応させる
	enum Sampler {
		LINEAR = 1,
		POINT = 2
	};
	cPostEffects(std::string _fileName);
	virtual ~cPostEffects();
	void SetTexSamplerState(const int slot, Sampler flag);
	
protected:
	void Draw(ID3D11ShaderResourceView** _tex, int _ResourceNum = 1);
	void SetShader();
private:
	static void Destroy();
	
	void Create();
	HRESULT CreateVertex();
	std::shared_ptr<ShaderData> m_Shader;		//扱うシェーダ

	//ここから下はクラス全体で共有する
	static ID3D11Buffer* vVertexBuf;			//頂点バッファ
	static ID3D11SamplerState* m_pSampleLinear;
	static ID3D11SamplerState* m_pSamplePoint;
	cConstBuf<DirectX::XMFLOAT4X4> m_ShaderConst;		//シェーダに渡す定数バッファ
};