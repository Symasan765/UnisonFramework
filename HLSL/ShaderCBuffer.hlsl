/*=================================================
//								FILE : ShaderCBuffer.hlsl
//		ファイル説明 :
//
//
//							HAL大阪 : 松本 雄之介
=================================================*/
//定義
#define MAX_BONE_MATRICES 128       //最大ボーン数
#define STATIC_MESH 128                 //アニメーションなし

cbuffer WorldMatrix : register(b0)
{
    matrix mW; //ワールド行列
    matrix mWVP; //ワールドから射影までの変換合成行列
    matrix mWLP; //ワールド・”ライトビュー”・プロジェクションの合成
    matrix mWLPB; //ワールド・”ライトビュー”・プロジェクション・UV行列の合成
    matrix mDepthWVP;
}

cbuffer Material : register(b1)
{
	// 反射率
    float4 k_a; // ambient
    float4 k_d; // diffuse
    float4 k_s; // specular
    float4 Emission; //発光色
    int4 General; //x成分 = シェーダ番号 他…未定
}

cbuffer global_bones : register(b2) //ボーンのポーズ行列が入る
{
    matrix g_mConstBoneWorld[MAX_BONE_MATRICES];
};

cbuffer global_Light:register(b3)
{	
	float4 LightDir;		//ライト位置
	float4 I_Ambient;
	float4 I_Deffuse;
};

cbuffer global_Camera:register(b4)
{	
	float4 CameraPos;
};

cbuffer global_ToonParam:register(b5)
{	
	float LightBorder;
	float ShadowBorder;
	float HighlightSub;
	float HighlightSum;
};