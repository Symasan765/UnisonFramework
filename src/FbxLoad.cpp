/*=================================================
//								FILE : FbxLoad.cpp
//		ファイル説明 :
//		FBXをロードするローダー
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "FbxLoad.h"
#include "Vertex.h"
#include "TextureManager.h"
#include "DirectX11.h"

HRESULT cFbxLoad::GetFBXLoader(Mesh3D * mesh, std::string _fileName)
{
	//FBXの初期化
	if (FAILED(Init(_fileName))) {
		MessageBox(0, "FBX Init失敗！！", NULL, MB_OK);
		return E_FAIL;
	}

	//--- ルートノードを取得 ---//
	FbxNode* rootNode = m_pScene->GetRootNode();

	if (NULL != rootNode) {
		//--- ルートノードの子ノード数を取得 ---//
		int childCount = rootNode->GetChildCount();

		//--- 子ノードの数だけ探査をする ---//
		for (int i = 0; childCount > i; i++) {
			GetMesh(rootNode->GetChild(i),mesh);
		}
	}

	//最後にFBXマネージャを解放するとFBX関連オブジェクトが解放される
	if (m_pManager) {
		m_pManager->Destroy();
	}
	m_pManager = nullptr;
	return S_OK;

	//マテリアル情報を取得
}


HRESULT cFbxLoad::Init(std::string _fileName) {
	//マネージャー作成
	m_pManager = FbxManager::Create();
	//インポータ作成
	FbxImporter* importer = FbxImporter::Create(m_pManager, "");
	importer->Initialize(_fileName.c_str(), -1);

	//シーン作成
	m_pScene = FbxScene::Create(m_pManager, "");
	importer->Import(m_pScene);

	auto meshCount = this->m_pScene->GetMemberCount<FbxMesh>();

	//現段階で1メッシュモデルのみに対応
	if (meshCount != 1) {
		MessageBox(0, "メッシュ数が1じゃない！", NULL, MB_OK);
	}

	//インポータは以降不要
	importer->Destroy();

	return S_OK;
}

void cFbxLoad::GetMesh(FbxNode* node, Mesh3D* meshdata) {
	//--- ノードの属性を取得 ---//
	FbxNodeAttribute* attr = node->GetNodeAttribute();

	if (NULL != attr) {
		//--- 属性の判別 ---//
		switch (attr->GetAttributeType()) {
			//--- メッシュノード発見 ---//
		case FbxNodeAttribute::eMesh:
			//----------------------------------------
			// メッシュが見つかったので必要な情報を取得
			//----------------------------------------
			FbxMesh* mesh = node->GetMesh();
			GetMeshData(mesh, meshdata);
			break;
		}
	}

	//--- 子ノードの再帰探査 ---//
	int childCount = node->GetChildCount();
	for (int i = 0; childCount > i; i++) {
		GetMesh(node->GetChild(i),meshdata);
	}
}

void cFbxLoad::GetMeshData(FbxMesh* pFbxMesh, Mesh3D* meshdata) {
	//事前に頂点数、ポリゴン数等を調べる
	int m_VertexCount = pFbxMesh->GetControlPointsCount();	//頂点の数
	int m_UVCount = pFbxMesh->GetTextureUVCount();			//UVの数
	int m_FaceCount = pFbxMesh->GetPolygonCount();			//ポリゴン(フェイス)の数
	meshdata->vTotalFace = m_FaceCount;

	//一時的なメモリ確保（頂点バッファとインデックスバッファ）
	VERTEX3D* pvVB = NULL;
	m_VertexCount<m_UVCount ? pvVB = new VERTEX3D[m_UVCount] : pvVB = new VERTEX3D[m_VertexCount];
	memset(pvVB, 0, sizeof(pvVB));
	////ポリゴンごとに　頂点読み込み 、法線読み込み、UV読み込み
	FbxVector4 Normal;

	for (int i = 0; i<m_FaceCount; i++)
	{
		int iIndex0 = 0;
		int iIndex1 = 0;
		int iIndex2 = 0;

		int iStartIndex = pFbxMesh->GetPolygonVertexIndex(i);
		int* piIndex = pFbxMesh->GetPolygonVertices();//（頂点インデックス）読み込み
		iIndex0 = piIndex[iStartIndex];
		iIndex1 = piIndex[iStartIndex + 1];
		iIndex2 = piIndex[iStartIndex + 2];
		if (m_VertexCount<m_UVCount)//UV数のほうが多い場合は本来の（頂点ベース）インデックスを利用しない。UVインデックスを基準にする
		{
			iIndex0 = pFbxMesh->GetTextureUVIndex(i, 0, FbxLayerElement::eTextureDiffuse);
			iIndex1 = pFbxMesh->GetTextureUVIndex(i, 1, FbxLayerElement::eTextureDiffuse);
			iIndex2 = pFbxMesh->GetTextureUVIndex(i, 2, FbxLayerElement::eTextureDiffuse);
		}
		//頂点
		FbxVector4* Coord = pFbxMesh->GetControlPoints();
		int index = pFbxMesh->GetPolygonVertex(i, 0);
		pvVB[iIndex0].vPos.x = -Coord[index][0];
		pvVB[iIndex0].vPos.y = Coord[index][1];
		pvVB[iIndex0].vPos.z = Coord[index][2];
		pvVB[iIndex0].vPos.w = 1.0f;

		index = pFbxMesh->GetPolygonVertex(i, 1);
		pvVB[iIndex1].vPos.x = -Coord[index][0];
		pvVB[iIndex1].vPos.y = Coord[index][1];
		pvVB[iIndex1].vPos.z = Coord[index][2];
		pvVB[iIndex1].vPos.w = 1.0f;

		index = pFbxMesh->GetPolygonVertex(i, 2);
		pvVB[iIndex2].vPos.x = -Coord[index][0];
		pvVB[iIndex2].vPos.y = Coord[index][1];
		pvVB[iIndex2].vPos.z = Coord[index][2];
		pvVB[iIndex2].vPos.w = 1.0f;
		//法線		
		pFbxMesh->GetPolygonVertexNormal(i, 0, Normal);
		pvVB[iIndex0].vNorm.x = -Normal[0];
		pvVB[iIndex0].vNorm.y = Normal[1];
		pvVB[iIndex0].vNorm.z = Normal[2];
		pvVB[iIndex0].vNorm.w = 0.0f;

		pFbxMesh->GetPolygonVertexNormal(i, 1, Normal);
		pvVB[iIndex1].vNorm.x = -Normal[0];
		pvVB[iIndex1].vNorm.y = Normal[1];
		pvVB[iIndex1].vNorm.z = Normal[2];
		pvVB[iIndex1].vNorm.w = 0.0f;

		pFbxMesh->GetPolygonVertexNormal(i, 2, Normal);
		pvVB[iIndex2].vNorm.x = -Normal[0];
		pvVB[iIndex2].vNorm.y = Normal[1];
		pvVB[iIndex2].vNorm.z = Normal[2];
		pvVB[iIndex2].vNorm.w = 0.0f;
		//テクスチャー座標
		int UVindex = pFbxMesh->GetTextureUVIndex(i, 0, FbxLayerElement::eTextureDiffuse);
		FbxLayerElementUV* pUV = 0;

		FbxLayerElementUV* uv = pFbxMesh->GetLayer(0)->GetUVs();

		if (m_UVCount && uv->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
		{
			//この取得の仕方は、UVマッピングモードがeBY_POLYGON_VERTEXの時じゃないとできない
			UVindex = pFbxMesh->GetTextureUVIndex(i, 0, FbxLayerElement::eTextureDiffuse);
			pUV = pFbxMesh->GetLayer(0)->GetUVs();
			FbxVector2 v2 = pUV->GetDirectArray().GetAt(UVindex);
			pvVB[iIndex0].vTex.x = v2[0];
			pvVB[iIndex0].vTex.y = 1.0f - v2[1];

			UVindex = pFbxMesh->GetTextureUVIndex(i, 1, FbxLayerElement::eTextureDiffuse);
			v2 = pUV->GetDirectArray().GetAt(UVindex);
			pvVB[iIndex1].vTex.x = v2[0];
			pvVB[iIndex1].vTex.y = 1.0f - v2[1];

			UVindex = pFbxMesh->GetTextureUVIndex(i, 2, FbxLayerElement::eTextureDiffuse);
			v2 = pUV->GetDirectArray().GetAt(UVindex);
			pvVB[iIndex2].vTex.x = v2[0];
			pvVB[iIndex2].vTex.y = 1.0f - v2[1];
		}
	}

	//===========================ここに課題用に球作成入れるか===========================
	DirectX::XMFLOAT3 maxS = { 0,0,0 };		//こいつら残せばバウンディングボックスに流用できる
	DirectX::XMFLOAT3 minS = { 0,0,0 };
	for (int no = 0; no < m_VertexCount; no++) {
		if (maxS.x < pvVB[no].vPos.x) maxS.x = pvVB[no].vPos.x;
		if (maxS.y < pvVB[no].vPos.y) maxS.y = pvVB[no].vPos.y;
		if (maxS.z < pvVB[no].vPos.z) maxS.z = pvVB[no].vPos.z;

		if (minS.x > pvVB[no].vPos.x) minS.x = pvVB[no].vPos.x;
		if (minS.y > pvVB[no].vPos.y) maxS.y = pvVB[no].vPos.y;
		if (minS.z > pvVB[no].vPos.z) maxS.z = pvVB[no].vPos.z;
	}

	//=======================================================================

	FbxLayerElementUV* uv = pFbxMesh->GetLayer(0)->GetUVs();
	if (m_UVCount && uv->GetMappingMode() == FbxLayerElement::eByControlPoint)
	{
		FbxLayerElementUV* pUV = pFbxMesh->GetLayer(0)->GetUVs();
		for (int k = 0; k<m_UVCount; k++)
		{
			FbxVector2 v2;
			v2 = pUV->GetDirectArray().GetAt(k);
			pvVB[k].vTex.x = v2[0];
			pvVB[k].vTex.y = 1.0f - v2[1];
		}
	}

	//マテリアル読み込み
	FbxNode* pNode = pFbxMesh->GetNode();
	meshdata->vMaterialCnt = pNode->GetMaterialCount();

	meshdata->vpMaterial = new Material[meshdata->vMaterialCnt];

	//マテリアルの数だけインデックスバッファーを作成
	meshdata->vppIndexBuffer = new ID3D11Buffer*[meshdata->vMaterialCnt];
	meshdata->ppiIndex = new int*[meshdata->vMaterialCnt];
	for (int i = 0; i<meshdata->vMaterialCnt; i++)
	{
		//フォンモデルを想定
		FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);
		FbxSurfacePhong* pPhong = (FbxSurfacePhong*)pMaterial;

		//環境光
		FbxDouble3 d3Ambient = pPhong->Ambient;
		meshdata->vpMaterial[i].Ka.x = (float)d3Ambient[0];
		meshdata->vpMaterial[i].Ka.y = (float)d3Ambient[1];
		meshdata->vpMaterial[i].Ka.z = (float)d3Ambient[2];
		//拡散反射光
		FbxDouble3 d3Diffuse = pPhong->Diffuse;
		meshdata->vpMaterial[i].Kd.x = (float)d3Diffuse[0];
		meshdata->vpMaterial[i].Kd.y = (float)d3Diffuse[1];
		meshdata->vpMaterial[i].Kd.z = (float)d3Diffuse[2];
		//鏡面反射光
		FbxDouble3 d3Specular = pPhong->Diffuse;
		meshdata->vpMaterial[i].Ks.x = (float)d3Specular[0];
		meshdata->vpMaterial[i].Ks.y = (float)d3Specular[1];
		meshdata->vpMaterial[i].Ks.z = (float)d3Specular[2];
		//テクスチャー（ディフューズテクスチャーのみ）
		FbxProperty lProperty;
		lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		//FbxTexture* texture=FbxCast<KFbxTexture>(lProperty.GetSrcObject(FbxTexture::ClassId, 0));
		//FbxTexture* texture = NULL;
		FbxFileTexture* texture = m_pScene->GetSrcObject<FbxFileTexture>(i);
		meshdata->vpMaterial[i].pDefaultTex = nullptr;
		if (texture)
		{
			//strcpy_s(m_pMaterial[i].szTextureName, texture->GetName());

			char tName[500];
			strcpy_s(tName, texture->GetRelativeFileName());
			//strcpy_s(tName, texture->GetName());
			/*char ext[10];
			_splitpath(tName, 0, 0, tName, ext);
			strcat_s(tName, ext);*/

			std::string texName = meshdata->dir + tName;
			//テクスチャーを作成
			meshdata->vpMaterial[i].pDefaultTex = cTexManager::GetInstance().LoadTexData(texName, true);
		}
		//マテリアルの数だけインデックスバッファーを作成
		int iCount = 0;
		int* pIndex = new int[m_FaceCount * 3];//とりあえず、メッシュ内のポリゴン数でメモリ確保（個々のポリゴングループはかならずこれ以下になるが）

											   //そのマテリアルであるインデックス配列内の開始インデックスを調べる　さらにインデックスの個数も調べる		
		iCount = 0;
		for (int k = 0; k<m_FaceCount; k++)
		{
			FbxLayerElementMaterial* mat = pFbxMesh->GetLayer(0)->GetMaterials();//レイヤーが1枚だけを想定
			int matId = mat->GetIndexArray().GetAt(k);
			if (matId == i)
			{
				if (m_VertexCount<m_UVCount)
				{
					pIndex[iCount] = pFbxMesh->GetTextureUVIndex(k, 0, FbxLayerElement::eTextureDiffuse);
					pIndex[iCount + 1] = pFbxMesh->GetTextureUVIndex(k, 1, FbxLayerElement::eTextureDiffuse);
					pIndex[iCount + 2] = pFbxMesh->GetTextureUVIndex(k, 2, FbxLayerElement::eTextureDiffuse);
				}
				else
				{
					pIndex[iCount] = pFbxMesh->GetPolygonVertex(k, 0);
					pIndex[iCount + 1] = pFbxMesh->GetPolygonVertex(k, 1);
					pIndex[iCount + 2] = pFbxMesh->GetPolygonVertex(k, 2);
				}
				iCount += 3;
			}
		}
		CreateIndexBuffer(iCount * sizeof(int), pIndex, &meshdata->vppIndexBuffer[i]);
		meshdata->vpMaterial[i].dwNumFace = iCount / 3;//そのマテリアル内のポリゴン数

		meshdata->ppiIndex[i] = pIndex;
	}

	//バーテックスバッファーを作成
	if (m_VertexCount<m_UVCount) m_VertexCount = m_UVCount;
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX3D) * m_VertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pvVB;
	if (FAILED(GetDirectX::Device()->CreateBuffer(&bd, &InitData, &meshdata->vVertexBuf)))
		return;

	meshdata->vpVertexData = pvVB;	//頂点情報を保持しておく
	return;
}

HRESULT cFbxLoad::CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer) {
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = dwSize;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pIndex;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if (FAILED(GetDirectX::Device()->CreateBuffer(&bd, &InitData, ppIndexBuffer)))
	{
		MessageBox(0, "インデックス読み込み失敗！", NULL, MB_OK);
		return FALSE;
	}

	return S_OK;
}