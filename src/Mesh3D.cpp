/*=================================================
//								FILE : Mesh3D.cpp
//		ファイル説明 :
//		メッシュマネージャを利用してメッシュ情報の取得を行う
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "Mesh3D.h"
#include "FbxLoad.h"
#include "XFileLoad.h"

using namespace DirectX;

std::shared_ptr<Mesh3D> MeshManager::LoadMeshData(std::string _fileName){
	//まずはパスを完成させる
	int point = (int)_fileName.find(".");		//.を探索して位置を調べる
	std::string name = _fileName.substr(0, point);	//ファイル名
	std::string extension = _fileName.substr(point + 1, _fileName.size());	//拡張子
	std::string path = "asset/Mesh/" + name + "/" + _fileName;		//[例]asset/model/model.fbx

	//まずその名前がmapに登録されているか確認する
	auto itr = m_MeshMap.find(path);        // "xyz" が設定されているか？
	if (itr != m_MeshMap.end()) {
		//設定されている場合の処理
		return m_MeshMap[path];		//shared_ptrのコピー
	}
	//次になければmapに領域を確保してから拡張子を判別して各パーサーを呼び出してデータを取得する
	else {
		//設定されていない場合の処理
		m_MeshMap[path] = std::make_shared<Mesh3D>();
		m_MeshMap[path]->dir = "asset/Mesh/" + name + "/";


		//ここでパーサーを呼び出し。読み込みフォルダを指定するの忘れずにね
		if (extension == "fbx") {	//FBX読み込み
			//将来的にここはFBXの場合に入れる処理にする
			cFbxLoad::GetInstance().GetFBXLoader(&*m_MeshMap[path], path);
		}
		else if (extension == "x") {	//X読み込み
			cXFileLoad::GetInstance().GetXFile(&*m_MeshMap[path], path);
			int a = 10;
		}
	}

	return m_MeshMap[path];
}

void MeshManager::SetBoneConstant(const BoneData* _bone, const int _Num, int pass)
{
	DirectX::XMMATRIX Mat[128];	//計算したデータを入れる場所
	DirectX::XMMATRIX Cur[128];

	//まず行列にそれぞれの姿勢行列をかけて格納する
	for (int i = 0; i < _Num; i++) {
		Cur[i] = XMLoadFloat4x4(&_bone[i].vInv);
		Mat[i] =  XMLoadFloat4x4(&_bone[i].vTransPose);
	}

	//次に親子関係をつけていく
	for (int i = 0; i < _Num; i++) {
		//親が存在する場合のみ処理する
		if (_bone[i].vParentNo != -1) {
			DirectX::XMFLOAT4X4 Child;
			DirectX::XMFLOAT4X4 Parent;
			XMStoreFloat4x4(&Child,Mat[i]);
			XMStoreFloat4x4(&Parent, Mat[_bone[i].vParentNo]);
			Mat[i] *= Mat[_bone[i].vParentNo];
		}
	}

	for (int i = 0; i < _Num; i++) {
		XMMATRIX offset = DirectX::XMLoadFloat4x4(&_bone[i].vOffset);
		XMFLOAT4X4 m;
		XMStoreFloat4x4(&m, Mat[i]);
		Mat[i] = offset * Mat[i];
		XMStoreFloat4x4(&m, Mat[i]);
		DirectX::XMStoreFloat4x4(&m_Bone.data.m[i], DirectX::XMMatrixTranspose(Mat[i]));		//ボーンをセットする
		int a = 10;
	}

	m_Bone.Set(pass);
}

void MeshManager::CheckDelete()
{
	//map上のすべてのデータを確認してカウンタ数を確認する
	//そのカウントがmapのみ(つまり1)ならそのmapを解放する
	//するとshared_ptrがデストラクタを呼びだしてMeshデータが解放される
	
	for (auto itr = m_MeshMap.begin(); itr != m_MeshMap.end(); ++itr) {
		//mapしかポインタを保持していない
		if (itr->second.use_count() == 1) {
			m_MeshMap.erase(itr->first);		//データ削除
			itr = m_MeshMap.begin();
			if (itr == m_MeshMap.end())
				return;
		}
	}
}
