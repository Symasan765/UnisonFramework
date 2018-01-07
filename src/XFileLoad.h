/*=================================================
//								FILE : XFileLoad.cpp
//		ファイル説明 :
//		アニメーション付きXファイルの読み込みを行うクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include <string>
#include <Windows.h>
#include <DirectXMath.h>
#include <fstream>

#include "Mesh3D.h"
#include "BoneStruct.h"

struct XFileLoadMeshBuf;		//Xファイル読み込み時に情報を一か所にまとめるための構造体

namespace XFileManager {
	//Xファイルから読み込んだボーン情報を一時的に格納しておく構造体
	struct XFileBone {
		std::string vName;					//自分の名前
		int vParentNo;						//親の添え字番号
		DirectX::XMFLOAT4X4 vMat;		//初期姿勢行列
		DirectX::XMFLOAT4X4 vBind;		//初期姿勢行列
	};
}

/// <summary>
/// Xファイルローダー。シングルトン
/// </summary>
class cXFileLoad {
public:
	cXFileLoad(const cXFileLoad&) = delete;			//コピーコンストラクタを delete 指定
	cXFileLoad& operator=(const cXFileLoad&) = delete;	//コピー代入演算子も delete 指定
	cXFileLoad(cXFileLoad&&) = delete;			//ムーブコンストラクタを delete 指定
	cXFileLoad& operator=(cXFileLoad&&) = delete;		//ムーブ代入演算子も delete 指定

	static cXFileLoad& GetInstance() {
		static cXFileLoad inst;			// privateなコンストラクタを呼び出す
		return inst;
	}
	void GetXFile(Mesh3D* pMesh,std::string _FileName);		//ここを呼び出してロードする

private:
	void InitData();		//読み込む準備をする
	void GetMesh(std::string _FileName, XFileLoadMeshBuf* _output);
	void CreateMesh3D(Mesh3D* pMesh, XFileLoadMeshBuf*);
	void GetNormal(std::ifstream* _pFP, XFileLoadMeshBuf* _output);
	void GetUV(std::ifstream* _pFP, XFileLoadMeshBuf* _output);
	void GetMaterialList(std::ifstream* _pFP, XFileLoadMeshBuf* _output);
	void GetMeshBone(std::ifstream* _pFP, XFileLoadMeshBuf * _output);

	void SearchFrame(std::ifstream* _pFP);
	int CheckType(std::string _FileName);
	void GetFrame(std::ifstream* _pFP,Bone* _pBone,std::string _ParentName);
	void SetBoneData(std::string _ParentName,std::string _Name,DirectX::XMFLOAT4X4 _Mat);
	void SkipScope(std::ifstream* _pFP);		// スコープを抜けるための関数

	//アニメーション読み込み
	int CountAnimeBone(std::ifstream* _pFP);		//アニメーションの中にあるボーンの数を数えてスコープを抜ける
	void GetAnimation(std::ifstream* _pFP,cAnimationData*& _output);	//実際にデータを格納していく関数
	void GetKeyFrame(std::ifstream* _pFP, BoneAnime* _pBone);

	HRESULT CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer);

	cXFileLoad() = default;						//コンストラクタを private に置く
	~cXFileLoad() = default;					//デストラクタを private に置く

	XFileManager::XFileBone m_Bones[128];	//読み込んだボーンを一時的に入れる
	short m_BoneInMotion[128];					//一つのモーションに格納されているボーン数(最大モーション数128)
	int m_MotionNum;	//モーション数
	int m_Suffix;			//現在何番目のボーンを読んでいるかをチェックする
	int m_Type;			//モデル情報を確認する()
};
