/*=================================================
//								FILE : BoneStruct.h
//		ファイル説明 : 
//		ボーン情報を格納する構造体を定義する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <string>
#include <vector>


// TODO Xファイルの読み込みで一時的に作ったもの。今後必ず消す
struct Bone {
	std::string vBoneName;		//ボーンの名前
	int vBoneIndex;					//ボーンのインデックス番号
	DirectX::XMFLOAT4X4 vTransform;		//姿勢
	DirectX::XMFLOAT4X4 vBindPose;			//初期姿勢
	int vNumChild;									//子の数
	std::vector<Bone*> vpChild;				//この実態を格納していく

	Bone() {
		//初期化
		vBoneIndex = 0;
		DirectX::XMStoreFloat4x4(&vTransform, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&vBindPose, DirectX::XMMatrixIdentity());
		vNumChild = 0;
	}
	~Bone() {
		//vectorの内部に入ったボーンを解放する
		for (int i = 0; i < vNumChild; i++) {
			delete vpChild[i];
			vpChild[i] = nullptr;
		}
	}
};

//Meshに持たせる正式なデータ
struct BoneData {
	std::string vName;		//ボーン名
	int vParentNo;			//親番号
	DirectX::XMFLOAT4X4 vOffset;
	DirectX::XMFLOAT4X4 vTransPose;		//移動中のポーズ
	DirectX::XMFLOAT4X4 vInv;			//初期姿勢

	BoneData() {
		vParentNo = -1;		//親なし
		//単位行列をセット
		DirectX::XMStoreFloat4x4(&vOffset, DirectX::XMMatrixIdentity());
		vInv = vTransPose = vOffset;
	}
};