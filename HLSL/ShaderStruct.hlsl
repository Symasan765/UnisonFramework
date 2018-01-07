/*=================================================
//								FILE : ShaderStruct.hlsl
//		ファイル説明 :
//
//
//							HAL大阪 : 松本 雄之介
=================================================*/

//頂点シェーダ出力構造体
struct VS_OUTPUT
{
    float4 Pos : POSITION;
    float4 Normal : NORMAL;
    float4 Depth : WORLDDEPTH;
    float4 LightDepth : DEPTH; //深度値
    float2 UV : TEXCOORD0;
    float4 LightUV : TEXCOORD1; //ライト座標に変換した際のUV座標
    float4 WorldPos : POSITION2;
};

//ピクセルシェーダ入力構造体
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Normal : NORMAL;
    float4 Depth : WORLDDEPTH;
    float4 LightDepth : DEPTH; //深度値
    float2 UV : TEXCOORD0;
    float4 LightUV : TEXCOORD1;
    float4 WorldPos : TEXCOORD2; //ワールド座標情報が必要なので転送する
};

//最終レンダリング
struct PS_OUTPUT
{
    float4 vColor : SV_Target0;					//照明計算済カラー
    float4 vPosition : SV_Target1;				//座標情報
    float4 vNormal : SV_Target2;					//法線の向き w : 深度
    float4 vBright : SV_Target3;					//高輝度情報
    float4 vScreenSpaceSSS : SV_Target4;	//XYZ : SSSSS部カラー W : SSSSSマスク
};

//スキニング後の頂点・法線が入る
struct Skin
{
    float4 Pos;
    float4 Normal;
};

//何も処理をせずそのまま出力する構造体
//主に頂点シェーダではなくジオメトリシェーダで処理する場合に使用する
//頂点シェーダの出力とドメインシェーダーの出力、ジオメトリシェーダの入力に利用する
struct INPUT_RAW_DATA
{
    float4 Pos : POSITION; // ローカル位置座標
    float4 Normal : NORMAL; // 法線ベクトル   使わないけど他との汎用性のために残す
    float2 Tex : TEX; //テクスチャ座標
    uint4 Bones : BONE_INDEX; //ボーンのインデックス
    float4 Weights : BONE_WEIGHT; //ボーンの重み
};