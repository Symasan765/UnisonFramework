/*=================================================
//								FILE : WaveSimulation.hlsl
//		ファイル説明 :
//      波のシミュレーションを行うテッセレーションステージを定義する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "ShaderStruct.hlsl"
#include "PerlinNoise.hlsl"

cbuffer global_waves : register(b4) //ボーンのポーズ行列が入る
{
    float g_time;
    float AMPLITUDE;			//波のふり幅
    float LENGTH;				//波長
    float CYCLE;					//周期
    float4 g_WavePoint; //波の発生源
};

struct HS_CONST
{
    float factor[3] : SV_TessFactor;
    float inner_factor : SV_InsideTessFactor;
};

struct HS_OUTPUT
{
    float4 Pos : POSITION; // ローカル位置座標
    float4 Normal : NORMAL; // 法線ベクトル   使わないけど他との汎用性のために残す
    float2 Tex : TEX; //テクスチャ座標
    uint4 Bones : BONE_INDEX; //ボーンのインデックス
    float4 Weights : BONE_WEIGHT; //ボーンの重み
};

//ハルシェーダーコンスタント
HS_CONST HSConstant(InputPatch<INPUT_RAW_DATA, 3> ip, uint pid : SV_PrimitiveID)
{
    HS_CONST Out = (HS_CONST)(0);

    float devide = 64;

    Out.factor[0] = devide;
    Out.factor[1] = devide;
    Out.factor[2] = devide;

    Out.inner_factor = devide;

    return Out;
}

//ハルシェーダー
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HSConstant")]
HS_OUTPUT HS(InputPatch<INPUT_RAW_DATA, 3> ip, uint cpid : SV_OutputControlPointID, uint pid : SV_PrimitiveID)
{
	//基本的にハルシェーダでは頂点情報をスルーして出力する
    HS_OUTPUT Out = (HS_OUTPUT) (0);
    Out.Pos = ip[cpid].Pos;
    Out.Normal = ip[cpid].Normal;
    Out.Bones = ip[cpid].Normal;
    Out.Tex = ip[cpid].Tex;
    Out.Weights = ip[cpid].Weights;
    return Out;
}

float GetWaveHeight(float3 pos,float a,float len,float time,float cycle){
	float Len = length(pos.xy);
    return a * sin(2 * 3.1415926f * (Len / len - time / cycle));
}

//ドメインシェーダー
[domain("tri")]
INPUT_RAW_DATA DS(HS_CONST In, float3 UV : SV_DomaInLocation, const OutputPatch<HS_OUTPUT, 3> patch)
{
	//ここでは座標変換以外のドメインの形成に関する処理を行う
	//まずは各情報を補間して正確な位置を割り出す処理が必要
    INPUT_RAW_DATA Out = (INPUT_RAW_DATA) (0);

    Out.Pos = patch[0].Pos * UV.x + patch[1].Pos * UV.y + patch[2].Pos * UV.z;
    Out.Normal = patch[0].Normal * UV.x + patch[1].Normal * UV.y + patch[2].Normal * UV.z;
    Out.Bones = patch[0].Bones * UV.x + patch[1].Bones * UV.y + patch[2].Bones * UV.z;
    Out.Tex = patch[0].Tex * UV.x + patch[1].Tex * UV.y + patch[2].Tex * UV.z;
    Out.Weights = patch[0].Weights * UV.x + patch[1].Weights * UV.y + patch[2].Weights * UV.z;

	//変形処理
	Out.Pos.z = GetWaveHeight(Out.Pos.xyz,AMPLITUDE,LENGTH,g_time,CYCLE);
	Out.Pos.z += GetWaveHeight(Out.Pos.xyz,AMPLITUDE / 4.0f,LENGTH * 2.0f,g_time,CYCLE * 1.2f);

	float aroundh[4] = { 0, 0, 0, 0 };
    float offset = 0.01f;   // 適当に設定
	aroundh[0] += GetWaveHeight(Out.Pos.xyz + (float3)(offset,0.0f,0.0f),AMPLITUDE,LENGTH,g_time,CYCLE);
    aroundh[1] += GetWaveHeight(Out.Pos.xyz + (float3)(0.0f,-offset,0.0f),AMPLITUDE,LENGTH,g_time,CYCLE);
    aroundh[2] += GetWaveHeight(Out.Pos.xyz + (float3)(-offset,0.0f,0.0f),AMPLITUDE,LENGTH,g_time,CYCLE);
    aroundh[3] += GetWaveHeight(Out.Pos.xyz + (float3)(0.0f,offset,0.0f),AMPLITUDE,LENGTH,g_time,CYCLE);

	//float4 norm = (float4)(cross(patch[1].Pos.xyz - patch[0].Pos.xyz,patch[2].Pos.xyz - patch[0].Pos.xyz),0.0f);
	//Out.Normal = normalize(norm);


	float dx = aroundh[2] - aroundh[0];
    float dz = aroundh[1] - aroundh[3];

	Out.Normal = -normalize( float4( dx, 0.2f, dz ,1.0f) );
    //Out.Pos.z = Out.Pos.y;
    //Out.Pos.y = PerlinNoise((Out.Pos.xy + 1.0f) / 2.0f, 1.0f / 8.0f, 1.0);
    return Out;
}