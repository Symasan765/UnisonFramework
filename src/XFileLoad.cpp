/*=================================================
//								FILE : XFileLoad.cpp
//		ファイル説明 :
//		Xファイルを読み込み、データとして読み取っていく
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "XFileLoad.h"
#include "TextureManager.h"

using namespace DirectX;
using namespace std;

//Xファイルを読み込む際に必要な情報を一時的に貯めておく構造体を隠す目的で利用する
namespace {
	//頂点情報を貯めておく
	struct XFileVertex {
		DirectX::XMFLOAT4 vPos;
		DirectX::XMFLOAT4 vNormal;
		DirectX::XMFLOAT2 vUV;
		DirectX::XMUINT4 vBoneNo;
		DirectX::XMFLOAT4 vWeight;

		int vBoneCnt;		//ボーンのカウント

		XFileVertex() {
			vPos = { 0.0f,0.0f,0.0f,1.0f };
			vNormal = { 0.0f,0.0f,0.0f,0.0f };
			vUV = { 0.0f,0.0f };
			vBoneNo = { 128,128,128,128 };
			vWeight = { 0.0f,0.0f,0.0f,0.0f };
			vBoneCnt = 0;
		}
	};

	struct XFileFace {
		XFileVertex* vVertex;			//頂点情報(頂点数分確保する)
		DirectX::XMINT3* vIndex;		//頂点インデックス(面数分確保する)
		int vVertexNum;		//頂点数
		int vFaceNum;			//面数

		XFileFace() {
			vVertex = nullptr;
			vIndex = nullptr;
			vVertexNum = 0;
			vFaceNum = 0;
		}

		~XFileFace() {
			delete[] vVertex;
			delete[] vIndex;
		}
	};

	struct XFileMaterial {
		DirectX::XMFLOAT4 vColor;
		DirectX::XMFLOAT4 vSpecular;
		std::string vTexFileName;		//テクスチャのファイル名。なければ"NONE"が入る
		float vPow;		//鏡面反射の強さ。まだ使ってないが将来を考え取得しておく

		XFileMaterial() {
			vColor = { 0.0f,0.0f,0.0f,1.0f };
			vSpecular = { 0.0f,0.0f,0.0f,1.0f };
			vTexFileName = "NONE";
			vPow = 0;
		}
	};

	//マテリアル情報を貯めておく
	struct XFileMaterialList {
		int vMaterilNum;		//マテリアル数を保存しておく
		int vTotalFaceNum;				//マテリアル面数(総面数と一緒のことが多いけど1とかで全部のポリゴンにそのマテリアルを指定とかもあるっぽい)
		int* vMaterialNo;		//面数分の領域を確保してその中に何番のマテリアルを使うか格納していく
		int* vFaceNum;			//マテリアル数分の領域を確保してその中に面数をカウントしていく
		XFileMaterial* vMaterialData;		//マテリアルのカラー情報を保持

		XFileMaterialList() {
			vMaterilNum = 0;
			vTotalFaceNum = 0;
			vMaterialNo = nullptr;
			vFaceNum = nullptr;
			vMaterialData = nullptr;
		}

		~XFileMaterialList() {
			delete[] vMaterialData;
			delete[] vFaceNum;
		}
	};

	//アニメーション情報を貯めておく
	struct XFileAnime {

	};


}

struct XFileLoadMeshBuf {
	XFileFace vFace;
	XFileMaterialList vMat;
};

/// <summary>
/// ここを呼び出してXファイルのロードを行う
/// </summary>
/// <param name="_FileName"></param>
/// <param name="_pBone"></param>
void cXFileLoad::GetXFile(Mesh3D* pMesh, std::string _FileName)
{
	InitData();		//読み込むために読み取り変数を初期化する

	//Xファイルに含まれる情報を取得する
	m_Type = CheckType(_FileName);
	m_Type &= 2;	//2はワンスキン情報が含まれているか？

	ifstream pFP(_FileName);		//読み込んだファイルの情報を扱う
	SearchFrame(&pFP);				//"Frame"を探して来てもらう

	if (m_Type) {
		Bone _pBone;
		GetFrame(&pFP, &_pBone, "NONE_PARENT");
		GetAnimation(&pFP, pMesh->vpAnimetion);
	}

	XFileLoadMeshBuf Data;
	GetMesh(_FileName, &Data);
	CreateMesh3D(pMesh, &Data);
	int a = 10;
}

void cXFileLoad::InitData()
{
	m_Suffix = 0;
	m_MotionNum = 0;

	//最大ボーン数分だけ初期化
	for (int i = 0; i < 128; i++) {
		m_Bones[i].vParentNo = -1;
		m_Bones[i].vName = "UninitializedName";
		XMStoreFloat4x4(&m_Bones[i].vMat, XMMatrixIdentity());		//単位行列を入れておく
		m_Bones[i].vBind = m_Bones[i].vMat;
		m_BoneInMotion[i] = -1;
	}
}

/// <summary>
/// メッシュ情報を見つけて取得する
/// </summary>
/// <param name="_FileName"></param>
/// <param name="_output"></param>
void cXFileLoad::GetMesh(std::string _FileName, XFileLoadMeshBuf * _output)
{
	ifstream pFP(_FileName);		//読み込んだファイルの情報を扱う
	string buf;		//文字をここに読んでいく

	while (buf != "Mesh" && !(pFP.eof())) {
		if (buf == "template") {
			getline(pFP, buf);		//templateは一行飛ばす
		}
		else {
			pFP >> buf;
		}
	}
	getline(pFP, buf);		//Meshの名前は必要ないので一行飛ばす
	//まずは"Mesh"の情報として定型の頂点数、頂点座標、インデックスを取り出す
	getline(pFP, buf, ';');		//ここに入るのは頂点数
	_output->vFace.vVertexNum = std::stod(buf);
	_output->vFace.vVertex = new XFileVertex[_output->vFace.vVertexNum];		//頂点分のデータ領域を確保

	//頂点数分、データを取得して行く。Xファイルの最後は;なのでそこまで来たら終わりに
	for (int i = 0; buf != ";"; i++) {
		getline(pFP, buf);	//改行を飛ばす

		getline(pFP, buf, ';');		//X座標
		_output->vFace.vVertex[i].vPos.x = (float)std::stod(buf);

		getline(pFP, buf, ';');		//Y座標
		_output->vFace.vVertex[i].vPos.y = (float)std::stod(buf);

		getline(pFP, buf, ';');		//Z座標
		_output->vFace.vVertex[i].vPos.z = (float)std::stod(buf);

		_output->vFace.vVertex[i].vPos.w = 1.0f;
		pFP >> buf;	//まだ継続してたら","が、終わっていたら";"が格納される
	}
	getline(pFP, buf);	//改行を飛ばす

	//この次に来るのは面数のはず
	getline(pFP, buf, ';');
	_output->vFace.vFaceNum = std::stod(buf);
	_output->vFace.vIndex = new XMINT3[_output->vFace.vFaceNum];	//領域確保
	getline(pFP, buf);	//改行しておく
	//面数分、データをロード
	for (int i = 0; i < _output->vFace.vFaceNum; i++) {
		//インデックスは初めにポリゴンの頂点数が来る
		getline(pFP, buf, ';');		// TODO 4角ポリゴンならエラーを出す

		getline(pFP, buf, ',');		//1点目
		_output->vFace.vIndex[i].x = std::stod(buf);

		getline(pFP, buf, ',');		//2点目
		_output->vFace.vIndex[i].y = std::stod(buf);

		getline(pFP, buf, ';');		//3点目。ここだけ';'で終わる
		_output->vFace.vIndex[i].z = std::stod(buf);

		getline(pFP, buf);	//改行しておく
	}

	//===================ここまでがMeshから必ず取り出せる情報======================

	//Meshの閉じカッコを見つけるまで繰り返し処理
	while (buf != "}" && !(pFP.eof())) {
		// TODO 今後読み込むものが増えたときはここを更新すればいい
		pFP >> buf;		//型を読み込む

		//分岐処理。どの関数も次に読んだら文字が入るように記述すること
		if (buf == "MeshNormals") {	//法線
			getline(pFP, buf);	//改行しておく
			GetNormal(&pFP, _output);		//法線を取得してくる
		}
		else if (buf == "MeshTextureCoords") {		//UV
			getline(pFP, buf);	//改行しておく
			GetUV(&pFP, _output);		//UVを取得してくる
		}
		else if (buf == "MeshMaterialList") {
			getline(pFP, buf);	//改行しておく
			GetMaterialList(&pFP, _output);		//マテリアル情報を取得してくる
		}
		else if (buf == "XSkinMeshHeader") {
			getline(pFP, buf);	//改行しておく
			GetMeshBone(&pFP, _output);
		}
		else {
			SkipScope(&pFP);
		}
	}

	pFP >> buf;
}

/// <summary>
/// 取得したデータからメッシュ情報を組み立てる
/// </summary>
/// <param name="pMesh"></param>
/// <param name=""></param>
void cXFileLoad::CreateMesh3D(Mesh3D * pMesh, XFileLoadMeshBuf *data)
{
	//まずは各情報を元にしてデータ領域を確保する
	pMesh->vTotalFace = data->vFace.vFaceNum;	//面数
	pMesh->vpVertexData = new VERTEX3D[data->vFace.vVertexNum];
	pMesh->vMaterialCnt = data->vMat.vMaterilNum;		//マテリアル数
	//インデックスの作成
	pMesh->ppiIndex = new int*[data->vMat.vMaterilNum];	//マテリアル数分のインデックス
	//マテリアル領域確保
	pMesh->vpMaterial = new Material[data->vMat.vMaterilNum];
	for (int i = 0; i < data->vMat.vMaterilNum; i++) {
		//各マテリアルに入っている面数分だけ領域を確保
		//pMesh->ppiIndex[i] = new int[data->vMat.vMaterialNo[i] * 3];		//頂点数分なので *3
		pMesh->ppiIndex[i] = new int[data->vMat.vFaceNum[i] * 3];		//頂点数分なので *3
		pMesh->vpMaterial[i].dwNumFace = data->vMat.vFaceNum[i];			//マテリアル毎のポリゴン数
	}

	//まずは頂点座標、法線、UVを格納する
	for (int i = 0; i < data->vFace.vVertexNum; i++) {
		//座標
		pMesh->vpVertexData[i].vPos.x = data->vFace.vVertex[i].vPos.x;
		pMesh->vpVertexData[i].vPos.y = data->vFace.vVertex[i].vPos.y;
		pMesh->vpVertexData[i].vPos.z = data->vFace.vVertex[i].vPos.z;
		pMesh->vpVertexData[i].vPos.w = data->vFace.vVertex[i].vPos.w;

		//法線
		pMesh->vpVertexData[i].vNorm.x = data->vFace.vVertex[i].vNormal.x;
		pMesh->vpVertexData[i].vNorm.y = data->vFace.vVertex[i].vNormal.y;
		pMesh->vpVertexData[i].vNorm.z = data->vFace.vVertex[i].vNormal.z;
		pMesh->vpVertexData[i].vNorm.w = data->vFace.vVertex[i].vNormal.w;

		//UV
		pMesh->vpVertexData[i].vTex.x = data->vFace.vVertex[i].vUV.x;
		pMesh->vpVertexData[i].vTex.y = data->vFace.vVertex[i].vUV.y;

		//ボーン番号
		pMesh->vpVertexData[i].vBones = data->vFace.vVertex[i].vBoneNo;

		//重み
		pMesh->vpVertexData[i].vWeight = data->vFace.vVertex[i].vWeight;
	}

	//次にインデックス情報を取得していく
	int* VertexCnt = new int[pMesh->vMaterialCnt]{ 0 };
	for (int i = 0; i < data->vMat.vTotalFaceNum; i++) {
		int Index = data->vMat.vMaterialNo[i];		//i番目の面がどのマテリアルを使うか判定
		if (pMesh->vpMaterial[Index].dwNumFace != 0) {
			pMesh->ppiIndex[Index][VertexCnt[Index]] = data->vFace.vIndex[i].x;
			pMesh->ppiIndex[Index][VertexCnt[Index] + 1] = data->vFace.vIndex[i].y;
			pMesh->ppiIndex[Index][VertexCnt[Index] + 2] = data->vFace.vIndex[i].z;
			VertexCnt[Index] += 3;
		}
	}
	delete[] VertexCnt;


	pMesh->vppIndexBuffer = new ID3D11Buffer*[pMesh->vMaterialCnt];
	for (int i = 0; i < pMesh->vMaterialCnt; i++) {
		//インデックスバッファの作成
		//万が一、マテリアル数はカウントされていてもそのマテリアルが使われていなかった場合の処理
		if (pMesh->vpMaterial[i].dwNumFace == 0) {

		}
		else {
			CreateIndexBuffer((pMesh->vpMaterial[i].dwNumFace * 3) * sizeof(int), pMesh->ppiIndex[i], &pMesh->vppIndexBuffer[i]);
		}
	}

	//マテリアル情報を読んでいく
	for (int i = 0; i < pMesh->vMaterialCnt; i++) {
		pMesh->vpMaterial[i].Ka = data->vMat.vMaterialData[i].vColor;
		pMesh->vpMaterial[i].Kd = data->vMat.vMaterialData[i].vColor;
		pMesh->vpMaterial[i].Ks = data->vMat.vMaterialData[i].vSpecular;

		//テクスチャの読み込み
		if (data->vMat.vMaterialData[i].vTexFileName != "NONE") {
			std::string ToonFolder[4] = {
				{"Default/"},
				{"Light/"},
				{"Shadow/"},
				{ "Mask/" }
			};

			//トゥーン用テクスチャをそれぞれ読み込む
			std::string texName = pMesh->dir + ToonFolder[0] + data->vMat.vMaterialData[i].vTexFileName;
			pMesh->vpMaterial[i].pDefaultTex = cTexManager::GetInstance().LoadTexData(texName, true);

			texName = pMesh->dir + ToonFolder[1] + data->vMat.vMaterialData[i].vTexFileName;
			pMesh->vpMaterial[i].pLightTex = cTexManager::GetInstance().LoadTexData(texName, true);

			texName = pMesh->dir + ToonFolder[2] + data->vMat.vMaterialData[i].vTexFileName;
			pMesh->vpMaterial[i].pShadowTex = cTexManager::GetInstance().LoadTexData(texName, true);

			texName = pMesh->dir + ToonFolder[3] + data->vMat.vMaterialData[i].vTexFileName;
			pMesh->vpMaterial[i].pMaterialMaskTex = cTexManager::GetInstance().LoadTexData(texName, true);
		}
		else {
			pMesh->vpMaterial[i].pDefaultTex = nullptr;
		}
	}

	//ボーン情報を渡す
	if (m_Suffix > 0) {
		pMesh->vpAnimetion->m_pBoneNum = m_Suffix;		//ボーン数取得
		pMesh->vpAnimetion->m_pBone = new BoneData[m_Suffix];	//領域確保

		for (int i = 0; i < m_Suffix; i++) {
			pMesh->vpAnimetion->m_pBone[i].vInv = m_Bones[i].vMat;
			pMesh->vpAnimetion->m_pBone[i].vOffset = m_Bones[i].vBind;
			pMesh->vpAnimetion->m_pBone[i].vTransPose = m_Bones[i].vMat;
			pMesh->vpAnimetion->m_pBone[i].vName = m_Bones[i].vName;
			pMesh->vpAnimetion->m_pBone[i].vParentNo = m_Bones[i].vParentNo;
		}
	}

	//最後に頂点バッファを作成
	//バーテックスバッファーを作成
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX3D) * data->vFace.vVertexNum;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pMesh->vpVertexData;
	if (FAILED(GetDirectX::Device()->CreateBuffer(&bd, &InitData, &pMesh->vVertexBuf)))
		return;
}

/// <summary>
/// 法線情報を取得する
/// </summary>
/// <param name="_pFP"></param>
/// <param name="_output"></param>
void cXFileLoad::GetNormal(std::ifstream * _pFP, XFileLoadMeshBuf * _output)
{
	string buf;	//文字を格納するバッファ

	//法線情報として法線の数が最初に入っているが、
	//頂点数と同数になるので無視して改行してしまう
	getline(*_pFP, buf);
	int cnt = _output->vFace.vVertexNum;
	if (std::stoi(buf) != _output->vFace.vVertexNum)
		cnt = std::stoi(buf);

	//頂点数分データを保持
	for (int i = 0; i < cnt; i++) {
		//X成分
		getline(*_pFP, buf, ';');
		_output->vFace.vVertex[i].vNormal.x = (float)std::stod(buf);

		//Y成分
		getline(*_pFP, buf, ';');
		_output->vFace.vVertex[i].vNormal.y = (float)std::stod(buf);

		//Z成分
		getline(*_pFP, buf, ';');
		_output->vFace.vVertex[i].vNormal.z = (float)std::stod(buf);

		_output->vFace.vVertex[i].vNormal.w = 0.0f;
		getline(*_pFP, buf);
	}

	//法線情報の中には面の法線情報も入っているが、必要はないので次の}まで飛ばしてしまう
	while (buf != "}")
	{
		*_pFP >> buf;
	}
	getline(*_pFP, buf);		//改行。これで終わり
}

/// <summary>
/// UV座標を取得する
/// </summary>
/// <param name="_pFP"></param>
/// <param name="_output"></param>
void cXFileLoad::GetUV(std::ifstream * _pFP, XFileLoadMeshBuf * _output)
{
	string buf;	//文字を格納するバッファ

	//UV情報の中にUV数が入っているが頂点数と同じになるので飛ばしてしまう
	getline(*_pFP, buf);

	//頂点数分だけデータを取得していく
	for (int i = 0; i < _output->vFace.vVertexNum; i++) {
		//U成分
		getline(*_pFP, buf, ';');
		_output->vFace.vVertex[i].vUV.x = (float)std::stod(buf);

		//V成分
		getline(*_pFP, buf, ';');
		_output->vFace.vVertex[i].vUV.y = (float)std::stod(buf);

		getline(*_pFP, buf); //改行
	}
	getline(*_pFP, buf); //改行。これで終わり
}

/// <summary>
/// マテリアルに関する情報を取得する
/// </summary>
/// <param name="_pFP"></param>
/// <param name="_output"></param>
void cXFileLoad::GetMaterialList(std::ifstream * _pFP, XFileLoadMeshBuf * _output)
{
	string buf;

	//マテリアルには初めにマテリアル数,次にマテリアルを適用する面数が記述されている
	getline(*_pFP, buf, ';');
	_output->vMat.vMaterilNum = std::stoi(buf);
	getline(*_pFP, buf);		//改行

	//各マテリアルに何枚の面が入っているかをカウントする
	_output->vMat.vFaceNum = new int[_output->vMat.vMaterilNum];
	for (int i = 0; i < _output->vMat.vMaterilNum; i++) {
		//初期化
		_output->vMat.vFaceNum[i] = 0;
	}
	//マテリアル情報を格納する
	_output->vMat.vMaterialData = new XFileMaterial[_output->vMat.vMaterilNum];


	getline(*_pFP, buf, ';');		//ここには面数が入る
	_output->vMat.vTotalFaceNum = std::stoi(buf);
	//その面番号に何番のマテリアルを適用するかを残しておく
	_output->vMat.vMaterialNo = new int[_output->vMat.vTotalFaceNum];
	getline(*_pFP, buf);		//改行

	//面数分、繰り返し処理
	for (int i = 0; i < _output->vMat.vTotalFaceNum - 1; i++) {
		getline(*_pFP, buf, ',');		//改行
		int MateNo = stoi(buf);		//マテリアル番号
		_output->vMat.vFaceNum[MateNo]++;		//面数をカウント
		_output->vMat.vMaterialNo[i] = MateNo;	//どのマテリアルNoを使うか格納
		getline(*_pFP, buf);		//改行
	}
	//マテリアルの最後は";"なので最後の一回だけ処理を変える
	getline(*_pFP, buf, ';');		//改行
	int MateNo = stoi(buf);		//マテリアル番号
	_output->vMat.vFaceNum[MateNo]++;		//面数をカウント
	_output->vMat.vMaterialNo[_output->vMat.vTotalFaceNum - 1] = MateNo;	//どのマテリアルNoを使うか格納
	getline(*_pFP, buf);		//改行


	//ここまでがマテリアルリストの定型取得で以降はマテリアルデータの取得となる
	//マテリアル数分だけ取得
	for (int i = 0; i < _output->vMat.vMaterilNum; i++) {
		*_pFP >> buf;	//GetLineだけだと改行だけを取得する可能性があるので先に確実に型名を取得する
		getline(*_pFP, buf);		//最初のMaterialという型名は飛ばす

		//最初に来るのは面の色データ
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vColor.x = stof(buf);
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vColor.y = stof(buf);
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vColor.z = stof(buf);
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vColor.w = stof(buf);
		getline(*_pFP, buf);		//改行

		//次にくるのは鏡面反射の強さ
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vPow = stof(buf);
		getline(*_pFP, buf);		//改行

		//次が鏡面反射色
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vSpecular.x = stof(buf);
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vSpecular.y = stof(buf);
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vSpecular.z = stof(buf);

		_output->vMat.vMaterialData[i].vSpecular.w = 1.0f;
		getline(*_pFP, buf);		//改行

		//最後に発光色
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vSpecular.x = stof(buf);
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vSpecular.y = stof(buf);
		getline(*_pFP, buf, ';');
		_output->vMat.vMaterialData[i].vSpecular.z = stof(buf);

		_output->vMat.vMaterialData[i].vSpecular.w = 1.0f;
		getline(*_pFP, buf);		//改行

		//ここまでがマテリアル定型。次はテクスチャの有無を確認する
		*_pFP >> buf;
		//テクスチャだった場合
		if (buf == "TextureFilename") {
			//ダブルクォーテーションを除外してテクスチャパスを読み込む
			getline(*_pFP, buf, '"');		//改行
			getline(*_pFP, buf, '"');		//改行
			_output->vMat.vMaterialData[i].vTexFileName = buf;	//パスを保存
			*_pFP >> buf;
			getline(*_pFP, buf);		//パスの後ろの改行を飛ばし点
			*_pFP >> buf;		//テクスチャスコープの}行を飛ばす
		}
		else {
			_output->vMat.vMaterialData[i].vTexFileName = "NONE";		//テクスチャなかったよ
		}
		if (buf != "}")
			getline(*_pFP, buf);		//改行
	}
	getline(*_pFP, buf);		//改行
}

/// <summary>
/// メッシュの中のスキンウェイトを探していく
/// </summary>
/// <param name="_FileName"></param>
/// <param name="_pFP"></param>
/// <param name="pMesh"></param>
void cXFileLoad::GetMeshBone(std::ifstream * _pFP, XFileLoadMeshBuf * _output)
{
	string buf;

	//まずはスキンメッシュの基本的な情報を取得していく
	getline(*_pFP, buf, ';');		//ここには頂点あたりの最大影響ボーン数が入る(基本的に４つ)
	int MaxVertexBone = stoi(buf);
	getline(*_pFP, buf);	//改行

	getline(*_pFP, buf, ';');		//ここには一面あたりの最大影響ボーン数が入る(基本的に6つ)
	int MaxFaceBone = stoi(buf);
	getline(*_pFP, buf);	//改行

	getline(*_pFP, buf, ';');		//ここに読み取るボーン数
	int MaxBone = stoi(buf);
	getline(*_pFP, buf);	//改行


	//ここから下はSkinWeightsを読み取っていく

	//ボーン数だけ読み込む
	for (int i = 0; i < MaxBone; i++) {
		//まずはSkinWeightsを探す
		while (buf != "SkinWeights") {
			*_pFP >> buf;
		}
		//SkinWeightsを見つけて抜けた
		getline(*_pFP, buf, '"');	//改行

		//型名の下にはボーンの名前が格納されている
		getline(*_pFP, buf, '"');	//ボーン名を取得
		int BoneNo = 0;	//ボーンが格納された配列番号
		for (int j = 0; m_Bones[j].vName != buf; j++) {
			BoneNo++;		//jが違ったら次の添え字を探すのでjと同じくBoneNoを++する
		}
		getline(*_pFP, buf);	//改行

		//ここで影響のある頂点インデックス数がくる
		*_pFP >> buf;
		int FaceNum = stoi(buf);
		int* pIndex = new int[FaceNum];	//インデックスを格納する入れ物
		for (int j = 0; j < FaceNum; j++) {
			*_pFP >> buf;
			pIndex[j] = stoi(buf);	//何番目の頂点に重みを付けるか格納
		}

		//次に各頂点に重み情報を渡していく
		for (int j = 0; j < FaceNum; j++) {
			//先ほど集めたデータ(pIndex)の場所に格納された頂点番号を取り出す
			int ind = pIndex[j];

			//ウェイトを読み込む
			*_pFP >> buf;
			float weight = stof(buf);


			//まずは何番目のデータかを確認する
			int ID = ind;
			switch (_output->vFace.vVertex[ID].vBoneCnt) {
			case 0:
				//初めて格納する場合は読み込んだ時の初期化を行う
				_output->vFace.vVertex[ID].vBoneNo = { (uint32_t)BoneNo,0,0,0 };
				_output->vFace.vVertex[ID].vWeight.x = weight;
				break;
			case 1:
				_output->vFace.vVertex[ID].vBoneNo.y = (uint32_t)BoneNo;
				_output->vFace.vVertex[ID].vWeight.y = weight;
				break;
			case 2:
				_output->vFace.vVertex[ID].vBoneNo.z = (uint32_t)BoneNo;
				_output->vFace.vVertex[ID].vWeight.z = weight;
				break;
			case 3:
				_output->vFace.vVertex[ID].vBoneNo.w = (uint32_t)BoneNo;
				_output->vFace.vVertex[ID].vWeight.w = weight;
				break;
			}

		}

		//ここからが最後の行列を読み込む
		XMFLOAT4X4 m;
		for (int j = 0; j < 15; j++) {
			getline(*_pFP, buf, ',');		//1要素を読み込む
			m.m[j / 4][j % 4] = stof(buf);
		}
		getline(*_pFP, buf, ';');		//1要素を読み込む
		m.m[3][3] = stof(buf);
		m_Bones[BoneNo].vBind = m;	//バインドポーズを格納する
		getline(*_pFP, buf);		//行列行を改行
		delete[] pIndex;	//解放

		*_pFP >> buf;	//ここにはスキンウェイトの}が入る
	}
	*_pFP >> buf;
}

/// <summary>
/// 一番最初の"Frame"を見つけるだけの関数
/// </summary>
/// <param name="_pFP"></param>
void cXFileLoad::SearchFrame(std::ifstream * _pFP)
{
	string buf;
	*_pFP >> buf;

	//Frameという文字を見つけていなく、尚且つファイル終端でもなければ繰り返し
	while (buf != "Frame" && !(_pFP->eof())) {
		if (buf == "template") {
			getline(*_pFP, buf);		//templateは一行飛ばす
		}
		else {
			*_pFP >> buf;
		}
	}

	//抜けた = ファイル読み込みが"Frame"まで行っている
}

//Xファイルの情報からスタティックかスキンメッシュかを判定する
int cXFileLoad::CheckType(std::string _FileName)
{
	ifstream pFP(_FileName.c_str());
	string buf;
	int dwMeshType = 0;
	pFP >> buf;

	//ファイル終焉まで探索を行い、様々な情報を取得する
	while (!(pFP.eof())) {
		if (buf == "template") {
			getline(pFP, buf);		//templateは一行飛ばす
		}
		if (buf == "XSkinMeshHeader") {
			dwMeshType = 2;			//このモデルデータにはアニメーションが含まれる
		}
		if (buf == "AnimationSet") {
			//モーションデータを見つけた
			//"Animation"の数(ボーン数)をカウントしてその値を返す関数
			m_BoneInMotion[m_MotionNum] = CountAnimeBone(&pFP);
			m_MotionNum++;		//モーション数をカウント
			pFP >> buf;
		}
		else {
			pFP >> buf;
		}
	}

	//抜けた = ファイル読み込みが"Frame"まで行っている
	return dwMeshType;
}

/// <summary>
/// Xファイル上のボーン(フレーム)を読み込む関数。すべて読み込み終わるまで回帰処理される
/// </summary>
/// <param name="_pFP">初めのFrameまで読み込んだ状態のものが渡される</param>
/// <param name="_pBone">今から読み込むボーンの情報を格納するアドレス</param>
void cXFileLoad::GetFrame(std::ifstream * _pFP, Bone * _pBone, std::string _ParentName)
{
	string buf;		//読み込んだ文字列を格納するバッファ
	string Name;		//自分自身の名前
	XMFLOAT4X4 Mat;

	//初めにFrameの名前を読み込む必要がある。
	*_pFP >> buf;		//ここで読み込んだものが名前
	Name = buf;
	_pBone->vBoneName = buf;	//名前を保持しておく
	getline(*_pFP, buf);		//名前を読んだ後は改行する。

	//名前の次の行はローカル行列型名、その次の行に数値が格納されているので取り出す
	while (buf != "FrameTransformMatrix") {
		*_pFP >> buf;		//行列数値が格納された行の一つ前までどんどん呼んでいく
	}
	getline(*_pFP, buf);	//中カッコを取り除く


	//まずは文字を','まで読み込むが最後だけ;;で終わるので行列要素数 - 1(= 15)回分だけ読み込む
	for (int i = 0; i < 15; i++) {
		getline(*_pFP, buf, ',');	//中カッコを取り除く
		_pBone->vTransform.m[i / 4][i % 4] = (float)std::stod(buf);
	}
	//ラスト一回は';'を除外した状態で読み込み最後にgetlineで次の行に進める
	getline(*_pFP, buf, ';');
	_pBone->vTransform._44 = (float)std::stod(buf);
	getline(*_pFP, buf);		//ここは二つ目の';'を読み込む'{'のためにもう一度…
	getline(*_pFP, buf);		//これで行列格納終わり
	Mat = _pBone->vTransform;	//行列保存

	//ここまで読み込めたらボーン情報を保存しておく
	SetBoneData(_ParentName, Name, Mat);


	//============ここまでがFrameスコープの定型処理。ここからはFrame or Meshを次々にロードすることになる===============
	*_pFP >> buf;		//ループ開始前に一度読み込む
	while (buf != "}" && !(_pFP->eof()))
	{
		//まずは読み込んだものがFrame か Meshのどちらかを確認する
		if (buf == "Frame") {
			//Frameの時の処理
			_pBone->vNumChild++;		//子の数をカウントする
			Bone* pChildBoneAddr = new Bone;
			GetFrame(_pFP, pChildBoneAddr, Name);		//回帰処理！
			_pBone->vpChild.push_back(pChildBoneAddr);
		}
		else if (buf == "Mesh") {
			//Meshの場合の処理
			SkipScope(_pFP);			//現段階では{}間を飛ばす意味しかない。
		}
		*_pFP >> buf;		//次の文字を読む
	}
}

/// <summary>
/// ボーンの情報を一時的に保存していく関数
/// </summary>
/// <param name="_ParentName"></param>
/// <param name="_Name"></param>
/// <param name="_Mat"></param>
void cXFileLoad::SetBoneData(std::string _ParentName, std::string _Name, DirectX::XMFLOAT4X4 _Mat)
{
	int ParentNo = -1;

	//まずは親の添え字を探す
	for (int i = 0; i < m_Suffix; i++) {
		if (m_Bones[i].vName == _ParentName) {
			ParentNo = i;
		}
	}

	m_Bones[m_Suffix].vParentNo = ParentNo;
	m_Bones[m_Suffix].vName = _Name;
	m_Bones[m_Suffix].vMat = _Mat;

	m_Suffix++;		//ボーン数をカウントする
}

/// <summary>
/// 次の}まで飛ばす
/// </summary>
/// <param name="_pFP"></param>
void cXFileLoad::SkipScope(std::ifstream* _pFP)
{
	int cnt = 1;		//'{'がくれば+ '}'がくれば-して0になったら戻る
	string buf;
	getline(*_pFP, buf);

	while (cnt > 0 && !(_pFP->eof()))
	{
		*_pFP >> buf;
		if (buf == "{") cnt++;
		else if (buf == "}") cnt--;
	}
}

/// <summary>
/// モーションの中にあるボーンの数をカウントする
/// </summary>
/// <param name="_pFP"></param>
/// <returns></returns>
int cXFileLoad::CountAnimeBone(std::ifstream * _pFP)
{
	//まずは下の階層にある"Animetion"を見つける
	string buf;
	int BoneNum = 0;				//ボーンの数を格納する
	getline(*_pFP, buf);		//ここにはアニメーションの名前が入ってくる
	*_pFP >> buf;		//ここに"Animetion"が入る

	while (buf != "}") {
		getline(*_pFP, buf);		//改行
		BoneNum++;		//数をカウント

		SkipScope(_pFP);
		*_pFP >> buf;		//ここに"Animetion" or "}"が入る
	}
	return BoneNum;			//ボーン数を返却
}

/// <summary>
/// アニメーションデータを作成する
/// </summary>
/// <param name="_pFP"></param>
/// <param name="_output"></param>
void cXFileLoad::GetAnimation(std::ifstream * _pFP, cAnimationData *& _output)
{
	string buf;
	//まずは必要なデータ領域を確保する
	_output = new cAnimationData;
	_output->m_MotionNum = m_MotionNum;		//モーションの数を保存
	_output->m_pMotion = new MotionData[m_MotionNum];	//モーション数分のデータを確保

	//次に一つのモーションの中のボーン数分のデータ領域を確保する
	for (int i = 0; i < m_MotionNum; i++) {
		_output->m_pMotion[i].vBoneNum = m_BoneInMotion[i];		//そのモーション番号にあるボーン数を保持
		_output->m_pMotion[i].vBoneAnime = new BoneAnime[m_BoneInMotion[i]];	//ボーン数分のデータ領域を確保する
	}

	//ここからがデータの読み込み。アニメーション数分のデータを取得していく
	for (int i = 0; i < m_MotionNum; i++) {
		*_pFP >> buf;				//ここで"AnimetionSet"を取得。
		getline(*_pFP, buf);		//その行を改行させる

		//ボーン数分のデータを読み込んでいく
		for (int j = 0; j < m_BoneInMotion[i]; j++) {
			*_pFP >> buf;				//ここで"Animetion"を取得。
			getline(*_pFP, buf);		//その行を改行させる
			GetKeyFrame(_pFP, &_output->m_pMotion[i].vBoneAnime[j]);		//ボーンにキーフレームデータを格納する
		}
		*_pFP >> buf;
	}
}

/// <summary>
/// 渡されたボーン構造体にキーフレームデータ、ボーン番号を渡す関数。"Animetion"の中身をまるまる取り出す
/// </summary>
/// <param name="_pFP"></param>
/// <param name="_pBone"></param>
void cXFileLoad::GetKeyFrame(std::ifstream * _pFP, BoneAnime * _pBone)
{
	//ここに来た時点で次に読み込めるものはAnimationKeyになっている前提
	string buf = "Start!";

	while (buf != "{")		//この"{"はキーフレームの後に来るボーン名が入っているスコープの入口
	{
		*_pFP >> buf;
		if (buf == "AnimationKey") {
			getline(*_pFP, buf);		//その行を改行させる
			*_pFP >> buf;		//ここに来るのはキーフレームのタイプ(0 = 回転, 1 = 拡大, 2 = 移動, 3 = 行列)
			int type = stoi(buf);

			*_pFP >> buf;		//ここにキーフレーム数が入ってくる
			int KeyNum = stoi(buf);	//その時のキーフレーム数

			//アドレスがnullptrの時だけ新しくデータ領域を確保する
			if (_pBone->vpKeyFrame == nullptr) {
				_pBone->vKeyNum = stoi(buf);
				_pBone->vpKeyFrame = new KeyFrame[_pBone->vKeyNum];
			}
			getline(*_pFP, buf);		//改行	

			//キーフレームの数だけ読み込む
			for (int i = 0; i < KeyNum; i++) {
				getline(*_pFP, buf, ';');		//キーフレーム時間を格納
				int Time = stoi(buf);	//時間情報を取得
				//まだデータが格納されていなければ強制的に時間情報を取得する
				if (_pBone->vpKeyFrame[i].vTime == -1) {
					_pBone->vpKeyFrame[i].vTime = Time;
				}
				else {
					//すでに格納されている場合の処理(同じフレーム番号を見つけそこの添え字にする)
					int Suf = 0;	//探す添え字
					while (_pBone->vpKeyFrame[i].vTime != Time)
					{
						i++;		//そこの位置まで読み込みを飛ばす
					}
				}
				//まずは初めにいくつの要素をデータとして持っているかを確認する
				getline(*_pFP, buf, ';');
				int ElemNum = stoi(buf);		//読み込む要素数
				//キーフレームタイプによってデータを分ける。
				if (type == 3) {
					//元から行列データが入っている場合
					getline(*_pFP, buf, ';');		//ここに要素数が入るが行列なので16になる
					//最後の一回だけ終焉が;なのでまず15回読み込む
					for (int No = 0; No < 15; No++) {
						getline(*_pFP, buf, ',');
						_pBone->vpKeyFrame[i].vMat.m[No / 4][No % 4] = stof(buf);
					}
					getline(*_pFP, buf, ';');
					_pBone->vpKeyFrame[i].vMat.m[3][3] = stof(buf);		//最後の一回
					getline(*_pFP, buf);		//改行
				}
				else {
					//行列以外の要素の場合

					//必要数分だけデータを取り出すが基本的に4要素目が不要なので三回分
					XMFLOAT4 read = { 0.0f,0.0f,0.0f,0.0f };
					getline(*_pFP, buf, ',');			//まず一要素目
					read.x = stof(buf);
					getline(*_pFP, buf, ',');			//まず一要素目
					read.y = stof(buf);
					//3要素目だけ要素数に応じて処理を切り替える
					if (ElemNum == 3) {
						//3だったら終わるときに;で終了
						getline(*_pFP, buf, ';');
					}
					else {
						//4だったら,で終了
						getline(*_pFP, buf, ',');
					}
					read.z = stof(buf);
					if (ElemNum == 4) {
						getline(*_pFP, buf, ';');
						read.w = stof(buf);
					}
					getline(*_pFP, buf);		//改行
					//行列を作成する
					XMMATRIX m;
					if (type == 0) {
						//Xファイル上ではクォータニオン(W,X,Y,Z)の順で格納されている
						XMFLOAT4 rotData = { read.y,read.z,read.w,read.x };
						XMVECTOR rotVec = XMLoadFloat4(&rotData);
						XMMATRIX RotMat = XMMatrixRotationQuaternion(rotVec);
						RotMat = XMMatrixInverse(NULL, RotMat);
						//m = XMMatrixRotationRollPitchYaw(read.x, read.y, read.z) * XMLoadFloat4x4(&_pBone->vpKeyFrame[i].vMat);		//他で変える処理はここ
						m = RotMat * XMLoadFloat4x4(&_pBone->vpKeyFrame[i].vMat);		//他で変える処理はここ
					}
					else if (type == 1)
						m = XMMatrixScaling(read.x, read.y, read.z) * XMLoadFloat4x4(&_pBone->vpKeyFrame[i].vMat);
					else if (type == 2)
						m = XMLoadFloat4x4(&_pBone->vpKeyFrame[i].vMat) * XMMatrixTranslation(read.x, read.y, read.z);
					else {
						MessageBox(0, "アニメーションにて不正な値が読み込まれました", NULL, MB_OK);
					}
					XMStoreFloat4x4(&_pBone->vpKeyFrame[i].vMat, m);
				}		// end if
			}		// end for
			*_pFP >> buf;
		}		// end "AnimationKey"
	}		// end while

	//最後にボーン番号を取得して完了
	*_pFP >> buf;	//ボーンの名前が入る
	bool BoneNameFindFlag = false;
	for (int i = 0; i < m_Suffix; i++) {
		if (m_Bones[i].vName == buf) {
			_pBone->vBoneNo = i;		//ボーン番号を見つけた！
			BoneNameFindFlag = true;
		}
	}
	if (!BoneNameFindFlag) {
		_pBone->vBoneNo = -1;
	}
	getline(*_pFP, buf);		//ボーンの列を改行
	getline(*_pFP, buf);		//アニメーションの分の改行
}


HRESULT cXFileLoad::CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer) {
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