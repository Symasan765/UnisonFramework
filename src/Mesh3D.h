/*=================================================
//								FILE : Mesh3D.h
//		ファイル説明 :
//		描画に利用するメッシュ部分を取り扱うクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include <memory>
#include <map>

#include "TextureManager.h"
#include "Vertex.h"
#include "WinMain.h"
#include "BoneStruct.h"
#include "ConstantBuffer.h"
#include "Animation.h"

struct Material {
	DirectX::XMFLOAT4 Ka;//アンビエント
	DirectX::XMFLOAT4 Kd;//ディフューズ
	DirectX::XMFLOAT4 Ks;//スペキュラー
	std::shared_ptr<Texture2D> pDefaultTex;				//通常ディフューズ
	std::shared_ptr<Texture2D> pShadowTex;				//影ディフューズ
	std::shared_ptr<Texture2D> pLightTex;					//ライトディフューズ
	std::shared_ptr<Texture2D> pMaterialMaskTex;		//特殊処理マスク(R成分 : SSSSS,G成分 : クックトランス)
	DWORD dwNumFace;//そのマテリアルであるポリゴン数
	Material() {
		Ka = { 1.0f,1.0f,1.0f,1.0f };
		Kd = { 1.0f,1.0f,1.0f,1.0f };
		Ks = { 1.0f,1.0f,1.0f,1.0f };
		dwNumFace = 0;
		pDefaultTex = nullptr;
		pShadowTex = nullptr;
		pLightTex = nullptr;
		pMaterialMaskTex = nullptr;
	}
};

struct Mesh3D {
	ID3D11Buffer* vVertexBuf;			//頂点バッファ
	ID3D11Buffer** vppIndexBuffer;		//インデックスバッファ(マテリアル数分作成される)
	Material* vpMaterial;					//マテリアル情報
	std::string dir;							//ディレクトリ名
	VERTEX3D* vpVertexData;				//頂点データを保持する
	cAnimationData* vpAnimetion;		//アニメーション情報
	int** ppiIndex;							//インデックスデータ
	int vMaterialCnt;							//マテリアルをいくつ持ってるか？
	int vTotalFace;							//メッシュが持つ総ポリゴン数

	Mesh3D() {
		vVertexBuf = nullptr;
		vppIndexBuffer = nullptr;
		vpMaterial = nullptr;
		vpVertexData = nullptr;
		ppiIndex = nullptr;
		vpAnimetion = nullptr;
	}

	void Draw() {
		//バーテックスバッファーをセット（バーテックスバッファーは一つでいい）
		UINT stride = sizeof(VERTEX3D);
		UINT offset = 0;
		GetDirectX::Context()->IASetVertexBuffers(0, 1, &vVertexBuf, &stride, &offset);

		//マテリアルの数だけ、それぞれのマテリアルのインデックスバッファ－を描画
		for (int i = 0; i<vMaterialCnt; i++)
		{
			//使用されていないマテリアル対策
			if (vpMaterial[i].dwNumFace == 0)
			{
				continue;
			}

			GetDirectX::Context()->IASetIndexBuffer(vppIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

			//プリミティブ・トポロジーをセット
			GetDirectX::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//テクスチャーをシェーダーに渡す
			if (vpMaterial[i].pDefaultTex != NULL)
			{
				GetDirectX::Context()->PSSetShaderResources(0, 1, &vpMaterial[i].pDefaultTex->vpTexture);
				GetDirectX::Context()->VSSetShaderResources(0, 1, &vpMaterial[i].pDefaultTex->vpTexture);
			}

			//Draw
			GetDirectX::Context()->DrawIndexed(vpMaterial[i].dwNumFace * 3, 0, 0);
		}
	}

	//解放処理
	~Mesh3D() {
		if (vVertexBuf) {
			vVertexBuf->Release();
		}
		for (int i = 0; i < vMaterialCnt; i++) {
			if (vppIndexBuffer[i])
				vppIndexBuffer[i]->Release();
			delete[] ppiIndex[i];
		}
		delete vpAnimetion;
		delete[] vpMaterial;
		delete[] ppiIndex;
		delete[] vpVertexData;
	}
};

struct ConstantBone {
	DirectX::XMFLOAT4X4 m[128];
};

//メッシュデータを管理する
//必要機能
//そのファイル名がmapに存在するか？
//ファイルがロードされたら名前を確認してすでに存在すればそれをshared_ptrを渡す
//毎フレーム確保しているshared_ptrを確認してカウンタがmapのみ(つまり1)だったらmapからshared_ptrを削除するとデストラクタを発動してくれる

//ファイル名が渡されたらまず拡張子を確認してそれに対応したローダーを呼び出す

//MainFrameをフレンド化してデストラクタ判定するといいかも
class MeshManager {
public:
	friend class cDirectX11;
	MeshManager(const MeshManager&) = delete;
	MeshManager& operator=(const MeshManager&) = delete;
	MeshManager(MeshManager&&) = delete;
	MeshManager& operator=(MeshManager&&) = delete;

	static MeshManager& GetInstance() {
		static MeshManager inst;			// privateなコンストラクタを呼び出す
		return inst;
	}
	std::shared_ptr<Mesh3D> LoadMeshData(std::string _fileName);	//読み込み関数		TODO:
	void SetBoneConstant(const BoneData* _bone, const int _Num,int pass = 2);	//ボーンデータをシェーダーにセットする
private:
	MeshManager() = default;					//コンストラクタを private に置く
	~MeshManager() = default;					//デストラクタを private に置く
	void CheckDelete();		//これでデータが消えたか確認する

	std::map<std::string, std::shared_ptr<Mesh3D>> m_MeshMap;		//Meshを保持するマップ
	cConstBuf<ConstantBone> m_Bone;
};