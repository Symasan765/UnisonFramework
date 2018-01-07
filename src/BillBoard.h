/*=================================================
//								FILE : BillBoard.h
//		ファイル説明 :
//		3D空間上で常にカメラを向き続ける2Dテクスチャを表示する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include <string>
#include "WinMain.h"
#include "Camera.h"
#include "MatrixCoord.h"
#include "DeferredConst.h"
#include "TextureManager.h"

/// <summary>
/// ビルボードを取り扱うクラス
/// </summary>
class cBillBoard : public cMatrix {
public:
	cBillBoard();
	cBillBoard(std::string _fileName);		//LoadDataを同時に呼び出すコンストラクタ
	~cBillBoard() = default;
	void LoadData(std::string _fileName);	//テクスチャのファイル名を指定する
	void Draw();
	void SetEmission(DirectX::XMFLOAT4 _color);	//自発光色を設定する
	void SetRotate(const float damy) {};			//ビルボードの回転関数は封印する
	void Rotation(const float, const float, const float) {};
private:
	HRESULT CreateVertexBuf();					//ビルボードクラスで使いまわす
	void SetConstant();							//保存されたデータを定数バッファに設定する
	std::shared_ptr<Texture2D> m_pTex;		//表示させるテクスチャを保持する
	cDeferredConst m_Constant;				//定数バッファ
	int m_ShaderType;								//ビット演算したシェーダ番号を格納する
	static ID3D11Buffer* m_VertexBuf;	// TODO 解放処理をなんとかしていれる。今のままだとデストラクタにいれるとビルボードを一つ削除すると不具合につながる
};