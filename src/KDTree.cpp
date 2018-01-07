/*=================================================
//								FILE : KDTree.cpp
//		�t�@�C������ :
//		���b�V���������ɂ���KDTree�\�����쐬����N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "KDTree.h"
#include "HitCheck.h"

using namespace DirectX;

void KDNode::Build(DirectX::XMFLOAT3 _BoxMin, DirectX::XMFLOAT3 _BoxMax, const DirectX::XMFLOAT3 * _PosData, const DirectX::XMINT3 * _Indexed, const int _IndexNum, const int _DivNum, const int _MinFaceNum)
{
	//�܂��͕�������K�v������̂��H�Ȃ��̂��H�Ȃ���Ζ؂̍\�����s���A��������Ȃ犄�鏈��������
	//�������������
	//1.	�������̃J�E���g���܂�0�ł͂Ȃ��B
	//2.	�i�[�����|���S�������w�萔�ȉ��ɒB���Ă��Ȃ�
	if (_DivNum > 0 && (_IndexNum > _MinFaceNum)) {
		//���������B�܂��͂ǂ̕����Ɋ��邩�𔻒肷��
		const float Len[3] = { _BoxMax.x - _BoxMin.x ,_BoxMax.y - _BoxMin.y ,_BoxMax.z - _BoxMin.z };
		vNodeState = State::SplitX;		//�Ƃ肠�����b���X���w��
		if (Len[0] < Len[1]) vNodeState = State::SplitY;		//Y�̕����傫��������Y
		if (Len[2] > Len[(int)vNodeState]) vNodeState = State::SplitZ;	//X��Y�ő傫�������傫��������Z
		
		//�����{�b�N�X�̋��E�����߂Ă���
		XMFLOAT3 DivMaxBox = _BoxMax;	//���E�ʒu
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

		//������ԂƂ��ꂼ��̃|���S���̓����蔻����s���A�����J�E���g���f�[�^���o�b�t�@�Ɋi�[���Ă���
		XMINT3* LeftNodeIndexed = nullptr;		//Min ~ Sprit�܂łɓ����Ă���|���S�����ꎞ�I�Ɋi�[
		XMINT3* RightNodeIndexed = nullptr;		//Sprit ~ Max�܂łɓ����Ă���|���S�����ꎞ�I�Ɋi�[
		int LeftNodeCnt = 0;
		int RightNodeCnt = 0;

		//�ꎞ�I�Ɏ��̋�Ԃ֓n�����߃C���f�b�N�X�̈���m�ۂ���
		LeftNodeIndexed = new XMINT3[_IndexNum];		//����������ƑS�|���S��������\��������̂ŕK�v�������������m�ۂ���B����������Ȃ��Ȃ邩���c
		//�{�b�N�X�Ƃ̓����蔻����s��
		for (int i = 0; i < _IndexNum; i++) {
			//���ɓ���|���S�������m�F
			//���̃|���S���ɂ��钸�_�ԍ����擾����
			int Ver1 = _Indexed[i].x;
			int Ver2 = _Indexed[i].y;
			int Ver3 = _Indexed[i].z;
			bool HitFlag = false;
			//�e���_�Ԃ̃x�N�g���Ƌ�Ԃ̓����蔻������	//TODO ����ȎO�p�|���S���Ə�����AABB�̏ꍇ������Ȃ����Ƃ�����B��肪�o��悤�Ȃ画���ς���
			if (HitCheck::AABBandLine(_BoxMin, DivMaxBox, _PosData[Ver1], _PosData[Ver2])) { HitFlag = true; }
			else if (HitCheck::AABBandLine(_BoxMin, DivMaxBox, _PosData[Ver2], _PosData[Ver3])) { HitFlag = true; }
			else if (HitCheck::AABBandLine(_BoxMin, DivMaxBox, _PosData[Ver3], _PosData[Ver1])) { HitFlag = true; }

			//�������Ă����I
			if (HitFlag) {
				LeftNodeIndexed[LeftNodeCnt] = _Indexed[i];	//�������Ă���|���S�������i�[����
				LeftNodeCnt++;
			}
			else {
				//�������ĂȂ�
				int a = 10;
			}
		}
		//�ċA�������s��
		vLeftNode = new KDNode;
		vLeftNode->Build(_BoxMin, DivMaxBox, _PosData, LeftNodeIndexed, LeftNodeCnt, _DivNum - 1, _MinFaceNum - 1);
		delete[] LeftNodeIndexed;


		RightNodeIndexed = new XMINT3[_IndexNum];
		//�{�b�N�X�Ƃ̓����蔻����s��
		for (int i = 0; i < _IndexNum; i++) {
			//���ɓ���|���S�������m�F
			//���̃|���S���ɂ��钸�_�ԍ����擾����
			int Ver1 = _Indexed[i].x;
			int Ver2 = _Indexed[i].y;
			int Ver3 = _Indexed[i].z;
			bool HitFlag = false;
			//�e���_�Ԃ̃x�N�g���Ƌ�Ԃ̓����蔻������	//TODO ����ȎO�p�|���S���Ə�����AABB�̏ꍇ������Ȃ����Ƃ�����B��肪�o��悤�Ȃ画���ς���
			if (HitCheck::AABBandLine(DivMinBox, _BoxMax, _PosData[Ver1], _PosData[Ver2])) { HitFlag = true; }
			else if (HitCheck::AABBandLine(DivMinBox, _BoxMax, _PosData[Ver2], _PosData[Ver3])) { HitFlag = true; }
			else if (HitCheck::AABBandLine(DivMinBox, _BoxMax, _PosData[Ver3], _PosData[Ver1])) { HitFlag = true; }

			//�������Ă����I
			if (HitFlag) {
				RightNodeIndexed[RightNodeCnt] = _Indexed[i];	//�������Ă���|���S�������i�[����
				RightNodeCnt++;
			}
		}
		//�ċA�������s��
		vRightNode = new KDNode;
		vRightNode->Build(DivMinBox, _BoxMax, _PosData, RightNodeIndexed, RightNodeCnt, _DivNum - 1, _MinFaceNum - 1);
		delete[] RightNodeIndexed;
	}
	else {
		//�؏��\�z����
		vBoxMin = _BoxMin;
		vBoxMax = _BoxMax;
		//�܂����̋�ԂɃ|���S�����܂܂�Ă��邩���m�F����
		if (_IndexNum > 0) {
			vElementNum = _IndexNum;	//�|���S������ێ�
			vIndexed = new XMINT3[vElementNum];
			for (int i = 0; i < vElementNum; i++) {
				vIndexed[i] = _Indexed[i];
			}
		}
	}
}

/// <summary>
/// ��A���������Ȃ���w���Ԃ�T���o��
/// </summary>
/// <param name="_Pos"></param>
/// <returns></returns>
const KDNode * KDNode::SearchNode(const DirectX::XMFLOAT3& _Pos, const DirectX::XMFLOAT3 _BoxMin, const DirectX::XMFLOAT3 _BoxMax)
{
	KDNode* RetNode = nullptr;
	//���̃m�[�h���I�[�ł���΂��̃m�[�h�̃A�h���X��Ԃ�
	if (vNodeState == State::SplitNone) {
		return this;
	}
	else {
		//�܂����̊K�w�����݂��Ă���
		XMFLOAT3 NodeSize = { _BoxMin.x + _BoxMax.x,_BoxMin.y + _BoxMax.y,_BoxMin.z + _BoxMax.z };
		XMFLOAT3 NodeMin = _BoxMin;
		XMFLOAT3 NodeMax = _BoxMax;

		switch (vNodeState)
		{
		case State::SplitX: {
			const float Size = NodeSize.x * 0.5f;		//X���̕����_�����߂�
			if (_Pos.x >= Size) {
				//���߂���W�������_���傫��
				NodeMin.x = Size;
				return vRightNode->SearchNode(_Pos, NodeMin, NodeMax);		//��A����
			}
			else {
				//���߂���W�������_��菬����
				NodeMax.x = Size;
				return vLeftNode->SearchNode(_Pos, NodeMin, NodeMax);		//��A����
			}
			break;
		}
		case State::SplitY: {
			const float Size = NodeSize.y * 0.5f;		//X���̕����_�����߂�
			if (_Pos.y >= Size) {
				//���߂���W�������_���傫��
				NodeMin.y = Size;
				return vRightNode->SearchNode(_Pos, NodeMin, NodeMax);		//��A����
			}
			else {
				//���߂���W�������_��菬����
				NodeMax.y = Size;
				return vLeftNode->SearchNode(_Pos, NodeMin, NodeMax);		//��A����
			}
			break;
		}
		case State::SplitZ: {
			const float Size = NodeSize.z * 0.5f;		//X���̕����_�����߂�
			if (_Pos.z >= Size) {
				//���߂���W�������_���傫��
				NodeMin.z = Size;
				return vRightNode->SearchNode(_Pos, NodeMin, NodeMax);		//��A����
			}
			else {
				//���߂���W�������_��菬����
				NodeMax.z = Size;
				return vLeftNode->SearchNode(_Pos, NodeMin, NodeMax);		//��A����
			}
			break;
		}
		}// end switch
	}

	return nullptr;		//�{�������ɂ͂��ǂ���Ȃ��͂�
}

cKDTree::cKDTree()
{
	m_BoxMin = {0,0,0};			//��Ԃ̍��WMin
	m_BoxMax = {0,0,0};		//��Ԃ̍��WMax
	m_Indexd = nullptr;		//�|���S�������̃C���f�b�N�X���
	m_VerPos = nullptr;		//���_���W
	m_IndexNum = 0;				//�C���f�b�N�X(�|���S��)��
}

cKDTree::~cKDTree()
{
	delete[] m_Indexd;
	delete[] m_VerPos;
}

void cKDTree::Create(const Mesh3D & _mesh, const int _DivNum, const int _MinFaceNum)
{
	//�K�v�ȏ���
	//�܂����b�V����񂩂璸�_���W�𔲂��o���B���̎��ɂ��Ăŋ�ԃT�C�Y���擾���Ă���
	//���Ƀ}�e���A�����ɕ�����Ă��钸�_�C���f�b�N�X����ɂ܂Ƃ߂邽��
	//�}�e���A�����̃C���f�b�N�X���̍��v���̗̈���m�ۂ��Ă����ɃC���f�b�N�X���i�[���Ă���
	//���ׂĂ̏�񂪔����o������KDTree�m�[�h�ɏ���n���Ė؂����������Ă���

	
	//�܂��}�e���A�����ꂼ��̒��_���𑫂��č��v���擾����
	m_IndexNum = 0;	//�|���S����
	int VertexNum = -1;		//���_��
	for (int i = 0; i < _mesh.vMaterialCnt; i++) {
		m_IndexNum += _mesh.vpMaterial[i].dwNumFace;		//�|���S���������Z
		for (int j = 0; j < _mesh.vpMaterial[i].dwNumFace * 3;j++) {
			//���傫�Ȓ��_��������
			if (VertexNum < _mesh.ppiIndex[i][j]) {
				VertexNum = _mesh.ppiIndex[i][j];
			}
		}
	}

	//���ɃC���f�b�N�X�����̃f�[�^���C���f�b�N�X��񂩂���グ��
	m_Indexd = new XMINT3[m_IndexNum];
	int s = 0;	//�|���S���Y����
	for (int i = 0; i < _mesh.vMaterialCnt; i++) {
		for (int j = 0; j < _mesh.vpMaterial[i].dwNumFace * 3; j += 3) {
			//1�|���S�����̃C���f�b�N�X���擾����
			m_Indexd[s].x = VertexNum = _mesh.ppiIndex[i][j + 0];
			m_Indexd[s].y = VertexNum = _mesh.ppiIndex[i][j + 1];
			m_Indexd[s].z = VertexNum = _mesh.ppiIndex[i][j + 2];
			s++;
		}
	}

	//���_���W�𔲂��o���Ă����B���̎��ɂ��łɃ{�b�N�X���쐬���Ă���
	m_VerPos = new XMFLOAT3[VertexNum];		//���_�����̃f�[�^���i�[����
	for (int i = 0; i < VertexNum; i++) {
		m_VerPos[i].x = _mesh.vpVertexData[i].vPos.x;
		m_VerPos[i].y = _mesh.vpVertexData[i].vPos.y;
		m_VerPos[i].z = _mesh.vpVertexData[i].vPos.z;
		//BOX�쐬�̍���
		if (m_VerPos[i].x < m_BoxMin.x) m_BoxMin.x = m_VerPos[i].x;
		if (m_VerPos[i].x > m_BoxMax.x) m_BoxMax.x = m_VerPos[i].x;

		if (m_VerPos[i].y < m_BoxMin.y) m_BoxMin.y = m_VerPos[i].y;
		if (m_VerPos[i].y > m_BoxMax.y) m_BoxMax.y = m_VerPos[i].y;

		if (m_VerPos[i].z < m_BoxMin.z) m_BoxMin.z = m_VerPos[i].z;
		if (m_VerPos[i].z > m_BoxMax.z) m_BoxMax.z = m_VerPos[i].z;
	}
	

	//��������͎��ۂɖ؍\���̍\�z�Ɉڂ�
	m_RootNode.Build(m_BoxMin, m_BoxMax, m_VerPos, m_Indexd, m_IndexNum, _DivNum, _MinFaceNum);
}

/// <summary>
/// �w����W�����݂����Ԃ��擾����
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
