/*=================================================
//								FILE : FbxLoad.cpp
//		�t�@�C������ :
//		FBX�����[�h���郍�[�_�[
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "FbxLoad.h"
#include "Vertex.h"
#include "TextureManager.h"
#include "DirectX11.h"

HRESULT cFbxLoad::GetFBXLoader(Mesh3D * mesh, std::string _fileName)
{
	//FBX�̏�����
	if (FAILED(Init(_fileName))) {
		MessageBox(0, "FBX Init���s�I�I", NULL, MB_OK);
		return E_FAIL;
	}

	//--- ���[�g�m�[�h���擾 ---//
	FbxNode* rootNode = m_pScene->GetRootNode();

	if (NULL != rootNode) {
		//--- ���[�g�m�[�h�̎q�m�[�h�����擾 ---//
		int childCount = rootNode->GetChildCount();

		//--- �q�m�[�h�̐������T�������� ---//
		for (int i = 0; childCount > i; i++) {
			GetMesh(rootNode->GetChild(i),mesh);
		}
	}

	//�Ō��FBX�}�l�[�W������������FBX�֘A�I�u�W�F�N�g����������
	if (m_pManager) {
		m_pManager->Destroy();
	}
	m_pManager = nullptr;
	return S_OK;

	//�}�e���A�������擾
}


HRESULT cFbxLoad::Init(std::string _fileName) {
	//�}�l�[�W���[�쐬
	m_pManager = FbxManager::Create();
	//�C���|�[�^�쐬
	FbxImporter* importer = FbxImporter::Create(m_pManager, "");
	importer->Initialize(_fileName.c_str(), -1);

	//�V�[���쐬
	m_pScene = FbxScene::Create(m_pManager, "");
	importer->Import(m_pScene);

	auto meshCount = this->m_pScene->GetMemberCount<FbxMesh>();

	//���i�K��1���b�V�����f���݂̂ɑΉ�
	if (meshCount != 1) {
		MessageBox(0, "���b�V������1����Ȃ��I", NULL, MB_OK);
	}

	//�C���|�[�^�͈ȍ~�s�v
	importer->Destroy();

	return S_OK;
}

void cFbxLoad::GetMesh(FbxNode* node, Mesh3D* meshdata) {
	//--- �m�[�h�̑������擾 ---//
	FbxNodeAttribute* attr = node->GetNodeAttribute();

	if (NULL != attr) {
		//--- �����̔��� ---//
		switch (attr->GetAttributeType()) {
			//--- ���b�V���m�[�h���� ---//
		case FbxNodeAttribute::eMesh:
			//----------------------------------------
			// ���b�V�������������̂ŕK�v�ȏ����擾
			//----------------------------------------
			FbxMesh* mesh = node->GetMesh();
			GetMeshData(mesh, meshdata);
			break;
		}
	}

	//--- �q�m�[�h�̍ċA�T�� ---//
	int childCount = node->GetChildCount();
	for (int i = 0; childCount > i; i++) {
		GetMesh(node->GetChild(i),meshdata);
	}
}

void cFbxLoad::GetMeshData(FbxMesh* pFbxMesh, Mesh3D* meshdata) {
	//���O�ɒ��_���A�|���S�������𒲂ׂ�
	int m_VertexCount = pFbxMesh->GetControlPointsCount();	//���_�̐�
	int m_UVCount = pFbxMesh->GetTextureUVCount();			//UV�̐�
	int m_FaceCount = pFbxMesh->GetPolygonCount();			//�|���S��(�t�F�C�X)�̐�
	meshdata->vTotalFace = m_FaceCount;

	//�ꎞ�I�ȃ������m�ہi���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�j
	VERTEX3D* pvVB = NULL;
	m_VertexCount<m_UVCount ? pvVB = new VERTEX3D[m_UVCount] : pvVB = new VERTEX3D[m_VertexCount];
	memset(pvVB, 0, sizeof(pvVB));
	////�|���S�����ƂɁ@���_�ǂݍ��� �A�@���ǂݍ��݁AUV�ǂݍ���
	FbxVector4 Normal;

	for (int i = 0; i<m_FaceCount; i++)
	{
		int iIndex0 = 0;
		int iIndex1 = 0;
		int iIndex2 = 0;

		int iStartIndex = pFbxMesh->GetPolygonVertexIndex(i);
		int* piIndex = pFbxMesh->GetPolygonVertices();//�i���_�C���f�b�N�X�j�ǂݍ���
		iIndex0 = piIndex[iStartIndex];
		iIndex1 = piIndex[iStartIndex + 1];
		iIndex2 = piIndex[iStartIndex + 2];
		if (m_VertexCount<m_UVCount)//UV���̂ق��������ꍇ�͖{���́i���_�x�[�X�j�C���f�b�N�X�𗘗p���Ȃ��BUV�C���f�b�N�X����ɂ���
		{
			iIndex0 = pFbxMesh->GetTextureUVIndex(i, 0, FbxLayerElement::eTextureDiffuse);
			iIndex1 = pFbxMesh->GetTextureUVIndex(i, 1, FbxLayerElement::eTextureDiffuse);
			iIndex2 = pFbxMesh->GetTextureUVIndex(i, 2, FbxLayerElement::eTextureDiffuse);
		}
		//���_
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
		//�@��		
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
		//�e�N�X�`���[���W
		int UVindex = pFbxMesh->GetTextureUVIndex(i, 0, FbxLayerElement::eTextureDiffuse);
		FbxLayerElementUV* pUV = 0;

		FbxLayerElementUV* uv = pFbxMesh->GetLayer(0)->GetUVs();

		if (m_UVCount && uv->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
		{
			//���̎擾�̎d���́AUV�}�b�s���O���[�h��eBY_POLYGON_VERTEX�̎�����Ȃ��Ƃł��Ȃ�
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

	//===========================�����ɉۑ�p�ɋ��쐬����邩===========================
	DirectX::XMFLOAT3 maxS = { 0,0,0 };		//������c���΃o�E���f�B���O�{�b�N�X�ɗ��p�ł���
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

	//�}�e���A���ǂݍ���
	FbxNode* pNode = pFbxMesh->GetNode();
	meshdata->vMaterialCnt = pNode->GetMaterialCount();

	meshdata->vpMaterial = new Material[meshdata->vMaterialCnt];

	//�}�e���A���̐������C���f�b�N�X�o�b�t�@�[���쐬
	meshdata->vppIndexBuffer = new ID3D11Buffer*[meshdata->vMaterialCnt];
	meshdata->ppiIndex = new int*[meshdata->vMaterialCnt];
	for (int i = 0; i<meshdata->vMaterialCnt; i++)
	{
		//�t�H�����f����z��
		FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);
		FbxSurfacePhong* pPhong = (FbxSurfacePhong*)pMaterial;

		//����
		FbxDouble3 d3Ambient = pPhong->Ambient;
		meshdata->vpMaterial[i].Ka.x = (float)d3Ambient[0];
		meshdata->vpMaterial[i].Ka.y = (float)d3Ambient[1];
		meshdata->vpMaterial[i].Ka.z = (float)d3Ambient[2];
		//�g�U���ˌ�
		FbxDouble3 d3Diffuse = pPhong->Diffuse;
		meshdata->vpMaterial[i].Kd.x = (float)d3Diffuse[0];
		meshdata->vpMaterial[i].Kd.y = (float)d3Diffuse[1];
		meshdata->vpMaterial[i].Kd.z = (float)d3Diffuse[2];
		//���ʔ��ˌ�
		FbxDouble3 d3Specular = pPhong->Diffuse;
		meshdata->vpMaterial[i].Ks.x = (float)d3Specular[0];
		meshdata->vpMaterial[i].Ks.y = (float)d3Specular[1];
		meshdata->vpMaterial[i].Ks.z = (float)d3Specular[2];
		//�e�N�X�`���[�i�f�B�t���[�Y�e�N�X�`���[�̂݁j
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
			//�e�N�X�`���[���쐬
			meshdata->vpMaterial[i].pDefaultTex = cTexManager::GetInstance().LoadTexData(texName, true);
		}
		//�}�e���A���̐������C���f�b�N�X�o�b�t�@�[���쐬
		int iCount = 0;
		int* pIndex = new int[m_FaceCount * 3];//�Ƃ肠�����A���b�V�����̃|���S�����Ń������m�ہi�X�̃|���S���O���[�v�͂��Ȃ炸����ȉ��ɂȂ邪�j

											   //���̃}�e���A���ł���C���f�b�N�X�z����̊J�n�C���f�b�N�X�𒲂ׂ�@����ɃC���f�b�N�X�̌������ׂ�		
		iCount = 0;
		for (int k = 0; k<m_FaceCount; k++)
		{
			FbxLayerElementMaterial* mat = pFbxMesh->GetLayer(0)->GetMaterials();//���C���[��1��������z��
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
		meshdata->vpMaterial[i].dwNumFace = iCount / 3;//���̃}�e���A�����̃|���S����

		meshdata->ppiIndex[i] = pIndex;
	}

	//�o�[�e�b�N�X�o�b�t�@�[���쐬
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

	meshdata->vpVertexData = pvVB;	//���_����ێ����Ă���
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
		MessageBox(0, "�C���f�b�N�X�ǂݍ��ݎ��s�I", NULL, MB_OK);
		return FALSE;
	}

	return S_OK;
}