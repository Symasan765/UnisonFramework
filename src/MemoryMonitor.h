/*=================================================
//								FILE : MemoryMonitor.h
//		ファイル説明 :
//		メモリー使用状況を取得し管理するクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"

struct MemoryData {
	DWORDLONG vUsed;		//使用中メモリ
	DWORDLONG vFree;		//空き容量
	DWORDLONG vSize;		//物理メモリ量
	DWORDLONG vInit;		//プログラム開始時のメモリ量
};

/// <summary>
/// メモリの管理クラス
/// </summary>
class cMemoryMonitor {
public:
	//更新処理や解放処理を行うためにcDirectXクラスをフレンド化しておく
	friend class cDirectX11;
	cMemoryMonitor(const cMemoryMonitor&) = delete;			//コピーコンストラクタを delete 指定
	cMemoryMonitor& operator=(const cMemoryMonitor&) = delete;	//コピー代入演算子も delete 指定
	cMemoryMonitor(cMemoryMonitor&&) = delete;			//ムーブコンストラクタを delete 指定
	cMemoryMonitor& operator=(cMemoryMonitor&&) = delete;		//ムーブ代入演算子も delete 指定

	static cMemoryMonitor& GetInstance() {
		static cMemoryMonitor inst;			// privateなコンストラクタを呼び出す
		return inst;
	}
	MemoryData GetMemoryData()const;

private:
	void Update();
	cMemoryMonitor();						//コンストラクタを private に置く
	~cMemoryMonitor() = default;	//デストラクタを private に置く
	MemoryData m_MemoryData;		//メモリ情報を保持する
};