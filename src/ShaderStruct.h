/*=================================================
//								FILE : ShaderStruct.h
//		ファイル説明 :
//		シェーダで利用する定数バッファ構造体を定義しておくファイル
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"

struct WorldData {
	DirectX::XMFLOAT4 vLightDir;	//ライト方向
	DirectX::XMFLOAT4 vEyePos;		//カメラ座標
};

struct MatrixData
{
	DirectX::XMFLOAT4X4 mWVP;		//ワールドから射影までの変換行列
	DirectX::XMFLOAT4X4 mW;			//ワールド変換行列
	void Transpose();			//保持している行列を転置行列に変換する
	void SetWorldMatrix(const DirectX::XMMATRIX& mat);
	void SetWVPMatrix(const DirectX::XMMATRIX& mat);
	void SetW_VPMatrix(const DirectX::XMFLOAT4X4& w, const DirectX::XMFLOAT4X4& vp);
};

struct LightData {
	DirectX::XMFLOAT4 I_a; // ambient
	DirectX::XMFLOAT4 I_d; // diffuse
};

struct MaterialData {
	DirectX::XMFLOAT4 k_a; // ambient
	DirectX::XMFLOAT4 k_d; // diffuse
	DirectX::XMFLOAT4 vColor; //マテリアルカラー
};

struct FogData {
	DirectX::XMFLOAT4 vColor;		//フォグのカラー
	float vFactor;						//フォグの強さを調整する因子
	int vShadowFlag;					//シャドウを適用するかどうかのフラグ
	DirectX::XMFLOAT2 vDamy;		//16倍数に調整するためのダミー
};

struct DoFData {
	DirectX::XMFLOAT2 vScreenPos;	//被写界深度の適用を行う画面座標
	float vDoFRange;						//ピントボケの範囲。大きいとすぐボケる
	float vBlur;								//ボケの大きさ
};

struct RadialBlurData {
	DirectX::XMFLOAT2 vBlurPos;        //ぼかし起点
	float vDelta;				//ぼかしの大きさ。-0.3くらいがちょうどいい
	float vBlurNum;              //ぼかし回数。多いとなめらかになる
};

/// <summary>
/// アプリケーション側で一時的にラジアルブラーの処理を行う構造体でシェーダには渡さない
/// </summary>
struct RadialBlurBuf {
	DirectX::XMFLOAT3 vBlurPos;        //ぼかし起点
	float vDelta;				//ぼかしの大きさ。-0.3くらいがちょうどいい
	float vBlurNum;              //ぼかし回数。多いとなめらかになる
	bool RadialBlurFlag;
};

//=======================ディファードレンダリングに利用する=========================//
//ワールドマトリクス情報
struct DefeMatrix {
	DirectX::XMFLOAT4X4 mW;
	DirectX::XMFLOAT4X4 mWVP;
	DirectX::XMFLOAT4X4 mWLP;
	DirectX::XMFLOAT4X4 mWLPB;
	DirectX::XMFLOAT4X4 mDepthWVP;
};

//マテリアル情報
struct DefeMaterial
{
	DirectX::XMFLOAT4 k_a;			//アンビエント
	DirectX::XMFLOAT4 k_d;			//ディフーズ
	DirectX::XMFLOAT4 k_s;			//スペキュラ
	DirectX::XMFLOAT4 Emission;	//自発光色
	DirectX::XMINT4 Flag;				//x成分 = シェーダ番号 他…未定
};

//環境光構造体
struct DirectionalLightData {
	DirectX::XMFLOAT4 LightDir;		//ライト位置
	DirectX::XMFLOAT4 I_Ambient;
	DirectX::XMFLOAT4 I_Deffuse;
};

//波のパラメータとして利用する
struct WaveStruct {
	float vtime;
	float AMPLITUDE;			//波のふり幅
	float LENGTH;				//波長
	float CYCLE;					//周期
	DirectX::XMFLOAT4 vWavePoint; //波の発生源
};

struct ToonLightingStruct {
	float LightBorder;
	float ShadowBorder;
	float HighlightSub;
	float HighlightSum;

	ToonLightingStruct() {
		LightBorder = 0;
		ShadowBorder = 0;
		HighlightSub = 0;
		HighlightSum = 0;
	}
};

struct OutlineParam {
	float offsetScall;
	float DepthRange;
	float NormalRange;
	float OutlineSum;

	OutlineParam() {
		offsetScall = 0;
		DepthRange = 0;
		NormalRange = 0;
		OutlineSum = 0;
	}
};