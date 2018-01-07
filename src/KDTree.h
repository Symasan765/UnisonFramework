/*=================================================
//								FILE : KDTree.h
//		ファイル説明 :
//		頂点情報からKDTreeを作成するクラスを定義する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"
#include "Mesh3D.h"

/// <summary>
/// KDTreeにて利用するノード構造体
/// </summary>
struct  KDNode
{
	//Treeをどのように分割しているか保持する
	enum State {
		SplitNone = -1,		//分割なし。ノードの終焉
		SplitX = 0,					//X軸分割
		SplitY = 1,					//Y軸分割
		SplitZ = 2					//Z軸分割
	};

	KDNode* vLeftNode;		//分割したノード
	KDNode* vRightNode;		//分割したノード
	State vNodeState;			//Tree状態
	DirectX::XMINT3* vIndexed;				//終焉ノードの場合、ここにインデックス番号を格納する
	DirectX::XMFLOAT3 vBoxMin;					//空間の座標
	DirectX::XMFLOAT3 vBoxMax;				//空間の座標
	int vElementNum;			//ノード内に格納されている要素数
	
	KDNode() {
		vLeftNode = nullptr;
		vRightNode = nullptr;
		vIndexed = nullptr;
		vElementNum = 0;
		vBoxMin = { 0,0,0 };
		vBoxMax = { 0,0,0 };
		vNodeState = SplitNone;
	}

	~KDNode() {
		delete vLeftNode;
		delete vRightNode;
		delete[] vIndexed;
	}

	/// <summary>
	/// ツリーを構成していく関数
	/// </summary>
	/// <param name="_BoxMin">分割空間の箱座標</param>
	/// <param name="_BoxMax">分割空間の箱座標</param>
	/// <param name="_PosData">頂点座標配列への先頭アドレス</param>
	/// <param name="_Indexed">インデックス配列への先頭アドレス</param>
	/// <param name="_IndexNum">インデックス(ポリゴン)の数</param>
	/// <param name="_DivNum">残り分割回数</param>
	void Build(DirectX::XMFLOAT3 _BoxMin, DirectX::XMFLOAT3 _BoxMax,const DirectX::XMFLOAT3* _PosData,const DirectX::XMINT3* _Indexed,const int _IndexNum,const int _DivNum,const int _MinFaceNum);
	const KDNode* SearchNode(const DirectX::XMFLOAT3& _Pos, const DirectX::XMFLOAT3 _BoxMin, const DirectX::XMFLOAT3 _BoxMax);
};


struct KDBoundingData {
	DirectX::XMFLOAT3* vVerPos;	//頂点座標
	DirectX::XMINT3* vIndexed;
	int vNums;
};

/// <summary>
/// KDTreeを管理するクラス
/// </summary>
class cKDTree {
public:
	cKDTree();
	~cKDTree();
	void Create(const Mesh3D& _mesh,const int _DivNum = 20, const int _MinFaceNum = 10);		//メッシュ情報から必要なデータを取り出したあとKDTReeを作成する
	const KDNode* GetPointSpace(const DirectX::XMFLOAT3& _Pos);
	const DirectX::XMFLOAT3* GetVertexs();
	const DirectX::XMINT3* GetIndexd() { return m_Indexd; };
private:
	KDNode m_RootNode;					//一番大きな空間ノード
	DirectX::XMFLOAT3 m_BoxMin;		//空間の座標Min
	DirectX::XMFLOAT3 m_BoxMax;		//空間の座標Max
	DirectX::XMINT3* m_Indexd;		//ポリゴン数分のインデックス情報
	DirectX::XMFLOAT3* m_VerPos;	//頂点座標
	int m_IndexNum;							//インデックス(ポリゴン)数
};