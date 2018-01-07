/*=================================================
//								FILE : KDTree.cpp
//		ファイル説明 :
//		メッシュ情報を元にしてKDTree構造を作成するクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "KDTree.h"
#include "HitCheck.h"

using namespace DirectX;

void KDNode::Build(DirectX::XMFLOAT3 _BoxMin, DirectX::XMFLOAT3 _BoxMax, const DirectX::XMFLOAT3 * _PosData, const DirectX::XMINT3 * _Indexed, const int _IndexNum, const int _DivNum, const int _MinFaceNum)
{
	//まずは分割する必要があるのか？ないのか？なければ木の構成を行い、分割するなら割る処理をする
	//分割する条件は
	//1.	分割数のカウントがまだ0ではない。
	//2.	格納されるポリゴン数が指定数以下に達していない
	if (_DivNum > 0 && (_IndexNum > _MinFaceNum)) {
		//分割処理。まずはどの方向に割るかを判定する
		const float Len[3] = { _BoxMax.x - _BoxMin.x ,_BoxMax.y - _BoxMin.y ,_BoxMax.z - _BoxMin.z };
		vNodeState = State::SplitX;		//とりあえず暫定でX軸指定
		if (Len[0] < Len[1]) vNodeState = State::SplitY;		//Yの方が大きかったらY
		if (Len[2] > Len[(int)vNodeState]) vNodeState = State::SplitZ;	//XとYで大きい方より大きかったらZ
		
		//分割ボックスの境界を求めておく
		XMFLOAT3 DivMaxBox = _BoxMax;	//境界位置
		XMFLOAT3 DivMinBox = _BoxMin;
		switch (vNodeState)
		{
		case State::SplitX:
			DivMaxBox.x = (_BoxMax.x + _BoxMin.x) * 0.5f;
			DivMinBox.x = (_BoxMax.x + _BoxMin.x) * 0.5f;
			break;
		case State::SplitY:
			DivMaxBox.y = (_BoxMax.y + _BoxMin.y) * 0.5f;
			DivMinBox.y = (_BoxMax.y + _BoxMin.y) * 0.5f;
			break;
		case State::SplitZ:
			DivMaxBox.z = (_BoxMax.z + _BoxMin.z) * 0.5f;
			DivMinBox.z = (_BoxMax.z + _BoxMin.z) * 0.5f;
			break;
		}// end switch

		//分割空間とそれぞれのポリゴンの当たり判定を行い、数をカウントしつつデータをバッファに格納していく
		XMINT3* LeftNodeIndexed = nullptr;		//Min ~ Spritまでに入っているポリゴンを一時的に格納
		XMINT3* RightNodeIndexed = nullptr;		//Sprit ~ Maxまでに入っているポリゴンを一時的に格納
		int LeftNodeCnt = 0;
		int RightNodeCnt = 0;

		//一時的に次の空間へ渡すためインデックス領域を確保する
		LeftNodeIndexed = new XMINT3[_IndexNum];		//もしかすると全ポリゴンが入る可能性があるので必要数分メモリを確保する。メモリ足りなくなるかも…
		//ボックスとの当たり判定を行う
		for (int i = 0; i < _IndexNum; i++) {
			//中に入るポリゴン数を確認
			//そのポリゴンにある頂点番号を取得する
			int Ver1 = _Indexed[i].x;
			int Ver2 = _Indexed[i].y;
			int Ver3 = _Indexed[i].z;
			bool HitFlag = false;
			//各頂点間のベクトルと空間の当たり判定を取る	//TODO 巨大な三角ポリゴンと小さなAABBの場合当たらないことがある。問題が出るようなら判定を変える
			if (HitCheck::AABBandLine(_BoxMin, DivMaxBox, _PosData[Ver1], _PosData[Ver2])) { HitFlag = true; }
			else if (HitCheck::AABBandLine(_BoxMin, DivMaxBox, _PosData[Ver2], _PosData[Ver3])) { HitFlag = true; }
			else if (HitCheck::AABBandLine(_BoxMin, DivMaxBox, _PosData[Ver3], _PosData[Ver1])) { HitFlag = true; }

			//当たっていた！
			if (HitFlag) {
				LeftNodeIndexed[LeftNodeCnt] = _Indexed[i];	//当たっているポリゴン情報を格納する
				LeftNodeCnt++;
			}
			else {
				//当たってない
				int a = 10;
			}
		}
		//再帰処理を行う
		vLeftNode = new KDNode;
		vLeftNode->Build(_BoxMin, DivMaxBox, _PosData, LeftNodeIndexed, LeftNodeCnt, _DivNum - 1, _MinFaceNum - 1);
		delete[] LeftNodeIndexed;


		RightNodeIndexed = new XMINT3[_IndexNum];
		//ボックスとの当たり判定を行う
		for (int i = 0; i < _IndexNum; i++) {
			//中に入るポリゴン数を確認
			//そのポリゴンにある頂点番号を取得する
			int Ver1 = _Indexed[i].x;
			int Ver2 = _Indexed[i].y;
			int Ver3 = _Indexed[i].z;
			bool HitFlag = false;
			//各頂点間のベクトルと空間の当たり判定を取る	//TODO 巨大な三角ポリゴンと小さなAABBの場合当たらないことがある。問題が出るようなら判定を変える
			if (HitCheck::AABBandLine(DivMinBox, _BoxMax, _PosData[Ver1], _PosData[Ver2])) { HitFlag = true; }
			else if (HitCheck::AABBandLine(DivMinBox, _BoxMax, _PosData[Ver2], _PosData[Ver3])) { HitFlag = true; }
			else if (HitCheck::AABBandLine(DivMinBox, _BoxMax, _PosData[Ver3], _PosData[Ver1])) { HitFlag = true; }

			//当たっていた！
			if (HitFlag) {
				RightNodeIndexed[RightNodeCnt] = _Indexed[i];	//当たっているポリゴン情報を格納する
				RightNodeCnt++;
			}
		}
		//再帰処理を行う
		vRightNode = new KDNode;
		vRightNode->Build(DivMinBox, _BoxMax, _PosData, RightNodeIndexed, RightNodeCnt, _DivNum - 1, _MinFaceNum - 1);
		delete[] RightNodeIndexed;
	}
	else {
		//木情報構築処理
		vBoxMin = _BoxMin;
		vBoxMax = _BoxMax;
		//まずこの空間にポリゴンが含まれているかを確認する
		if (_IndexNum > 0) {
			vElementNum = _IndexNum;	//ポリゴン数を保持
			vIndexed = new XMINT3[vElementNum];
			for (int i = 0; i < vElementNum; i++) {
				vIndexed[i] = _Indexed[i];
			}
		}
	}
}

/// <summary>
/// 回帰処理をしながら指定空間を探し出す
/// </summary>
/// <param name="_Pos"></param>
/// <returns></returns>
const KDNode * KDNode::SearchNode(const DirectX::XMFLOAT3& _Pos, const DirectX::XMFLOAT3 _BoxMin, const DirectX::XMFLOAT3 _BoxMax)
{
	KDNode* RetNode = nullptr;
	//このノードが終端であればこのノードのアドレスを返す
	if (vNodeState == State::SplitNone) {
		return this;
	}
	else {
		//まだ下の階層が存在している
		XMFLOAT3 NodeSize = { _BoxMin.x + _BoxMax.x,_BoxMin.y + _BoxMax.y,_BoxMin.z + _BoxMax.z };
		XMFLOAT3 NodeMin = _BoxMin;
		XMFLOAT3 NodeMax = _BoxMax;

		switch (vNodeState)
		{
		case State::SplitX: {
			const float Size = NodeSize.x * 0.5f;		//X軸の分割点を求める
			if (_Pos.x >= Size) {
				//求める座標が分割点より大きい
				NodeMin.x = Size;
				return vRightNode->SearchNode(_Pos, NodeMin, NodeMax);		//回帰処理
			}
			else {
				//求める座標が分割点より小さい
				NodeMax.x = Size;
				return vLeftNode->SearchNode(_Pos, NodeMin, NodeMax);		//回帰処理
			}
			break;
		}
		case State::SplitY: {
			const float Size = NodeSize.y * 0.5f;		//X軸の分割点を求める
			if (_Pos.y >= Size) {
				//求める座標が分割点より大きい
				NodeMin.y = Size;
				return vRightNode->SearchNode(_Pos, NodeMin, NodeMax);		//回帰処理
			}
			else {
				//求める座標が分割点より小さい
				NodeMax.y = Size;
				return vLeftNode->SearchNode(_Pos, NodeMin, NodeMax);		//回帰処理
			}
			break;
		}
		case State::SplitZ: {
			const float Size = NodeSize.z * 0.5f;		//X軸の分割点を求める
			if (_Pos.z >= Size) {
				//求める座標が分割点より大きい
				NodeMin.z = Size;
				return vRightNode->SearchNode(_Pos, NodeMin, NodeMax);		//回帰処理
			}
			else {
				//求める座標が分割点より小さい
				NodeMax.z = Size;
				return vLeftNode->SearchNode(_Pos, NodeMin, NodeMax);		//回帰処理
			}
			break;
		}
		}// end switch
	}

	return nullptr;		//本来ここにはたどりつかないはず
}

cKDTree::cKDTree()
{
	m_BoxMin = {0,0,0};			//空間の座標Min
	m_BoxMax = {0,0,0};		//空間の座標Max
	m_Indexd = nullptr;		//ポリゴン数分のインデックス情報
	m_VerPos = nullptr;		//頂点座標
	m_IndexNum = 0;				//インデックス(ポリゴン)数
}

cKDTree::~cKDTree()
{
	delete[] m_Indexd;
	delete[] m_VerPos;
}

void cKDTree::Create(const Mesh3D & _mesh, const int _DivNum, const int _MinFaceNum)
{
	//必要な処理
	//まずメッシュ情報から頂点座標を抜き出す。この時についてで空間サイズも取得しておく
	//次にマテリアル毎に分かれている頂点インデックスを一つにまとめるため
	//マテリアル毎のインデックス数の合計分の領域を確保してそこにインデックスを格納していく
	//すべての情報が抜き出せたらKDTreeノードに情報を渡して木を完成させていく

	
	//まずマテリアルそれぞれの頂点数を足して合計を取得する
	m_IndexNum = 0;	//ポリゴン数
	int VertexNum = -1;		//頂点数
	for (int i = 0; i < _mesh.vMaterialCnt; i++) {
		m_IndexNum += _mesh.vpMaterial[i].dwNumFace;		//ポリゴン数を加算
		for (int j = 0; j < _mesh.vpMaterial[i].dwNumFace * 3;j++) {
			//より大きな頂点を見つけた
			if (VertexNum < _mesh.ppiIndex[i][j]) {
				VertexNum = _mesh.ppiIndex[i][j];
			}
		}
	}

	//次にインデックス数分のデータをインデックス情報から作り上げる
	m_Indexd = new XMINT3[m_IndexNum];
	int s = 0;	//ポリゴン添え字
	for (int i = 0; i < _mesh.vMaterialCnt; i++) {
		for (int j = 0; j < _mesh.vpMaterial[i].dwNumFace * 3; j += 3) {
			//1ポリゴン分のインデックスを取得する
			m_Indexd[s].x = VertexNum = _mesh.ppiIndex[i][j + 0];
			m_Indexd[s].y = VertexNum = _mesh.ppiIndex[i][j + 1];
			m_Indexd[s].z = VertexNum = _mesh.ppiIndex[i][j + 2];
			s++;
		}
	}

	//頂点座標を抜き出していく。その時についでにボックスも作成しておく
	m_VerPos = new XMFLOAT3[VertexNum];		//頂点数分のデータを格納する
	for (int i = 0; i < VertexNum; i++) {
		m_VerPos[i].x = _mesh.vpVertexData[i].vPos.x;
		m_VerPos[i].y = _mesh.vpVertexData[i].vPos.y;
		m_VerPos[i].z = _mesh.vpVertexData[i].vPos.z;
		//BOX作成の項目
		if (m_VerPos[i].x < m_BoxMin.x) m_BoxMin.x = m_VerPos[i].x;
		if (m_VerPos[i].x > m_BoxMax.x) m_BoxMax.x = m_VerPos[i].x;

		if (m_VerPos[i].y < m_BoxMin.y) m_BoxMin.y = m_VerPos[i].y;
		if (m_VerPos[i].y > m_BoxMax.y) m_BoxMax.y = m_VerPos[i].y;

		if (m_VerPos[i].z < m_BoxMin.z) m_BoxMin.z = m_VerPos[i].z;
		if (m_VerPos[i].z > m_BoxMax.z) m_BoxMax.z = m_VerPos[i].z;
	}
	

	//ここからは実際に木構造の構築に移る
	m_RootNode.Build(m_BoxMin, m_BoxMax, m_VerPos, m_Indexd, m_IndexNum, _DivNum, _MinFaceNum);
}

/// <summary>
/// 指定座標が存在する空間を取得する
/// </summary>
/// <param name="_Pos"></param>
/// <returns></returns>
const KDNode * cKDTree::GetPointSpace(const DirectX::XMFLOAT3 & _Pos)
{
	return m_RootNode.SearchNode(_Pos, m_BoxMin, m_BoxMax);
}

const DirectX::XMFLOAT3 * cKDTree::GetVertexs()
{
	return m_VerPos;
}
