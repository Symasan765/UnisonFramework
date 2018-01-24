/*=================================================
//								FILE : HighlightBloom.h
//		ファイル説明 :
//		ハイライト部分へのブルーム処理を施す
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "RenderTarget.h"

/// <summary>
/// ブルームを作成するクラス
/// </summary>
class cHighlightBloom {
public:
	cHighlightBloom(const cHighlightBloom&) = delete;			//コピーコンストラクタを delete 指定
	cHighlightBloom& operator=(const cHighlightBloom&) = delete;	//コピー代入演算子も delete 指定
	cHighlightBloom(cHighlightBloom&&) = delete;			//ムーブコンストラクタを delete 指定
	cHighlightBloom& operator=(cHighlightBloom&&) = delete;		//ムーブ代入演算子も delete 指定

	static cHighlightBloom& GetInstance() {
		static cHighlightBloom inst;			// privateなコンストラクタを呼び出す
		return inst;
	}

	void DrawBloom(ID3D11ShaderResourceView* _EmiTex);		//自発光色が格納されたテクスチャを渡す
	ID3D11ShaderResourceView* GetBloomResourceView();
private:
	cHighlightBloom();					//コンストラクタを private に置く
	~cHighlightBloom();					//デストラクタを private に置く


	cRenderTargetTex* m_pBoloomTex;		//出来たブルームのテクスチャを格納するレンダーターゲットクラス
};