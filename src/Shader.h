/*=================================================
//								FILE : Shader.h
//		ファイル説明 :
//		シェーダのロードや領域確保を行うクラスを定義する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once

#include <string>
#include <memory>
#include <map>
#include "WinMain.h"
#include "ShaderStruct.h"

//各種シェーダを格納できる
struct ShaderData {
	ID3D11VertexShader* vpVertexShader;
	ID3D11GeometryShader* vpGeometryShader;
	ID3D11PixelShader* vpPixelShader;
	ID3D11InputLayout* vpInputLayout;

	void Set() {
		GetDirectX::Context()->IASetInputLayout(vpInputLayout);

		GetDirectX::Context()->VSSetShader(vpVertexShader, NULL, 0);
		GetDirectX::Context()->GSSetShader(vpGeometryShader, NULL, 0);
		GetDirectX::Context()->PSSetShader(vpPixelShader, NULL, 0);
	}

	~ShaderData() {
		SAFE_RELEASE(vpVertexShader);
		SAFE_RELEASE(vpGeometryShader);
		SAFE_RELEASE(vpPixelShader);
		SAFE_RELEASE(vpInputLayout);
	}

	//戻り値はGSの有無
	bool release() {
		bool GSflag = false;
		if (vpGeometryShader) GSflag = true;
		SAFE_RELEASE(vpVertexShader);
		SAFE_RELEASE(vpGeometryShader);
		SAFE_RELEASE(vpPixelShader);
		/*SAFE_RELEASE(vpInputLayout);*/		//インプットレイアウトは更新の必要なし
		return GSflag;
	}
};

//取得するインプットレイアウトを指定する(ビット演算)
enum InpLayout {
	inPOSITION = 1,
	inNORMAL = 2,
	inTEX_UV = 4,
	inBone = 8,
	inWeight = 16
};

/// <summary>
/// シェーダの管理を行う。シングルトン
/// </summary>
class SHADER {
public:
	//更新処理や解放処理を行うためにcDirectXクラスをフレンド化しておく
	friend class cDirectX11;
	SHADER(const SHADER&) = delete;
	SHADER& operator=(const SHADER&) = delete;
	SHADER(SHADER&&) = delete;
	SHADER& operator=(SHADER&&) = delete;

	static SHADER* GetInstance() {
		static SHADER inst;			// privateなコンストラクタを呼び出す
		return &inst;
	}

	std::shared_ptr<ShaderData> LoadShaderFile(std::string _fileName, const short bitFlag, bool _GSflag = false);
	
	template<typename T>
	static bool GetConstBuffer(ID3D11Buffer** _constBuf) {
		D3D11_BUFFER_DESC cb1;
		cb1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cb1.ByteWidth = sizeof(T);
		cb1.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cb1.MiscFlags = 0;
		cb1.StructureByteStride = 0;
		cb1.Usage = D3D11_USAGE_DYNAMIC;

		if (FAILED(GetDirectX::Device()->CreateBuffer(&cb1, NULL, _constBuf)))
		{
			return false;
		}
		return true;
	}

	template<typename T>
	static bool SetConstBufferData(ID3D11Buffer* _constBuf, T* data) {
		bool succes = false;
		D3D11_MAPPED_SUBRESOURCE pData1;
		if (SUCCEEDED(GetDirectX::Context()->Map(_constBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData1)))
		{
			memcpy_s(pData1.pData, pData1.RowPitch, (void*)(data), sizeof(T));
			succes = true;
		}
		GetDirectX::Context()->Unmap(_constBuf, 0);

		return succes;
	}

	HRESULT ShaderReload(std::string _fileName);
private:
	HRESULT LoadShader(std::string _ShaderFileName, const short bitFlag, ShaderData* _data, bool _GSflag);
	HRESULT GetInputLayout(ShaderData* _data, ID3DBlob* blob, const short bitFlag);
	void CheckDelete();
	SHADER();
	~SHADER();

	std::map<std::string, std::shared_ptr<ShaderData>> m_Map;		//テッセレーションを保持するマップ

	//================デフォルトの定数バッファ===================//
	ID3D11Buffer* ConstWorldMtx;		//ワールド、プロジェクション行列を転送する定数バッファ
	ID3D11Buffer* ConstLightVec;		//ライト、カメラ座標を管理
	ID3D11Buffer* ConstMaterial;		//マテリアルを管理
};
