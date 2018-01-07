/*=================================================
//								FILE : KDTree.h
//		�t�@�C������ :
//		���_��񂩂�KDTree���쐬����N���X���`����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"
#include "Mesh3D.h"

/// <summary>
/// KDTree�ɂė��p����m�[�h�\����
/// </summary>
struct  KDNode
{
	//Tree���ǂ̂悤�ɕ������Ă��邩�ێ�����
	enum State {
		SplitNone = -1,		//�����Ȃ��B�m�[�h�̏I��
		SplitX = 0,					//X������
		SplitY = 1,					//Y������
		SplitZ = 2					//Z������
	};

	KDNode* vLeftNode;		//���������m�[�h
	KDNode* vRightNode;		//���������m�[�h
	State vNodeState;			//Tree���
	DirectX::XMINT3* vIndexed;				//�I���m�[�h�̏ꍇ�A�����ɃC���f�b�N�X�ԍ����i�[����
	DirectX::XMFLOAT3 vBoxMin;					//��Ԃ̍��W
	DirectX::XMFLOAT3 vBoxMax;				//��Ԃ̍��W
	int vElementNum;			//�m�[�h���Ɋi�[����Ă���v�f��
	
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
	/// �c���[���\�����Ă����֐�
	/// </summary>
	/// <param name="_BoxMin">������Ԃ̔����W</param>
	/// <param name="_BoxMax">������Ԃ̔����W</param>
	/// <param name="_PosData">���_���W�z��ւ̐擪�A�h���X</param>
	/// <param name="_Indexed">�C���f�b�N�X�z��ւ̐擪�A�h���X</param>
	/// <param name="_IndexNum">�C���f�b�N�X(�|���S��)�̐�</param>
	/// <param name="_DivNum">�c�蕪����</param>
	void Build(DirectX::XMFLOAT3 _BoxMin, DirectX::XMFLOAT3 _BoxMax,const DirectX::XMFLOAT3* _PosData,const DirectX::XMINT3* _Indexed,const int _IndexNum,const int _DivNum,const int _MinFaceNum);
	const KDNode* SearchNode(const DirectX::XMFLOAT3& _Pos, const DirectX::XMFLOAT3 _BoxMin, const DirectX::XMFLOAT3 _BoxMax);
};


struct KDBoundingData {
	DirectX::XMFLOAT3* vVerPos;	//���_���W
	DirectX::XMINT3* vIndexed;
	int vNums;
};

/// <summary>
/// KDTree���Ǘ�����N���X
/// </summary>
class cKDTree {
public:
	cKDTree();
	~cKDTree();
	void Create(const Mesh3D& _mesh,const int _DivNum = 20, const int _MinFaceNum = 10);		//���b�V����񂩂�K�v�ȃf�[�^�����o��������KDTRee���쐬����
	const KDNode* GetPointSpace(const DirectX::XMFLOAT3& _Pos);
	const DirectX::XMFLOAT3* GetVertexs();
	const DirectX::XMINT3* GetIndexd() { return m_Indexd; };
private:
	KDNode m_RootNode;					//��ԑ傫�ȋ�ԃm�[�h
	DirectX::XMFLOAT3 m_BoxMin;		//��Ԃ̍��WMin
	DirectX::XMFLOAT3 m_BoxMax;		//��Ԃ̍��WMax
	DirectX::XMINT3* m_Indexd;		//�|���S�������̃C���f�b�N�X���
	DirectX::XMFLOAT3* m_VerPos;	//���_���W
	int m_IndexNum;							//�C���f�b�N�X(�|���S��)��
};