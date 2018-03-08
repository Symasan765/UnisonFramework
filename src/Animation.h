/*=================================================
//								FILE : Animation.h
//		ファイル説明 :
//		メッシュデータのアニメーションを扱うクラス、構造体を定義する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"
#include "BoneStruct.h"

/// <summary>
/// 1キーフレームを管理する構造体
/// </summary>
struct KeyFrame {
	int vTime;								//アニメーションするフレーム
	DirectX::XMFLOAT4X4 vMat;		//補間する姿勢

	KeyFrame() {
		vTime = -1;		//初期タイム
		DirectX::XMStoreFloat4x4(&vMat, DirectX::XMMatrixIdentity());		//単位行列をセット
	}
};

/// <summary>
/// 一つのボーンのアニメーションを管理
/// </summary>
struct BoneAnime {
	int vBoneNo;						//動かすボーン番号
	int vKeyNum;						//キーフレームの数
	KeyFrame* vpKeyFrame;		//キーフレーム

	BoneAnime() {
		vBoneNo = 0;
		vKeyNum = 0;
		vpKeyFrame = nullptr;
	}

	~BoneAnime() {
		delete[] vpKeyFrame;
	}
};

/// <summary>
/// 一つの動作に対するデータを保持する
/// </summary>
struct MotionData {
	int vBoneNum;						//動かすボーン数
	int vMaxTime;						//動いている最大フレーム数
	BoneAnime* vBoneAnime;		//複数のボーンのアニメーション

	MotionData() {
		vBoneNum = 0;
		vMaxTime = 0;
		vBoneAnime = nullptr;
	}

	~MotionData() {
		delete[] vBoneAnime;
	}
};

/// <summary>
/// 一つのモデルが保持するアニメーションデータを管理する
/// </summary>
class cAnimationData {
public:
	//アニメーション情報をもらうためにファイルローダーをフレンド化する
	friend class cXFileLoad;
	cAnimationData();
	~cAnimationData();
	bool ToAnimetion(int _AnimeNo);
	void SetBoneConst(int _AnimeNo,float _Time);			//ボーン情報を定数バッファにセットする
private:
	MotionData* m_pMotion;		//動作分のモーションを保持する
	int m_MotionNum;				//モーション数
	int m_NowMotionNo;			//現在動かしているモーション番号
	int m_AnimeCnt;					//アニメーションさせているフレームレートをカウントしていく

	BoneData* m_pBone;						//ボーンデータ
	int m_pBoneNum;								//ボーン数
};