/*=================================================
//								FILE : GameActor.h
//		�t�@�C������ :
//		�Q�[�����Ŏg�p�����ԑJ�ڂŎg�p������N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once

class cFighter;
class NormalEnemy;

//�v���C���[�̒ʏ��Ԃ̊Ǘ����s��
enum PlayerNormalState {
	CHANGE_NONE = 0,		//�ω��Ȃ�
	NORMAL,						//�ʏ���
	ROLLING,					//���[�����O��
	BURSTWAIT,				//�o�[�X�g�U���ҋ@��
};

class cGameActor {
public:
	cGameActor() = default;
	~cGameActor() = default;
	virtual void Entry(cFighter * pFighter) = 0;
	virtual void Exit(cFighter * pFighter) = 0;
	virtual PlayerNormalState Update(cFighter* pFighter) = 0;
};


/// <summary>
/// �G�̏�Ԃ̊��N���X
/// </summary>
class cEnemyActor {
public:
	enum EnemyState {
		CHANGE_NONE = 0,		//�ω��Ȃ�
		NORMAL,						//�ʏ���
		FALL,							//�e���������ė�����
		BULLETWAIT,				//�e���ˑҋ@���
	};
	cEnemyActor() = default;
	~cEnemyActor() = default;
	virtual void Entry(NormalEnemy * pFighter) = 0;
	virtual void Exit(NormalEnemy * pFighter) = 0;
	virtual EnemyState Update(NormalEnemy* pFighter) = 0;
};