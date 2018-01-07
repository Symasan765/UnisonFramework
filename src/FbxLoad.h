/*=================================================
//								FILE : FbxLoad.h
//		ファイル説明 :
//		FBXをロードするローダー
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once

#include "Mesh3D.h"
#include <fbxsdk.h>

/// <summary>
/// 基本的にはメッシュマネージャからしか呼び出さない
/// </summary>
class cFbxLoad {
public:
	cFbxLoad(const cFbxLoad&) = delete;			//コピーコンストラクタを delete 指定
	cFbxLoad& operator=(const cFbxLoad&) = delete;	//コピー代入演算子も delete 指定
	cFbxLoad(cFbxLoad&&) = delete;			//ムーブコンストラクタを delete 指定
	cFbxLoad& operator=(cFbxLoad&&) = delete;		//ムーブ代入演算子も delete 指定

	static cFbxLoad& GetInstance() {
		static cFbxLoad inst;			// privateなコンストラクタを呼び出す
		return inst;
	}

	HRESULT GetFBXLoader(Mesh3D* mesh,std::string);

private:
	HRESULT cFbxLoad::Init(std::string _fileName);
	void cFbxLoad::GetMesh(FbxNode* node, Mesh3D* mesh);
	void cFbxLoad::GetMeshData(FbxMesh* pFbxMesh, Mesh3D* meshdata);
	HRESULT cFbxLoad::CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer);
	cFbxLoad() = default;					//コンストラクタを private に置く
	~cFbxLoad() = default;					//デストラクタを private に置く

	FbxManager* m_pManager;
	FbxScene* m_pScene;
};