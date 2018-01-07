/*=================================================
//								FILE : Vertex.h
//		ファイル説明 :
//		シェーダー上で扱う頂点構造体を定義する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"

struct VERTEX3D {
	DirectX::XMFLOAT4 vPos;		//座標
	DirectX::XMFLOAT4 vNorm;	//法線
	DirectX::XMFLOAT2 vTex;		//UV座標
	DirectX::XMUINT4 vBones;	//適用ボーン
	DirectX::XMFLOAT4 vWeight;	//ボーンの重み

	VERTEX3D() {
		vPos = { 0.0f,0.0f,0.0f,0.0f };
		vNorm = { 0.0f,0.0f,0.0f,0.0f };
		vTex = { 0.0f,0.0f };
		vBones = { 128,128,128,128 };		//ボーン最大数を入れておくとシェーダ上でボーンなしとなる
		vWeight = { 0.0f,0.0f,0.0f,0.0f };
	}

	VERTEX3D(DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 norm, DirectX::XMFLOAT2 uv, DirectX::XMUINT4 bone,DirectX::XMFLOAT4 weight) {
		vPos = pos;
		vNorm = norm;
		vTex = uv;
		vBones = bone;		//ボーン最大数を入れておくとシェーダ上でボーンなしとなる
		vWeight = weight;
	}
} ;

typedef struct _XYZUV {
	DirectX::XMFLOAT4 vPos;		//座標
	DirectX::XMFLOAT2 vTex;		//UV座標
}XYZUV;