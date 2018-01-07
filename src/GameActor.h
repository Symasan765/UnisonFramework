/*=================================================
//								FILE : GameActor.h
//		ファイル説明 :
//		ゲーム内で使用する状態遷移で使用する基底クラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once

class cFighter;
class NormalEnemy;

//プレイヤーの通常状態の管理を行う
enum PlayerNormalState {
	CHANGE_NONE = 0,		//変化なし
	NORMAL,						//通常状態
	ROLLING,					//ローリング中
	BURSTWAIT,				//バースト攻撃待機中
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
/// 敵の状態の基底クラス
/// </summary>
class cEnemyActor {
public:
	enum EnemyState {
		CHANGE_NONE = 0,		//変化なし
		NORMAL,						//通常状態
		FALL,							//弾が当たって落下中
		BULLETWAIT,				//弾発射待機状態
	};
	cEnemyActor() = default;
	~cEnemyActor() = default;
	virtual void Entry(NormalEnemy * pFighter) = 0;
	virtual void Exit(NormalEnemy * pFighter) = 0;
	virtual EnemyState Update(NormalEnemy* pFighter) = 0;
};