/*=================================================
//								FILE : Animation.cpp
//		ファイル説明 :
//		メッシュデータのアニメーションを扱うクラス、構造体を定義する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "Animation.h"
#include "Mesh3D.h"

using namespace DirectX;

/// <summary>
/// メンバ変数の初期化を行う
/// </summary>
cAnimationData::cAnimationData()
{
	m_AnimeCnt = 0;
	m_MotionNum = 0;
	m_NowMotionNo = -1;
	m_pMotion = nullptr;
	m_pBone = nullptr;
	m_pBoneNum = 0;
}

/// <summary>
/// 解放処理
/// </summary>
cAnimationData::~cAnimationData()
{
	delete[] m_pMotion;
	delete[] m_pBone;
}

/// <summary>
/// 指定したアニメーション番号を元にボーン情報を更新する
/// </summary>
/// <param name="_AnimeNo">動かすアニメ番号</param>
/// <returns>アニメーションさせられたかどうか？</returns>
bool cAnimationData::ToAnimetion(int _AnimeNo)
{
	//まずアニメーション番号が存在するかチェックする
	if (m_MotionNum >= _AnimeNo) {
		//前回からアニメーションが変わったかをチェックする
		if (m_NowMotionNo != _AnimeNo) {
			m_NowMotionNo = _AnimeNo;
			m_AnimeCnt = 0;		//新しいモーションに入ったのでカウントを0に
		}
		// TODO アニメーションの更新処理関数をここに記述する
		m_AnimeCnt++;			//カウント
		return true;
	}
	return false;		//アニメーションできなかった
}

/// <summary>
/// アニメーション情報を取得してシェーダに渡す
/// </summary>
/// <param name="_AnimeNo"></param>
/// <param name="_Time"></param>
void cAnimationData::SetBoneConst(int _AnimeNo, int _Time)
{
	//必要な処理はそれぞれの行列情報とアニメーションの最終補間姿勢
	//再生するアニメーションの中の動かすボーン分だけ処理する

	if (m_pBoneNum > 0) {
		int Suf = 0;		//どこの添え字番号のところが最終座標か
		int MaxKey = m_pMotion[_AnimeNo].vBoneAnime[0].vKeyNum;
		//まずは時間情報を範囲内に収める
		_Time %= m_pMotion[_AnimeNo].vBoneAnime[0].vpKeyFrame[MaxKey - 1].vTime;
		bool KeyTimeFindFlag = false;		//キーフレームを見つけたかのフラグ
		for (int i = 0; i < MaxKey; i++) {
			//今からモーションするフレームの中からタイムが近いものを探し出す
			if (m_pMotion[_AnimeNo].vBoneAnime[0].vpKeyFrame[i].vTime > _Time) {
				Suf = i;		//暫定でその添え字を動かすことになる
				if (!KeyTimeFindFlag)
					break;
			}
		}

		//補間の割合を算出する
		const int TargetTime = m_pMotion[_AnimeNo].vBoneAnime[0].vpKeyFrame[Suf].vTime;			//補間するタイムを取得する
		float t = 0;//補間する際の割合
					//ゼロ徐算回避
		if (TargetTime > 0) {
			t = (float)_Time / (float)TargetTime;
		}
		else {
			t = 1.0f;		//まぁ目標が0なら可能性があるのはタイム0でしょうね
		}
		//t = t * t * (3 - 2 * t);
		if (t > 1.0f) t = 1.0f;	//範囲固定

		// i == これから動かすボーンの番号となる
		for (int i = 0; i < m_pMotion[_AnimeNo].vBoneNum; i++) {
			//まずは動かす最終姿勢を取り出す
			// TODO アニメーションの補間を行えるようにすること
			int BoneNo = m_pMotion[_AnimeNo].vBoneAnime[i].vBoneNo;		//動かすボーン番号
			//ボーン番号が-1は存在していないボーン
			if (BoneNo != -1) {
				
				int Former = Suf - 1;		//一つ前のフレームを補間する
				if (Former < 0) Suf = 0;	//マイナスの値ならゼロになってろ
				DirectX::XMFLOAT4X4 Before = m_pMotion[_AnimeNo].vBoneAnime[i].vpKeyFrame[Former].vMat;	//変換元のデータ
				DirectX::XMFLOAT4X4 After = m_pMotion[_AnimeNo].vBoneAnime[i].vpKeyFrame[Suf].vMat;		//これが補間する最終データ

				//=====================補間の処理を行っていく=====================
				//移動の補間処理 TODO 一度線形補間のコードを直書きするが関数化しておくこと
				XMVECTOR BeforePos = XMLoadFloat3(&XMFLOAT3{ Before._41, Before._42 ,Before._43 });
				XMVECTOR AfterPos = XMLoadFloat3(&XMFLOAT3{ After._41, After._42 ,After._43 });
				XMFLOAT3 LerpPosBuf{ 0,0,0 };
				XMVECTOR LerpPos = AfterPos * t + BeforePos * (1.0f - t);		//補間後の座標
				XMStoreFloat3(&LerpPosBuf, LerpPos);

				//回転の補間を行う
				Before._41 = Before._42 = Before._43 = Before._44 = 0.0f;
				After._41 = After._42 = After._43 = After._44 = 0.0f;
				XMVECTOR BeforeRot = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&Before));
				XMVECTOR AfterRot = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&After));
				XMVECTOR LerpRot = XMQuaternionSlerp(BeforeRot, AfterRot, t);		//補間後のクォータニオンを格納

				//作成した補間情報をまとめる
				XMMATRIX LerpMat = DirectX::XMMatrixRotationQuaternion(LerpRot) * XMMatrixTranslation(LerpPosBuf.x, LerpPosBuf.y, LerpPosBuf.z);

				//=================作ったデータをシェーダへセットする=================
				DirectX::XMStoreFloat4x4(&m_pBone[BoneNo].vTransPose, LerpMat);
			}
		}

		MeshManager::GetInstance().SetBoneConstant(m_pBone, m_pBoneNum);
	}
}
